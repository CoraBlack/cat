#include "main.h"

#include "Motor.h"
#include "OLED.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_tim.h"
#include <stdint.h>

#define Delay_Time 500
#define MIN_DISTANCE 100
#define INVALID_DIST 65535U

extern TIM_HandleTypeDef htim3;

volatile uint32_t read_last_update_tick = 0;

volatile uint8_t read_flag = 0;

void Distance_Sensor_Init(){
   HAL_TIM_IC_Start(&htim3, TIM_CHANNEL_1); 
   
}

void Distance_sensor_start(){
    HAL_GPIO_WritePin(Distance_Trig_GPIO_Port, Distance_Trig_Pin, GPIO_PIN_SET);
    for (volatile uint32_t i = 0; i < 3360; i++);
    HAL_GPIO_WritePin(Distance_Trig_GPIO_Port, Distance_Trig_Pin, GPIO_PIN_RESET);


    
    
    
}

uint16_t Distance_Sensor_Figure_Distance(){
    return (HAL_TIM_ReadCapturedValue(&htim3, TIM_CHANNEL_1)* 17 + 5) / 100 ;
}

uint16_t Distance_Sensor_Get_Distance(void){
    uint32_t now = HAL_GetTick();
    

    if (read_flag == 0 && (now - read_last_update_tick >= 70U)) {

        Distance_sensor_start();
        read_last_update_tick = now;
        read_flag = 1; 
        return INVALID_DIST; 
        
    } else if (read_flag == 1 && (now - read_last_update_tick >= 40U)) {

        read_flag = 0;
        return Distance_Sensor_Figure_Distance();
    }
    
    // 其他时间（等待期间）返回无效值
    return INVALID_DIST;
}


uint8_t Is_Near(){
    
    uint16_t distance = Distance_Sensor_Get_Distance();
    
    if (distance < MIN_DISTANCE && distance != INVALID_DIST){
        return 1;
    }
    else{
        return 0;
    }
    
}

