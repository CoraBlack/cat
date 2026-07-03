#include "oled.h"
#include <string.h>
#include "animation.h"  
#include "emotion.h"
#include "oled_frames.h"

// 帧数据表保持全小写蛇形命名
const uint8_t* normal_frames[]  = {oled_normal_0, oled_normal_1, oled_normal_2, oled_normal_3, oled_normal_4, oled_normal_5, oled_normal_6, oled_normal_7, oled_normal_8, oled_normal_9};
const uint8_t* happy_frames[]   = {oled_happy_0, oled_happy_1, oled_happy_2, oled_happy_3, oled_happy_4, oled_happy_5, oled_happy_6, oled_happy_7, oled_happy_8, oled_happy_9};
const uint8_t* alert_frames[]   = {oled_alert_0, oled_alert_1, oled_alert_2, oled_alert_3, oled_alert_4, oled_alert_5, oled_alert_6, oled_alert_7, oled_alert_8, oled_alert_9};
const uint8_t* sleep_frames[]   = {oled_sleep_0, oled_sleep_1, oled_sleep_2, oled_sleep_3, oled_sleep_4, oled_sleep_5, oled_sleep_6, oled_sleep_7, oled_sleep_8, oled_sleep_9};
const uint8_t* angry_frames[]   = {oled_angry_0, oled_angry_1, oled_angry_2, oled_angry_3, oled_angry_4, oled_angry_5, oled_angry_6, oled_angry_7, oled_angry_8, oled_angry_9};
const uint8_t* curious_frames[] = {oled_curious_0, oled_curious_1, oled_curious_2, oled_curious_3, oled_curious_4, oled_curious_5, oled_curious_6, oled_curious_7, oled_curious_8, oled_curious_9};
const uint8_t* shy_frames[]     = {oled_shy_0, oled_shy_1, oled_shy_2, oled_shy_3, oled_shy_4, oled_shy_5, oled_shy_6, oled_shy_7, oled_shy_8, oled_shy_9};

// 情绪动画映射表：利用 Emotion_Count 约束数组长度，防止越界
const Animation emotion_animation_table[Emotion_Count] = {
    [Emotion_Normal]  = {normal_frames,  10, 100},
    [Emotion_Happy]   = {happy_frames,   10, 100},
    [Emotion_Alert]   = {alert_frames,   10, 100},
    [Emotion_Angry]   = {angry_frames,   10, 100},
    [Emotion_Curious] = {curious_frames, 10, 100},
    [Emotion_Shy]     = {shy_frames,     10, 100}
};

// 独立睡眠动画定义
const Animation sleep_animation = {sleep_frames, 10, 100};

// 内部状态结构体：使用 PascalCase，去除了 Typedef 后缀，变量使用全小写蛇形
static OledCurrentState pet_state = {
    .main_state = PetState_Awake,
    .prev_main_state = PetState_Awake,
    .current_emotion = Emotion_Normal,
    .prev_emotion = Emotion_Normal,
    .frame_count = 0
};

/**
 * @brief 更新并绘制当前的动画帧
 * @note 保证当前动画播放完完整的一轮后，才允许切换到新状态对应的动画
 * @param oled OLED硬件控制结构体指针
 */
void animation_update_frame(Oled *oled) {
    // 1. 根据上一次的状态，动态获取当前正在播放的动画指针
    const Animation *playing_anim = NULL;
    if (pet_state.prev_main_state == PetState_Asleep) {
        playing_anim = &sleep_animation;
    } else {
        playing_anim = &emotion_animation_table[pet_state.prev_emotion];
    }

    // 2. 检测状态是否发生改变
    uint8_t is_state_changed = (pet_state.main_state != pet_state.prev_main_state) || 
                               (pet_state.current_emotion != pet_state.prev_emotion);

    // 3. 状态发生改变，且当前动画已完整播完一轮，执行动画切换逻辑
    if (is_state_changed && pet_state.frame_count >= playing_anim->frame_count) {
        pet_state.frame_count = 0;
        pet_state.prev_main_state = pet_state.main_state;
        pet_state.prev_emotion = pet_state.current_emotion;
        
        // 重新将播放指针切到新动画
        if (pet_state.prev_main_state == PetState_Asleep) {
            playing_anim = &sleep_animation;
        } else {
            playing_anim = &emotion_animation_table[pet_state.prev_emotion];
        }
    }

    // 4. 获取当前帧的数据指针并渲染
    const uint8_t *frame = playing_anim->frame_list[pet_state.frame_count % playing_anim->frame_count];
    oled_show_frame(oled, frame);
    
    pet_state.frame_count++;
}