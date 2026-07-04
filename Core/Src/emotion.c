#include "emotion.h"

// 定义全局主状态，开机默认是清醒的
static PetState current_main_state = PetState_Awake;

void set_pet_main_state(PetState new_state) {
    current_main_state = new_state;
}

PetState get_pet_main_state(void) {
    return current_main_state;
}