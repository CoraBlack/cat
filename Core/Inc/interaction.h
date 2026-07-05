#ifndef INTERACTION_H_
#define INTERACTION_H_

#include "main.h"
#include "animation.h" 
#include "emotion.h"

// 优化后的手势枚举，包含详细中文注释
typedef enum {
    ACTION_NONE          = 0x00, // [无动作] 没有检测到手势，或手势不明确
    ACTION_WAVE_HAND     = 0x01, // [挥手] 五指张开左右摆动 (类似打招呼 "Hi~")
    ACTION_FIST_PUNCH    = 0x04, // [出拳/握拳] 握紧拳头 (用拳头正面挥向摄像头)
    ACTION_POINT_FINGER  = 0x05, // [食指指认] 仅伸出食指指着前方 (类似指着某人)
    ACTION_FINGER_HEART  = 0x06  // [手指比心] 拇指和食指交叉 (流行的韩式单手小爱心)
} GestureActionDef;

/**
 * @brief  初始化交互通信模块
 */
void Interaction_Init(UART_HandleTypeDef *huart);

/**
 * @brief  交互与表情切换逻辑更新 (对外接口)
 */
void Interaction_Update(void);

/**
 * @brief  查询当前解析出的手势动作 (对外接口)
 * @retval GestureActionDef 当前的手势动作枚举
 */
GestureActionDef Interaction_GetCurrentAction(void);

/**
 * @brief  查询当前解析出的 X 轴偏移量 (新增对外接口)
 * @retval int8_t 偏移量数值 (-100 到 100，左正右负)
 */
int8_t Interaction_GetXOffset(void);

#endif /* INTERACTION_H_ */