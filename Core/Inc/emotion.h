#ifndef _EMOTION_H
#define _EMOTION_H

// emotion.h
typedef enum {
    PetState_Awake = 0,    // 视觉主导的清醒状态
    PetState_Asleep,       // 睡眠状态
    PetState_AudioMode,    // 新增：已接入听觉，听觉接管表情控制权
    PetState_Count
} PetState;

// 2. 定义微观情绪状态 (Sub State - 仅在 Awake 状态下有效)
typedef enum {
    Emotion_Normal = 0,
    Emotion_Happy,
    Emotion_Alert,
    Emotion_Angry,
    Emotion_Curious,
    Emotion_Shy,
    Emotion_Count 
} EmotionTypeDef

#endif 