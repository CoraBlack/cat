//回调函数（状态机接串口） 根据当前情绪和接收到的收手势动作来切换情绪
//待解决问题：视觉和听觉输入优先级和冲突问题

#include "interaction.h"
#include "emotion.h"
#include "animation.h"

// 模块内部私有变量
static UART_HandleTypeDef *interact_uart = NULL;
static uint8_t rx_byte;          
static uint8_t rx_state = 0;     
static uint8_t current_cmd = 0;  
static GestureActionDef current_action = ACTION_NONE;

// 初始化通信
void Interaction_Init(UART_HandleTypeDef *huart)
{
    interact_uart = huart;
    if (interact_uart != NULL) {
        HAL_UART_Receive_IT(interact_uart, &rx_byte, 1);
    }
}

// 串口接收中断回调：状态机解析，只记录事件不处理逻辑
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == interact_uart && interact_uart != NULL)
    {
        switch (rx_state)
        {
            case 0:
                if (rx_byte == 0xAA) rx_state = 1;
                break;
            case 1:
                current_cmd = rx_byte;
                rx_state = 2;
                break;
            case 2:
                if (rx_byte == 0xFF) {
                    current_action = (GestureActionDef)current_cmd;
                }
                rx_state = 0;
                break;
            default:
                rx_state = 0;
                break;
        }
        HAL_UART_Receive_IT(interact_uart, &rx_byte, 1);
    }
}

// 表情切换逻辑
// interaction.c (省略了头文件和中断接收部分，只看核心更新函数)

void interaction_update(void) {
    // 如果没有接收到任何视觉新动作，直接退出
    if (current_action == GestureAction_None) {
        return;
    }

    PetState current_main_state = get_pet_main_state();

    // ==========================================
    // 最高优先级：摆手 (WAVE) 控制睡与醒
    // 逻辑：无视当前是什么模式，睡就醒，醒就睡
    // ==========================================
    if (current_action == GestureAction_Wave) {
        if (current_main_state == PetState_Asleep) {
            set_pet_main_state(PetState_Awake);
            set_pet_emotion(Emotion_Normal); // 睡醒默认回到普通清醒状态
        } else {
            set_pet_main_state(PetState_Asleep);
        }
        current_action = GestureAction_None;
        return; 
    }

    // ==========================================
    // 状态拦截墙：睡觉时，或听觉接管时，视觉动作无效
    // ==========================================
    if (current_main_state == PetState_Asleep || current_main_state == PetState_AudioMode) {
        // 直接清空视觉指令并退出，把表情控制权完全让给睡眠逻辑或听觉同学的代码
        current_action = GestureAction_None;
        return;
    }

    // ==========================================
    // 常规视觉交互逻辑 (仅在 PetState_Awake 状态下有效)
    // ==========================================
    switch (current_action) {
        case GestureAction_Punch:
            set_pet_emotion(Emotion_Angry);
            break;

        case GestureAction_Point:
            set_pet_emotion(Emotion_Curious);
            break;

        case GestureAction_Heart:
            set_pet_emotion(Emotion_Shy);
            break;

        default:
            break;
    }

    // 清空标志，等待下一次动作
    current_action = GestureAction_None;
}
