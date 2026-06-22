#include "Motor.h"
#include "main.h"
#include "stm32_hal_legacy.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_tim.h"


void Motor_Back(){
    HAL_GPIO_WritePin(Motor_Left_AIN1_GPIO_Port, Motor_Left_AIN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Motor_Left_AIN2_GPIO_Port, Motor_Left_AIN2_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(Motor_Right_BIN1_GPIO_Port, Motor_Right_BIN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Motor_Right_BIN2_GPIO_Port, Motor_Right_BIN2_Pin, GPIO_PIN_SET);
}

void Motor_Forward(){
    HAL_GPIO_WritePin(Motor_Left_AIN1_GPIO_Port, Motor_Left_AIN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Motor_Left_AIN2_GPIO_Port, Motor_Left_AIN2_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(Motor_Right_BIN1_GPIO_Port, Motor_Right_BIN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Motor_Right_BIN2_GPIO_Port, Motor_Right_BIN2_Pin, GPIO_PIN_SET);
}

void Motor_Turn_Left(int Steering){
    HAL_GPIO_WritePin(Motor_Left_AIN1_GPIO_Port, Motor_Left_AIN1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(Motor_Left_AIN2_GPIO_Port, Motor_Left_AIN2_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(Motor_Right_BIN1_GPIO_Port, Motor_Right_BIN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Motor_Right_BIN2_GPIO_Port, Motor_Right_BIN2_Pin, GPIO_PIN_SET);

}

void Motor_Turn_Right(int Steering){
    HAL_GPIO_WritePin(Motor_Left_AIN1_GPIO_Port, Motor_Left_AIN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Motor_Left_AIN2_GPIO_Port, Motor_Left_AIN2_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(Motor_Right_BIN1_GPIO_Port, Motor_Right_BIN1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(Motor_Right_BIN2_GPIO_Port, Motor_Right_BIN2_Pin, GPIO_PIN_SET);

}


void  Motor_Stop(){
    HAL_GPIO_WritePin(Motor_Left_AIN1_GPIO_Port, Motor_Left_AIN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Motor_Left_AIN2_GPIO_Port, Motor_Left_AIN2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Motor_Right_BIN1_GPIO_Port, Motor_Right_BIN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Motor_Right_BIN2_GPIO_Port, Motor_Right_BIN2_Pin, GPIO_PIN_RESET);
}

