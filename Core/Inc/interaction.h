#ifndef __INTERACTION_H
#define __INTERACTION_H

#include "main.h"
#include "animation.h" 
#include "emotion.h"

typedef enum {
    ACTION_NONE  = 0x00,
    ACTION_WAVE  = 0x01,
    ACTION_PUNCH = 0x04,
    ACTION_POINT = 0x05,
    ACTION_HEART = 0x06
} GestureActionDef;

/**
 * @brief  初始化交互通信模块
 * @param  huart 绑定的串口句柄指针 (如 &huart1)
 * @retval None
 * @note   启动单字节串口中断接收状态机，需在 main 的 while(1) 前调用
 */
void Interaction_Init(UART_HandleTypeDef *huart);

/**
 * @brief  交互与表情切换逻辑更新 (对外接口)
 * @param  None
 * @retval None
 * @note   获取当前解析出的手势动作，并结合当前状态修改桌宠的表情状态，重置动画帧。
 * 该函数应放入 main.c 的非阻塞时间片中定期调用。
 */
void Interaction_Update(void);

#endif 