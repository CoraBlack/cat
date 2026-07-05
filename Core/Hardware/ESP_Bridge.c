#include "ESP_Bridge.h"
#include "main.h"
#include "stm32f4xx_hal.h"
#include "animation.h" /* get_pet_emotion / EmotionTypeDef */
#include <string.h>

extern SPI_HandleTypeDef hspi2;

/* SPI2_TX = DMA1_Stream4 Channel0（RX 已由 CubeMX 配为 Stream3）。 */
static DMA_HandleTypeDef hdma_spi2_tx;
static volatile uint8_t  frame_done = 0;
static volatile EspPhase g_phase = ESP_PHASE_RECORD;

/* DMA 收发用的临时缓冲（内部 SRAM，DMA 可用）。 */
static uint8_t tx_scratch[ESP_SPI_FRAME_BYTES];
static uint8_t rx_scratch[ESP_SPI_FRAME_BYTES];

/* CS 低有效：空闲高、传输期间拉低整帧。 */
#define ESP_CS_LOW()  HAL_GPIO_WritePin(ESP_Bridge_CS_GPIO_Port, ESP_Bridge_CS_Pin, GPIO_PIN_RESET)
#define ESP_CS_HIGH() HAL_GPIO_WritePin(ESP_Bridge_CS_GPIO_Port, ESP_Bridge_CS_Pin, GPIO_PIN_SET)

/* 单帧超时（1280B @ ~3MHz ≈ 3.4ms）。 */
#define ESP_SPI_TIMEOUT_MS 100u

void ESP_Bridge_Init(void) {
    ESP_CS_HIGH();

    /* 建立 SPI2 TX DMA 并链接到 hspi2（CubeMX 只配了 RX）。 */
    __HAL_RCC_DMA1_CLK_ENABLE();
    hdma_spi2_tx.Instance                 = DMA1_Stream4;
    hdma_spi2_tx.Init.Channel             = DMA_CHANNEL_0;
    hdma_spi2_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    hdma_spi2_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_spi2_tx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_spi2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_spi2_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    hdma_spi2_tx.Init.Mode                = DMA_NORMAL;
    hdma_spi2_tx.Init.Priority            = DMA_PRIORITY_LOW;
    hdma_spi2_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
    HAL_DMA_Init(&hdma_spi2_tx);
    __HAL_LINKDMA(&hspi2, hdmatx, hdma_spi2_tx);
    HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);

    /* 握手线改双边沿 EXTI（CubeMX 默认只上升沿，收不到 Play→Record）。 */
    GPIO_InitTypeDef gi = {0};
    gi.Pin  = ESP_Bridge_HS_Pin;
    gi.Mode = GPIO_MODE_IT_RISING_FALLING;
    gi.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ESP_Bridge_HS_GPIO_Port, &gi);
    HAL_NVIC_SetPriority(ESP_Bridge_HS_EXTI_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(ESP_Bridge_HS_EXTI_IRQn);

    ESP_Bridge_HS_ISR(); /* 读取初始相位 */
}

static int emotion_to_code(EmotionTypeDef e) {
    switch (e) {
        case Emotion_Normal:  return 0;
        case Emotion_Happy:   return 1;
        case Emotion_Alert:   return 2;
        case Emotion_Angry:   return 3;
        case Emotion_Curious: return 4;
        case Emotion_Shy:     return 5;
        default:              return 0;
    }
}

void ESP_Bridge_Start(void) {
    int code = emotion_to_code(get_pet_emotion());

    /* 构建控制帧：[0x01][code][0x00][0x00] + 1276B 补零。 */
    uint8_t ctrl[ESP_SPI_FRAME_BYTES];
    memset(ctrl, 0, sizeof(ctrl));
    ctrl[0] = 0x01;
    ctrl[1] = (uint8_t)code;
    ESP_Bridge_SendMic((const int16_t *)ctrl);
}

void ESP_Bridge_HS_ISR(void) {
    g_phase = (HAL_GPIO_ReadPin(ESP_Bridge_HS_GPIO_Port, ESP_Bridge_HS_Pin) == GPIO_PIN_SET)
                  ? ESP_PHASE_PLAY
                  : ESP_PHASE_RECORD;
}

EspPhase ESP_Bridge_Phase(void) {
    return g_phase;
}

/* SPI2 TX DMA 中断（CubeMX 未生成，此处覆盖启动文件的弱符号）。 */
void DMA1_Stream4_IRQHandler(void) {
    HAL_DMA_IRQHandler(&hdma_spi2_tx);
}

/* 全双工 DMA 传输完成：拉高 CS、置完成标志。 */
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi) {
    if (hspi->Instance == SPI2) {
        ESP_CS_HIGH();
        frame_done = 1;
    }
}

static int esp_xfer(const uint8_t *tx, uint8_t *rx) {
    frame_done = 0;
    ESP_CS_LOW();
    if (HAL_SPI_TransmitReceive_DMA(&hspi2, (uint8_t *)tx, rx, ESP_SPI_FRAME_BYTES) != HAL_OK) {
        ESP_CS_HIGH();
        return -1;
    }
    uint32_t t0 = HAL_GetTick();
    while (!frame_done) {
        if (HAL_GetTick() - t0 > ESP_SPI_TIMEOUT_MS) {
            HAL_SPI_DMAStop(&hspi2);
            ESP_CS_HIGH();
            return -1;
        }
    }
    return 0;
}

int ESP_Bridge_SendMic(const int16_t *pcm) {
    if (pcm == NULL) {
        return -1;
    }
    return esp_xfer((const uint8_t *)pcm, rx_scratch);
}

int ESP_Bridge_RecvPlay(int16_t *pcm) {
    if (pcm == NULL) {
        return -1;
    }
    return esp_xfer(tx_scratch, (uint8_t *)pcm);
}

void ESP_Bridge_Pack16(const int32_t *in24, int16_t *out16, uint32_t count) {
    if (in24 == NULL || out16 == NULL) {
        return;
    }
    for (uint32_t i = 0; i < count; ++i) {
        out16[i] = (int16_t)(in24[i] >> 8); /* 24-bit → 16-bit：取高 16 位 */
    }
}

void ESP_Bridge_SelfTest(void) {
    static uint8_t seed = 0;
    /* 仅在 Record 相位发送递增图案帧（ESP 自测模式据此校验）。 */
    if (ESP_Bridge_Phase() != ESP_PHASE_RECORD) {
        return;
    }
    for (uint32_t i = 0; i < ESP_SPI_FRAME_BYTES; ++i) {
        tx_scratch[i] = (uint8_t)(seed + i);
    }
    ESP_Bridge_SendMic((const int16_t *)tx_scratch);
    ++seed;
}

/* 每帧 640 样本 @16k ≈ 40ms：按实时节奏发送，避免 ESP 端麦克风环形缓冲溢出。 */
#define ESP_TESTCHAT_FRAME_MS         40u
/* 静音尾端帧数 ≈ 1.2s，须 > ESP 端 VAD 挂起时长(默认 600ms)。 */
#define ESP_TESTCHAT_TAIL_FRAMES      30u
/* 等待后端 STT→LLM→TTS 出首帧的上限。 */
#define ESP_TESTCHAT_REPLY_TIMEOUT_MS 15000u

void ESP_Bridge_TestChat(const int16_t *pcm, uint32_t samples) {
    if (pcm == NULL || samples == 0) {
        return;
    }

    int16_t frame[ESP_SPI_FRAME_SAMPLES];

    /* 0) 先发情绪控制帧开启回合（等同唤醒动作）。 */
    ESP_Bridge_Start();

    /* 1) 发送自录语音（末帧补零对齐到整帧）。 */
    uint32_t idx = 0;
    while (idx < samples) {
        uint32_t n = samples - idx;
        if (n > ESP_SPI_FRAME_SAMPLES) {
            n = ESP_SPI_FRAME_SAMPLES;
        }
        memcpy(frame, &pcm[idx], n * sizeof(int16_t));
        if (n < ESP_SPI_FRAME_SAMPLES) {
            memset(&frame[n], 0, (ESP_SPI_FRAME_SAMPLES - n) * sizeof(int16_t));
        }
        if (ESP_Bridge_SendMic(frame) != 0) {
            return;
        }
        idx += n;
        HAL_Delay(ESP_TESTCHAT_FRAME_MS);
    }

    /* 2) 长静音尾端：触发 ESP 端 VAD 判定说话结束。 */
    memset(frame, 0, sizeof(frame));
    for (uint32_t f = 0; f < ESP_TESTCHAT_TAIL_FRAMES; ++f) {
        if (ESP_Bridge_SendMic(frame) != 0) {
            return;
        }
        HAL_Delay(ESP_TESTCHAT_FRAME_MS);
    }

    /* 3) 等 ESP 完成上传+推理并切到 Play（握手拉高）。 */
    uint32_t t0 = HAL_GetTick();
    while (ESP_Bridge_Phase() != ESP_PHASE_PLAY) {
        if (HAL_GetTick() - t0 > ESP_TESTCHAT_REPLY_TIMEOUT_MS) {
            return; /* 后端超时 */
        }
    }

    /* 4) 接收下行播放帧，直到 ESP 回到 Record（播放结束）。 */
    while (ESP_Bridge_Phase() == ESP_PHASE_PLAY) {
        if (ESP_Bridge_RecvPlay(frame) != 0) {
            break;
        }
        /* 播放交由音频层处理；测试仅负责接收。 */
    }
}
