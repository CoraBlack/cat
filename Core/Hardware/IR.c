#include "IR.h"
#include "main.h"
#include "Motor.h"
#include "stm32f411xe.h"
#include "stm32f4xx_hal_gpio.h"
#include <stdint.h>

volatile uint8_t ir_flags = 0;

#define FLAG_LEFT_FRONT   (1U << 0)
#define FLAG_RIGHT_FRONT  (1U << 1)
#define FLAG_RIGHT_REAR   (1U << 2)  
#define FLAG_LEFT_REAR    (1U << 3)

// 统一避障状态机
typedef enum {
    AVOID_IDLE = 0,       // 空闲，等待触发
    AVOID_PHASE_1,        // 避障第一阶段
    AVOID_PHASE_2         // 避障第二阶段
} Avoid_State_t;

// 当前激活的避障动作类型
typedef enum {
    ACT_NONE = 0,
    ACT_LEFT_FRONT,
    ACT_RIGHT_FRONT,
    ACT_LEFT_REAR,
    ACT_RIGHT_REAR
} Avoid_Action_t;

/*======================== 2. 状态机私有变量 ========================*/
static Avoid_State_t  avoid_state = AVOID_IDLE;
static Avoid_Action_t current_action = ACT_NONE;
static uint32_t       phase_start_tick = 0;

#define PHASE_DURATION_MS  3000U  // 每个阶段持续 3秒 (对应你原来的 MOTOR_TURN_TIME)

/*======================== 3. 中断回调 (极简、安全) ========================*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    // 仅置位标志，绝不在中断里做任何耗时操作
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);
    switch (GPIO_Pin) {
        case LeftFront_Pin:  ir_flags |= FLAG_LEFT_FRONT;  break;
        case RightFront_Pin: ir_flags |= FLAG_RIGHT_FRONT; break;
        case LeftReap_Pin:   ir_flags |= FLAG_LEFT_REAR;   break; 
        case RightReap_Pin:  ir_flags |= FLAG_RIGHT_REAR;  break;
        default: break;
    }
}

/*======================== 4. 核心状态机引擎 ========================*/
/**
 * @brief 替代原有的 IR_While()，必须在 main.c 的 while(1) 中高频调用
 */
void IR_Avoidance_Task(void) {
    uint32_t now = HAL_GetTick();
    uint32_t elapsed = now - phase_start_tick;

    switch (avoid_state) {
        /*---------- 空闲态：检测新触发 ----------*/
        case AVOID_IDLE: {
            // 优先级：左前 > 右前 > 左后 > 右后 (可根据实际需求调整)
            if      (ir_flags & FLAG_LEFT_FRONT)  { current_action = ACT_LEFT_FRONT;  ir_flags &= ~FLAG_LEFT_FRONT;  }
            else if (ir_flags & FLAG_RIGHT_FRONT) { current_action = ACT_RIGHT_FRONT; ir_flags &= ~FLAG_RIGHT_FRONT; }
            else if (ir_flags & FLAG_LEFT_REAR)   { current_action = ACT_LEFT_REAR;   ir_flags &= ~FLAG_LEFT_REAR;   }
            else if (ir_flags & FLAG_RIGHT_REAR)  { current_action = ACT_RIGHT_REAR;  ir_flags &= ~FLAG_RIGHT_REAR;  }
            
            if (current_action != ACT_NONE) {
                avoid_state = AVOID_PHASE_1;
                phase_start_tick = now;
                
                // ⭐ 进入第一阶段立即执行初始动作
                switch (current_action) {
                    case ACT_LEFT_FRONT:  Motor_Back(); Motor_Right(); break; 
                    case ACT_RIGHT_FRONT: Motor_Left();                break; 
                    case ACT_LEFT_REAR:   Motor_Left();                break; 
                    case ACT_RIGHT_REAR:  Motor_Back(); Motor_Left();  break; 
                    default: break;
                }
            }
            break;
        }

        /*---------- 第一阶段：等待超时进入第二阶段 ----------*/
        case AVOID_PHASE_1: {
            if (elapsed >= PHASE_DURATION_MS) {
                avoid_state = AVOID_PHASE_2;
                phase_start_tick = now; // ⭐ 仅在状态切换时重置计时基准
                
                // ⭐ 执行第二阶段动作
                switch (current_action) {
                    case ACT_LEFT_FRONT:  Motor_Back(); Motor_Left();  break; 
                    case ACT_RIGHT_FRONT: Motor_Right();               break; 
                    case ACT_LEFT_REAR:   Motor_Right();               break; 
                    case ACT_RIGHT_REAR:  Motor_Back(); Motor_Right(); break; 
                    default: break;
                }
            }
            break;
        }

        /*---------- 第二阶段：等待超时恢复 ----------*/
        case AVOID_PHASE_2: {
            if (elapsed >= PHASE_DURATION_MS) {
                Motor_Forward();          // 避障完成，恢复前进
                current_action = ACT_NONE;
                avoid_state = AVOID_IDLE; // ⭐ 回到空闲态，准备接收下一次触发
            }
            break;
        }

        default:
            avoid_state = AVOID_IDLE;
            break;
    }
}