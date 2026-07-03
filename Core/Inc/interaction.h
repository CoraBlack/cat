#ifndef __INTERACTION_H
#define __INTERACTION_H

#include "main.h"
#include "animation.h" 
#include "emotion.h"

typedef enum {
    ACTION_NONE         = 0x00, // 无动作
    ACTION_WAVE_HAND    = 0x01, // 挥手 (比单纯的 WAVE 更明确是手部动作)
    ACTION_PUNCH        = 0x04, // 出拳 (原 0x04 保留)
    ACTION_POINTING     = 0x05, // 用手指 (Pointing 明确表示“正在指着”的动作)
    ACTION_FINGER_HEART = 0x06  // 比心 (Finger Heart 是国际通用的“单手比心”英文)
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

/**
 * @brief  查询当前解析出的手势动作 (对外接口)qq
 * @param  None
 * @retval GestureActionDef 当前解析出的手势动作(没动作也是一种动作哦)
 * @note   获取当前解析出的手势动作
 */
GestureActionDef Interaction_GetCurrentAction(void);

/**
 * @brief  查询当前解析出的 X 轴偏移量 (对外接口)
 * @param  None
 * @retval int8_t 当前解析出的 X 轴偏移量 (左正右负)
 */
int8_t Interaction_GetXOffset(void);
#endif 