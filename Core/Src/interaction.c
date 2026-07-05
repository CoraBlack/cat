// 回调函数（状态机接串口） 根据当前情绪和接收到的手势动作来切换情绪

#include "interaction.h"
#include "emotion.h"
#include "animation.h"

// 模块内部私有变量
static UART_HandleTypeDef *interact_uart = NULL;

static volatile uint8_t rx_byte;          
static volatile uint8_t rx_state = 0;     

// 引入临时缓冲变量，避免帧错位或不完整导致运动和动作误触发
static uint8_t temp_cmd = 0;  
static int8_t  temp_x_offset = 0; 

static volatile GestureActionDef current_action = ACTION_NONE;
static volatile int8_t current_x_offset = 0; 

// 初始化通信
void Interaction_Init(UART_HandleTypeDef *huart)
{
    interact_uart = huart;
    if (interact_uart != NULL) {
        HAL_UART_Receive_IT(interact_uart, (uint8_t *)&rx_byte, 1);
    }
}

// 串口接收中断回调：4字节状态机解析，只记录事件不处理逻辑
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == interact_uart && interact_uart != NULL)
    {
        switch (rx_state)
        {
            case 0: // 匹配帧头 0xAA
                if (rx_byte == 0xAA) rx_state = 1;
                break;
                
            case 1: // 接收动作指令 (先存入临时变量)
                temp_cmd = rx_byte;
                rx_state = 2;
                break;
                
            case 2: // 接收 X轴偏移量 (强制转换为有符号整型 int8_t 存入临时变量)
                temp_x_offset = (int8_t)rx_byte;
                rx_state = 3;
                break;
                
            case 3: // 匹配帧尾 0xFF
                if (rx_byte == 0xFF) {
                    // 核心校验通过，同步数据给全局变量
                    current_action = (GestureActionDef)temp_cmd;
                    current_x_offset = temp_x_offset;
                }
                rx_state = 0; // 状态归零，准备接收下一帧
                break;
                
            default:
                rx_state = 0;
                break;
        }
        HAL_UART_Receive_IT(interact_uart, (uint8_t *)&rx_byte, 1);
    }
}

// 表情切换逻辑
void Interaction_Update(void) {
    // 快速读取并清除，防止在执行临界逻辑期间被突发的新中断覆盖数据导致丢失
    GestureActionDef action_to_process = current_action;
    current_action = ACTION_NONE;

    // 如果没有接收到任何视觉新动作，直接退出
    if (action_to_process == ACTION_NONE) {
        return;
    }

    PetState current_main_state = get_pet_main_state();

    // ==========================================
    // 最高优先级：摆手 (WAVE) 控制睡与醒
    // ==========================================
    if (action_to_process == ACTION_WAVE_HAND) {
        if (current_main_state == PetState_Asleep) {
            set_pet_main_state(PetState_Awake);
            set_pet_emotion(Emotion_Normal); // 睡醒默认回到普通清醒状态
        } else {
            set_pet_main_state(PetState_Asleep);
            set_pet_emotion(Emotion_Sleep);  
        }
        return; 
    }

    // ==========================================
    // 状态拦截墙：睡觉时，或听觉接管时，视觉动作无效
    // ==========================================
    if (current_main_state == PetState_Asleep || current_main_state == PetState_AudioMode) {
        // 直接退出，把表情控制权完全让给睡眠逻辑或听觉同学的代码
        return;
    }

    // ==========================================
    // 常规视觉交互逻辑 (仅在 PetState_Awake 状态下有效)
    // ==========================================
   
    switch (action_to_process) {
        case ACTION_FIST_PUNCH:
            set_pet_emotion(Emotion_Angry);
            break;

        case ACTION_POINT_FINGER:
            set_pet_emotion(Emotion_Curious);
            break;

        case ACTION_FINGER_HEART:
            set_pet_emotion(Emotion_Shy);
            break;

        default:
            break;
    }
}

/**
 * @brief  查询当前解析出的手势动作 (对外接口)
 */
GestureActionDef Interaction_GetCurrentAction(void) {
    return current_action;
}

/**
 * @brief  查询当前解析出的 X 轴偏移量 (对外接口)
 * @param  None
 * @retval int8_t 偏移量数值 (-100 到 100，左正右负)
 */
int8_t Interaction_GetXOffset(void) {
    return current_x_offset;
}