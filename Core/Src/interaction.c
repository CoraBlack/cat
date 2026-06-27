//回调函数（状态机接串口） 根据当前情绪和接收到的收手势动作来切换情绪
//待解决问题：视觉和听觉输入优先级和冲突问题（个人建议视觉优先因为视觉理论上更明确也关键，且用户需要付出额外成本（接线），指令一般也更关键和少）

#include "interaction.h"

extern OledCurrentStateTypedef current_state;

// 模块内部私有变量
static UART_HandleTypeDef *interact_uart = NULL;
static uint8_t rx_byte;          
static uint8_t rx_state = 0;     
static uint8_t current_cmd = 0;  
static uint8_t target_emotion = 0; // 目标表情
static uint8_t current_emotion = 0; // 当前表情
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
void Interaction_Update(void)
{
    // 如果没有接收到新动作，直接退出
    if (current_action == ACTION_NONE) {
        return;
    }

    switch (current_action) 
    {
        // ==========================================
        // 动作：挥手 (WAVE)
        // 逻辑：醒的话睡，睡的话醒
        // ==========================================
        case ACTION_WAVE:
            if (current_state.current_emotion == Emotion_Sleep) {
                current_state.current_emotion = Emotion_Normal; // 睡醒变正常
            } else {
                current_state.current_emotion = Emotion_Sleep;  // 醒着去睡觉
            }
            current_state.frame_count = 0; 
            break;

        // ==========================================
        // 动作：出拳 (PUNCH)
        // 逻辑：单一表情 -> 切换为生气
        // ==========================================
        case ACTION_PUNCH:
            current_state.current_emotion = Emotion_Angry;
            current_state.frame_count = 0;
            break;

        // ==========================================
        // 动作：指着 (POINT)
        // 逻辑：单一表情 -> 切换为好奇
        // ==========================================
        case ACTION_POINT:
            current_state.current_emotion = Emotion_Curious;
            current_state.frame_count = 0;
            break;

        // ==========================================
        // 动作：比心 (HEART)
        // 逻辑：单一表情 -> 切换为害羞
        // ==========================================
        case ACTION_HEART:
            current_state.current_emotion = Emotion_Shy;
            current_state.frame_count = 0;
            break;

        // 其他未知动作不做处理
        default:
            break;
    }

    // 逻辑处理完毕，清空标志，等待下一次串口下发动作
    current_action = ACTION_NONE;
}