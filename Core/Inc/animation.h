#ifndef ANIMATION_H
#define ANIMATION_H

#include "emotion.h"
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
 * @brief  更新 OLED 动画
 * @param  oled OLED 设备对象指针
 * @retval None
 * @note   根据当前情绪状态切换动画帧，确保同一状态循环播放，不同状态切换时先播放完上一状态的动画再切换到新状态（也就是说如果一秒内有多次切换表情以最后一次为准）
 */
void animation_update(OledTypedef *oled);

/**
 * @brief  供其他模块(如交互逻辑、串口)调用的表情设置接口
 * @param  new_emo 要切换的目标表情
 */
void set_pet_emotion(EmotionTypeDef new_emo);

/**
 * @brief  获取当前桌宠的目标表情，用于逻辑判断
 */
EmotionTypeDef get_pet_emotion(void);

#endif 

