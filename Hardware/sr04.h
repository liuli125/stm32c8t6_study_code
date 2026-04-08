#include "stm32f10x.h"                  // Device header
#include "Delay.h"

void SR04_Init(void);
void TIM3_Init_SR04(void);
void Trig_Send_SR04(void);
void EXTI_Init_SR04(void);

float Get_Length_SR04(void);

void Array_fill_SR04(float *array);
float Get_Length_Filter(float *array,uint8_t length);

