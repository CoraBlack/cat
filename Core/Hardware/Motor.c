#include "Motor.h"
#include "main.h"
#include "stm32_hal_legacy.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_tim.h"

// @arg froward 1
// @arg back 2
// @arg TurnLeft 3
// @arg TurnRight 4

int current_move_direction = 0;   



int Is_Front(char dir){
    if (dir == 'R'){
        return (HAL_GPIO_ReadPin(Motor_Right_BIN1_GPIO_Port, Motor_Right_BIN2_Pin) == GPIO_PIN_SET 
        && HAL_GPIO_ReadPin(Motor_Right_BIN2_GPIO_Port, Motor_Right_BIN2_Pin) == GPIO_PIN_RESET);
    }
    if (dir == 'L'){
        return (HAL_GPIO_ReadPin(Motor_Left_AIN1_GPIO_Port, Motor_Left_AIN1_Pin) == GPIO_PIN_SET 
        && HAL_GPIO_ReadPin(Motor_Left_AIN2_GPIO_Port, Motor_Left_AIN2_Pin) == GPIO_PIN_RESET);
    }
}

int Is_Back(char dir){
    if (dir == 'R'){
        return (HAL_GPIO_ReadPin(Motor_Right_BIN1_GPIO_Port, Motor_Right_BIN2_Pin) == GPIO_PIN_RESET 
        && HAL_GPIO_ReadPin(Motor_Right_BIN2_GPIO_Port, Motor_Right_BIN2_Pin) == GPIO_PIN_SET);
    }
    if (dir == 'L'){
        return (HAL_GPIO_ReadPin(Motor_Left_AIN1_GPIO_Port, Motor_Left_AIN1_Pin) == GPIO_PIN_RESET 
        && HAL_GPIO_ReadPin(Motor_Left_AIN2_GPIO_Port, Motor_Left_AIN2_Pin) == GPIO_PIN_SET);
    }
}

int Is_Stop(char dir){
     if (dir == 'R'){
        return (HAL_GPIO_ReadPin(Motor_Right_BIN1_GPIO_Port, Motor_Right_BIN2_Pin) 
        == HAL_GPIO_ReadPin(Motor_Right_BIN2_GPIO_Port, Motor_Right_BIN2_Pin));
    }
    if (dir == 'L'){
        return (HAL_GPIO_ReadPin(Motor_Left_AIN1_GPIO_Port, Motor_Left_AIN1_Pin)  
        == HAL_GPIO_ReadPin(Motor_Left_AIN2_GPIO_Port, Motor_Left_AIN2_Pin) );
    }
}

float min(float a,float b){
    if (a > b){
        return b;
    }
    else{
        return a;
    }
}

void Motor_Back(){
    HAL_GPIO_WritePin(Motor_Left_AIN1_GPIO_Port, Motor_Left_AIN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Motor_Left_AIN2_GPIO_Port, Motor_Left_AIN2_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(Motor_Right_BIN1_GPIO_Port, Motor_Right_BIN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Motor_Right_BIN2_GPIO_Port, Motor_Right_BIN2_Pin, GPIO_PIN_SET);
}

void Motor_Forward(){
    
    HAL_GPIO_WritePin(Motor_Left_AIN1_GPIO_Port, Motor_Left_AIN1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(Motor_Left_AIN2_GPIO_Port, Motor_Left_AIN2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Motor_Right_BIN1_GPIO_Port, Motor_Right_BIN1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(Motor_Right_BIN2_GPIO_Port, Motor_Right_BIN2_Pin, GPIO_PIN_RESET);
}

void Motor_TurnLeft(int Steering_Angle){

    HAL_GPIO_WritePin(Motor_Left_AIN1_GPIO_Port, Motor_Left_AIN1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(Motor_Left_AIN2_GPIO_Port, Motor_Left_AIN2_Pin, GPIO_PIN_SET);
    Motor_Stop();
    

}

void Motor_TurnRight(int Steering_Angle){
   
    HAL_GPIO_WritePin(Motor_Right_BIN1_GPIO_Port, Motor_Right_BIN1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(Motor_Right_BIN2_GPIO_Port, Motor_Right_BIN2_Pin, GPIO_PIN_SET);
    Motor_Stop();
}


void  Motor_Stop(){
    HAL_GPIO_WritePin(Motor_Left_AIN1_GPIO_Port, Motor_Left_AIN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Motor_Left_AIN2_GPIO_Port, Motor_Left_AIN2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Motor_Right_BIN1_GPIO_Port, Motor_Right_BIN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Motor_Right_BIN2_GPIO_Port, Motor_Right_BIN2_Pin, GPIO_PIN_RESET);

}

void Motor_Set_Speed(float Speed){
    if (Speed > 0){
        __HAL_TIM_SetCompare(htim3,TIM_CHANNEL_1,min(Speed / htim2.Instance->ARR,1.0))
    }
    
}

