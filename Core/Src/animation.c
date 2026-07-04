#include "animation.h"  
#include "emotion.h"
#include "oled_frames.h"
#include <string.h>

// 建表
const uint8_t* normal_frames[]  = {oled_normal_0, oled_normal_1, oled_normal_2, oled_normal_3, oled_normal_4, oled_normal_5, oled_normal_6, oled_normal_7, oled_normal_8, oled_normal_9};
const uint8_t* happy_frames[]   = {oled_happy_0, oled_happy_1, oled_happy_2, oled_happy_3, oled_happy_4, oled_happy_5, oled_happy_6, oled_happy_7, oled_happy_8, oled_happy_9};
const uint8_t* alert_frames[]   = {oled_alert_0, oled_alert_1, oled_alert_2, oled_alert_3, oled_alert_4, oled_alert_5, oled_alert_6, oled_alert_7, oled_alert_8, oled_alert_9};
const uint8_t* sleep_frames[]   = {oled_sleep_0, oled_sleep_1, oled_sleep_2, oled_sleep_3, oled_sleep_4, oled_sleep_5, oled_sleep_6, oled_sleep_7, oled_sleep_8, oled_sleep_9};
const uint8_t* angry_frames[]   = {oled_angry_0, oled_angry_1, oled_angry_2, oled_angry_3, oled_angry_4, oled_angry_5, oled_angry_6, oled_angry_7, oled_angry_8, oled_angry_9};
const uint8_t* curious_frames[] = {oled_curious_0, oled_curious_1, oled_curious_2, oled_curious_3, oled_curious_4, oled_curious_5, oled_curious_6, oled_curious_7, oled_curious_8, oled_curious_9};
const uint8_t* shy_frames[]     = {oled_shy_0, oled_shy_1, oled_shy_2, oled_shy_3, oled_shy_4, oled_shy_5, oled_shy_6, oled_shy_7, oled_shy_8, oled_shy_9};

// 去掉了尾部多余的 100 参数
const AnimationTypedef emotion_animation_table[] = {
    [Emotion_Normal]  = {normal_frames,  10},
    [Emotion_Happy]   = {happy_frames,   10},
    [Emotion_Alert]   = {alert_frames,   10},
    [Emotion_Sleep]   = {sleep_frames,   10},
    [Emotion_Angry]   = {angry_frames,   10},
    [Emotion_Curious] = {curious_frames, 10},
    [Emotion_Shy]     = {shy_frames,     10}
};

static OledCurrentStateTypedef current_state = {
    .current_emotion = Emotion_Normal,
    .previous_emotion = Emotion_Normal,
    .frame_count = 0
};

// --- 新增的表情控制接口实现 ---
void set_pet_emotion(EmotionTypeDef new_emo) {
    current_state.current_emotion = new_emo;
}

EmotionTypeDef get_pet_emotion(void) {
    return current_state.current_emotion;
}

// --- 动画刷新逻辑 ---
void animation_update(OledTypedef *oled) {
    // 逻辑：确保同一状态循环播放，不同状态切换时先播放完上一状态的动画再切换到新状态
    if(current_state.current_emotion != current_state.previous_emotion && 
       current_state.frame_count >= emotion_animation_table[current_state.previous_emotion].frame_count) {
        current_state.frame_count = 0;
        current_state.previous_emotion = current_state.current_emotion;
    }

    const AnimationTypedef *animation = &emotion_animation_table[current_state.previous_emotion]; 
    const uint8_t *frame = animation->frame_list[current_state.frame_count % animation->frame_count];
    oled_show_frame(oled, frame);
    current_state.frame_count++;
}