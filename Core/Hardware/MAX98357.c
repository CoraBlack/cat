#include "MAX98357.h"
#include "cJSON.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2s.h"
#include <stdint.h>
#include "base64.h"


#define MAX_SAMPLES 1500
extern I2S_HandleTypeDef hi2s3;
extern char * json_rx_buffer;
static uint32_t I2s_TX_Buffer[MAX_SAMPLES];
static volatile uint32_t sample_count = 0;

static uint32_t _get_str_len(const char *str) {
    if (str == NULL) return 0;
    uint32_t len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}


void Get_audio_data(){

    cJSON * root = cJSON_Parse(json_rx_buffer);
    if (root != NULL){
        static uint8_t base64_buffer[MAX_SAMPLES * 3];
        int len = base64_decode_safe(root->valuestring, _get_str_len(root->valuestring), base64_buffer, sizeof(base64_buffer));
        sample_count = (uint32_t)(len / 3);
        if (sample_count > MAX_SAMPLES) sample_count = MAX_SAMPLES;
        for (int i = 0;i < sample_count;i++){
            int index = i* 3;
            uint32_t raw_data = base64_buffer[index] << 16 | base64_buffer[index + 1] << 8 | base64_buffer[index + 2];
            if (raw_data & 0x00800000){
                raw_data |= 0xFF000000;
            }
            
            I2s_TX_Buffer[i] = raw_data;
        }
        cJSON_Delete(root);

    }
    
   
}


void MAX98357_I2S_Send(){
    HAL_I2S_Transmit_DMA(&hi2s3, (uint16_t*)I2s_TX_Buffer, sample_count);
}