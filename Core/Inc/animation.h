#ifndef ANIMATION_H_
#define ANIMATION_H_

#include <stdint.h>
#include "oled.h"
#include "emotion.h" 

typedef struct {
    const uint8_t **frame_list;
    uint8_t frame_count;
} AnimationTypedef;

typedef struct {
    EmotionTypeDef current_emotion;  
    EmotionTypeDef previous_emotion;
    uint8_t frame_count;
} OledCurrentStateTypedef;

/**
 * @brief  更新 OLED 动画(唯一驱动接口)
 */
void animation_update(OledTypedef *oled);

// ======== 表情动画(Emotion) 对外接口 ========
/**
 * @brief  供其他模块(如交互逻辑)调用的表情设置接口
 */
void set_pet_emotion(EmotionTypeDef new_emo);

/**
 * @brief  获取当前桌宠的目标表情
 */
EmotionTypeDef get_pet_emotion(void);

#endif /* ANIMATION_H_ */