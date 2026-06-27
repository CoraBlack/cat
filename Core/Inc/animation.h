#ifndef ANIMATION_H
#define ANIMATION_H

#include "animation.h"
#include "oled.h"

typedef struct {
    const uint8_t **frame_list;
    uint8_t frame_count;
} AnimationTypedef;

typedef struct {
    uint8_t current_emotion;
    uint8_t previous_emotion;
    uint8_t frame_count;
} OledCurrentStateTypedef;
/**
 * @brief  更新 OLED 动画(唯一对外接口)
 * @param  oled OLED 设备对象指针
 * @retval None
 * @note   根据当前情绪状态切换动画帧，确保同一状态循环播放，不同状态切换时先播放完上一状态的动画再切换到新状态
 */
void animation_update(OledTypedef *oled);
#endif 

