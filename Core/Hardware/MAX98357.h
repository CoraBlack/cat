#ifndef __MAX98357_H
#define __MAX98357_H

#include <stdint.h>

/*
 * MAX98357 单声道功放驱动 (I2S4 / SPI4, Master TX, DMA circular)
 * 播放格式：PCM16LE / 24kHz / 单声道(mono)
 *
 * 噪声控制（硬件 shutdown 门控）：
 *  - 功放 SD_MODE 由 Speaker_CTL(PC14) 控制：低=shutdown(静音)，高=使能。
 *  - 默认关断。麦克风(I2S3)运行时功放处于 shutdown，不会放大共电源/地耦合噪声。
 *  - 仅在播放时 MAX98357_Start() 使能功放并启动 I2S4；MAX98357_Stop() 关断并停 DMA。
 *
 * I2S 帧约束：Philips 标准每 WS 周期硬件固定发 L+R 两个时隙，故仅在填充 DMA
 * 缓冲的最末端把每个 mono 样本复制进 L=R（硬件要求，对外/内部仍全程 mono）。
 */

/* 一个 ESP 桥接帧的 mono 样本数 (1280B / 2)。 */
#define MAX98357_FRAME_SAMPLES 640u

/** @brief 初始化：配置 Speaker_CTL(PC14) 推挽输出、功放默认关断。开机调用一次。 */
void MAX98357_Init(void);

/** @brief 开始播放：启动 I2S4 循环 DMA(先静音) 并使能功放。进入 PLAY 相位时调用。 */
void MAX98357_Start(void);

/** @brief 停止播放：关断功放(shutdown) 并停止 I2S4 DMA。回到 RECORD/空闲时调用。 */
void MAX98357_Stop(void);

/**
 * @brief 压入待播放的 mono PCM16 样本（24kHz）。须在 Start 之后调用。
 * @param pcm 指向 mono int16 样本
 * @param samples 样本个数
 * @return 实际写入的样本数（环形缓冲满时可能小于 samples）
 */
uint32_t MAX98357_WritePCM(const int16_t *pcm, uint32_t samples);

/** @brief 环形缓冲当前可写入的 mono 样本数（用于流控）。 */
uint32_t MAX98357_GetFreeSamples(void);

/** @brief 丢弃缓冲中未播放数据（输出转为静音，功放保持使能）。 */
void MAX98357_Mute(void);

/**
 * @brief 单元测试：使能功放→播放约 2 秒 1kHz 正弦音(阻塞)→关断功放。
 *        能听到纯音即证明 I2S4→功放链路正常。
 */
void MAX98357_SelfTest(void);

#endif
