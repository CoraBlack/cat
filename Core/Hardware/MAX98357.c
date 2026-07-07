#include "MAX98357.h"
#include "cJSON.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2s.h"
#include <stdint.h>
#include "base64.h"


/* MAX98357.c */
#define MAX_SAMPLES 1280
uint16_t I2s_TX_Buffer[MAX_SAMPLES];
extern I2S_HandleTypeDef hi2s4;

// 仅用于首次启动
void MAX98357_I2S_Start(void){
    HAL_I2S_Transmit_DMA(&hi2s4, I2s_TX_Buffer, MAX_SAMPLES);
}

// DMA 传输完成回调（自动双缓冲交替）
void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s)
{
    if (hi2s->Instance == SPI4)
    {
        // ✅ 在这里填数据！DMA 刚把这块搬走，现在填绝对安全
        for (uint32_t i = 0; i < MAX_SAMPLES; i++) {
            // 16B_EXTENDED 模式下，直接填 16bit 值即可
            I2s_TX_Buffer[i] = 0x55AA; 
        }
    }
}

// 半传输完成回调（可选，为了无缝衔接最好也加上）
void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
    if (hi2s->Instance == SPI4)
    {
        for (uint32_t i = 0; i < MAX_SAMPLES / 2; i++) {
            I2s_TX_Buffer[i] = 0x55AA; 
        }
    }
}