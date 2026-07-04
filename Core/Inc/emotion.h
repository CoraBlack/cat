#ifndef EMOTION_H_
#define EMOTION_H_

#include <stdint.h>

// 1. 定义宏观主状态 (Main State)
typedef enum {
    PetState_Awake = 0,    // 视觉主导的清醒状态
    PetState_Asleep,       // 睡眠状态
    PetState_AudioMode,    // 已接入听觉，听觉接管表情控制权
    PetState_Count
} PetState;

// 2. 定义微观情绪状态 (Sub State)
typedef enum {
    Emotion_Normal = 0,
    Emotion_Happy,
    Emotion_Alert,
    Emotion_Sleep,         // 新增：补充缺失的睡眠表情，以对应动画表
    Emotion_Angry,
    Emotion_Curious,
    Emotion_Shy,
    Emotion_Count 
} EmotionTypeDef;


/**
 * @brief  设置桌宠主状态
 * @param  new_state 目标主状态 (如 PetState_Awake)
 */
void set_pet_main_state(PetState new_state);

/**
 * @brief  获取当前桌宠的主状态
 */
PetState get_pet_main_state(void);

#endif /* EMOTION_H_ */