#include "MAX98357.h"
#include "main.h"          /* Speaker_CTL_Pin / Speaker_CTL_GPIO_Port (CubeMX 生成) */
#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <string.h>

/*
 * 缓冲结构（详见 MAX98357.h）：
 *
 *   MAX98357_WritePCM(mono) ─► pcm_ring[mono, SPSC] ─┐
 *                                                    │(DMA 回调消费, 每样本 L=R)
 *   i2s_tx_buf[两个 slot, 交织 L/R] ◄────────────────┘
 *        │ circular DMA @24kHz
 *        ▼
 *      MAX98357 功放（单声道，Speaker_CTL 门控使能）
 */

extern I2S_HandleTypeDef hi2s4;

/* 一帧 mono 展开为立体声后的 int16 字数 (L+R)。 */
#define DMA_SLOT_WORDS (MAX98357_FRAME_SAMPLES * 2u)   /* 1280 */
/* DMA 双缓冲：两个 slot，供半/全传输回调交替刷新。 */
#define DMA_BUF_WORDS  (DMA_SLOT_WORDS * 2u)           /* 2560 */
/* mono 暂存环：4 帧深(~106ms)，吸收主循环抖动。 */
#define RING_SAMPLES   (MAX98357_FRAME_SAMPLES * 4u)   /* 2560 */

/* 功放 shutdown 门控：低=关断(静音)，高=使能。 */
#define SPK_ENABLE()  HAL_GPIO_WritePin(Speaker_CTL_GPIO_Port, Speaker_CTL_Pin, GPIO_PIN_SET)
#define SPK_DISABLE() HAL_GPIO_WritePin(Speaker_CTL_GPIO_Port, Speaker_CTL_Pin, GPIO_PIN_RESET)

/* 使能/关断功放前后等待时钟稳定、抑制 pop 的延时(ms)。 */
#define SPK_SETTLE_MS 2u

static int16_t i2s_tx_buf[DMA_BUF_WORDS];
static int16_t pcm_ring[RING_SAMPLES];

/* SPSC 无锁环：生产者(线程)只改 ring_head，消费者(DMA中断)只改 ring_tail。 */
static volatile uint32_t ring_head;
static volatile uint32_t ring_tail;
static volatile uint8_t  mute_req;
static volatile uint8_t  running;

static uint32_t ring_count(uint32_t head, uint32_t tail) {
    return (head - tail + RING_SAMPLES) % RING_SAMPLES;
}

/* 刷新一个 slot：从环形缓冲取 mono 样本，逐样本复制成 L=R；缺数据补零(静音)。 */
static void max_fill_slot(int16_t *slot) {
    uint32_t head = ring_head;
    uint32_t tail = ring_tail;

    if (mute_req) {
        tail = head;      /* 消费者拥有 tail，可安全清空 */
        mute_req = 0;
    }

    for (uint32_t i = 0; i < MAX98357_FRAME_SAMPLES; ++i) {
        int16_t s = 0;
        if (tail != head) {
            s = pcm_ring[tail];
            tail = (tail + 1u) % RING_SAMPLES;
        }
        slot[2u * i]      = s;   /* L */
        slot[2u * i + 1u] = s;   /* R */
    }

    ring_tail = tail;            /* 发布消费进度 */
}

void MAX98357_Init(void) {
    SPK_DISABLE();               /* 功放默认关断（麦克风运行时也不放大耦合噪声） */
    running = 0;
    ring_head = 0;
    ring_tail = 0;
    mute_req = 0;
}

void MAX98357_Start(void) {
    if (running) {
        return;
    }
    ring_head = 0;
    ring_tail = 0;
    mute_req  = 0;
    memset(i2s_tx_buf, 0, sizeof(i2s_tx_buf));               /* 起播先填静音 */

    if (HAL_I2S_Transmit_DMA(&hi2s4, (uint16_t *)i2s_tx_buf, DMA_BUF_WORDS) != HAL_OK) {
        return;
    }
    running = 1;
    HAL_Delay(SPK_SETTLE_MS);    /* 等 BCLK 稳定，功放锁定时钟 */
    SPK_ENABLE();                /* 后使能功放，抑制起播 pop */
}

void MAX98357_Stop(void) {
    if (!running) {
        return;
    }
    SPK_DISABLE();               /* 先关断功放，抑制停时钟 pop */
    HAL_Delay(SPK_SETTLE_MS);
    HAL_I2S_DMAStop(&hi2s4);
    running = 0;
}

uint32_t MAX98357_WritePCM(const int16_t *pcm, uint32_t samples) {
    if (pcm == NULL || samples == 0u) {
        return 0u;
    }

    uint32_t head = ring_head;
    uint32_t tail = ring_tail;                       /* 快照 */
    uint32_t space = (RING_SAMPLES - 1u) - ring_count(head, tail);
    if (samples > space) {
        samples = space;
    }

    for (uint32_t i = 0; i < samples; ++i) {
        pcm_ring[head] = pcm[i];
        head = (head + 1u) % RING_SAMPLES;
    }
    ring_head = head;                                /* 发布写入 */
    return samples;
}

uint32_t MAX98357_GetFreeSamples(void) {
    return (RING_SAMPLES - 1u) - ring_count(ring_head, ring_tail);
}

void MAX98357_Mute(void) {
    mute_req = 1;                                    /* 由 DMA 回调执行清空 */
}

/* 半传输完成：slot0 已播完，刷新 [0 .. DMA_SLOT_WORDS-1]。 */
void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s) {
    if (hi2s->Instance == SPI4) {
        max_fill_slot(&i2s_tx_buf[0]);
    }
}

/* 全传输完成：slot1 已播完，刷新 [DMA_SLOT_WORDS .. DMA_BUF_WORDS-1]。 */
void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s) {
    if (hi2s->Instance == SPI4) {
        max_fill_slot(&i2s_tx_buf[DMA_SLOT_WORDS]);
    }
}

/* 1kHz 正弦查表：24kHz 采样 → 每周期 24 点，幅度 ±8192（约 -12dBFS，防削顶）。 */
static const int16_t kSine1k[24] = {
        0,  2120,  4096,  5793,  7094,  7913,
     8192,  7913,  7094,  5793,  4096,  2120,
        0, -2120, -4096, -5793, -7094, -7913,
    -8192, -7913, -7094, -5793, -4096, -2120,
};

void MAX98357_SelfTest(void) {
    const uint32_t total = 24000u * 2u;   /* 2 秒 @24kHz */
    uint32_t sent = 0;
    uint32_t phase = 0;
    int16_t chunk[MAX98357_FRAME_SAMPLES];

    MAX98357_Start();

    while (sent < total) {
        uint32_t n = total - sent;
        if (n > MAX98357_FRAME_SAMPLES) {
            n = MAX98357_FRAME_SAMPLES;
        }
        for (uint32_t i = 0; i < n; ++i) {
            chunk[i] = kSine1k[phase];
            phase = (phase + 1u) % 24u;
        }

        uint32_t pushed = 0;
        while (pushed < n) {
            uint32_t w = MAX98357_WritePCM(&chunk[pushed], n - pushed);
            pushed += w;
            if (w == 0u) {
                HAL_Delay(1);   /* 环满：等 DMA 消费 */
            }
        }
        sent += n;
    }

    HAL_Delay(120);   /* 等尾部数据播空 */
    MAX98357_Stop();
}
