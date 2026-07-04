#ifndef __INMP441_H
#define __INMP441_H
#include "stdint.h"
#include "cJSON.h"
void INMP441_Init();
cJSON * translate(int32_t *output_buffer);
extern cJSON *Extract_Left_Channel();



#endif 