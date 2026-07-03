#include "IR.h"
#include "main.h"
#include "stm32f4xx_hal_gpio.h"
#include "Motor.h"
#define Steering 20
void LeftFront(){
    Motor_Back();
    Motor_TurnRight(Steering);
    Motor_Back();
    Motor_TurnLeft(Steering);

    
}

void LeftReap(){
    
    Motor_TurnRight(Steering);
    Motor_TurnRight(Steering);
    
}

void RightFront(){
    
    Motor_TurnLeft(Steering);
    Motor_TurnRight(Steering);
    
}

void RightReap(){
    Motor_Back();
    Motor_TurnLeft(Steering);
    Motor_Back();
    Motor_TurnLeft(Steering);
    
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
    switch (GPIO_Pin) {
        case LeftFront_Pin:
            LeftFront();
            break;
        case LeftReap_Pin:
            LeftReap();
            break;
        case RightFront_Pin:
            
            break;
        case RightReap_Pin:
            break;
    }

    
}