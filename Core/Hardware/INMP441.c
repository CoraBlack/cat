#include "INMP441.h"
#include "cjson.h"
#include "base64.h"
#include <ctype.h>
#include <stdint.h>
#include "stm32f411xe.h"
#include "stm32f4xx_hal.h"
#include "animation.h"
#include "stm32f4xx_hal_gpio.h"
#include "TEXT_OLED.h"

#define AUDIO_BUFFER_SIZE_HALF_WORDS 1280    
volatile uint8_t audio_frame_ready = 0; 
uint16_t i2s_rx_buffer[AUDIO_BUFFER_SIZE_HALF_WORDS];
static int32_t left_channel_buf[AUDIO_BUFFER_SIZE_HALF_WORDS / 4];
extern I2S_HandleTypeDef hi2s3;

void INMP441_Init(){
    HAL_I2S_Receive_DMA(&hi2s3, i2s_rx_buffer, AUDIO_BUFFER_SIZE_HALF_WORDS / 2);
    
}

void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s) {
    if (hi2s->Instance == SPI3) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
        audio_frame_ready = 1; 
    }
    
}


cJSON *translate(int32_t *output_buffer){   
    static char base64_buffer[6832];
    base64_encode_safe((const uint8_t *)(output_buffer),AUDIO_BUFFER_SIZE_HALF_WORDS / 4 * sizeof(int32_t),base64_buffer,sizeof(base64_buffer));
    char emotion[20][20] = {"Normal","Happy","Alert","Sleep","Angry","Curious","Shy","Sad","Count"};
    char gesture[20][20] = {};
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "emotion", emotion[get_pet_emotion()]);
    cJSON_AddStringToObject(root, "gesture", gesture[0]);
    cJSON_AddStringToObject(root, "contents", base64_buffer);
    return root;
}



cJSON *Extract_Left_Channel() {
    int32_t *output_buffer = left_channel_buf;
    
    if (audio_frame_ready) {
        
        HAL_I2S_DMAStop(&hi2s3); 
        for (int i = 0; i < AUDIO_BUFFER_SIZE_HALF_WORDS / 4; i++) {
            int index = i * 4;
            // uint32_t raw_32bit = (i2s_rx_buffer[index + 1] << 16) | i2s_rx_buffer[index]; 
            uint32_t raw_32bit = (i2s_rx_buffer[index] << 16) | i2s_rx_buffer[index + 1]; 
            int32_t valid_audio = (int32_t)(raw_32bit >> 8); 
            if (raw_32bit & 0x80000000){
                valid_audio = valid_audio | 0xFF000000;
            }

    

            output_buffer[i] = valid_audio;
        }
        audio_frame_ready = 0;
        HAL_I2S_Receive_DMA(&hi2s3, i2s_rx_buffer, AUDIO_BUFFER_SIZE_HALF_WORDS / 2); // 重新启动
        return translate(output_buffer);
    }
    return NULL;
}
