#ifndef __ESP_SPI_H
#define __ESP_SPI_H

#include <stdint.h>

/* 与 ESP32-C3 从机约定：详见 PROTOCOL.md
 * SPI2 主机 / Mode0 / 8-bit / MSB / CS(PC7) 低有效 / 握手(PC6) 高有效
 * 定长帧 1280 字节 = 640 个 int16 单声道样本。
 * 上行麦克风 PCM16LE@16k 走 MOSI；下行播放 PCM16LE@24k 走 MISO。
 * 收发经 SPI2 DMA(全双工)，CS 在传输完成回调里拉高。 */
#define ESP_SPI_FRAME_BYTES   1280u
#define ESP_SPI_FRAME_SAMPLES (ESP_SPI_FRAME_BYTES / 2u)

typedef enum {
    ESP_PHASE_RECORD = 0, /* 握手低：STM 发麦克风 */
    ESP_PHASE_PLAY   = 1  /* 握手高：STM 读播放  */
} EspPhase;

/** @brief 初始化桥接：CS 置高、建立 SPI2 TX DMA、握手线改双边沿 EXTI、读初始相位。 */
void ESP_Bridge_Init(void);

/** @brief 返回当前相位（由握手线 EXTI 维护）。 */
EspPhase ESP_Bridge_Phase(void);

/** @brief 握手线 EXTI 事件处理：由 IR.c 的 HAL_GPIO_EXTI_Callback 转发调用。 */
void ESP_Bridge_HS_ISR(void);

/**
 * @brief 上行：CS 门控经 DMA 发送一帧麦克风 PCM16（ESP_SPI_FRAME_SAMPLES 个样本）。
 * @return 0 成功，-1 失败/超时。
 */
int ESP_Bridge_SendMic(const int16_t *pcm);

/**
 * @brief 下行：CS 门控经 DMA 接收一帧播放 PCM16（ESP_SPI_FRAME_SAMPLES 个样本）。
 * @return 0 成功，-1 失败/超时。
 */
int ESP_Bridge_RecvPlay(int16_t *pcm);

/**
 * @brief 将 INMP441 的 24-bit 有效值(存于 int32) 转为 16-bit PCM（取高 16 位）。
 */
void ESP_Bridge_Pack16(const int32_t *in24, int16_t *out16, uint32_t count);

/**
 * @brief 链路自测（阻塞、不依赖音频）：Record 相位下持续发送递增图案帧，
 *        供 ESP 侧自测模式校验 SPI 通信是否打通。在 main 的 while(1) 里调用。
 */
void ESP_Bridge_SelfTest(void);

/**
 * @brief 业务自测：把一段自录 PCM16(16kHz/单声道) 当作麦克风数据发给 ESP，
 *        末尾追加长静音尾端触发 ESP 端 VAD 判定说话结束，ESP 随后走完整业务
 *        流程（上传→STT→LLM→TTS）；本函数在 ESP 切到 Play 后接收下行播放帧
 *        （交给扬声器由音频层处理）。阻塞直到播放结束或超时。
 * @param pcm     16kHz 单声道 PCM16 样本（调用方提供自录音频）
 * @param samples 样本个数
 */
void ESP_Bridge_TestChat(const int16_t *pcm, uint32_t samples);

#endif
