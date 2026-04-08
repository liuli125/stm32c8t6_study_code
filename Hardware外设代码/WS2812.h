#ifndef __WS2812_H
#define __WS2812_H

#include "stm32f10x.h"                  // Device header

#define LED_Count 64


void PWM_DMA_WS2812_Init(void);
void WS2812_Update(void);
void WS2812_SetColor(uint8_t n, uint8_t r, uint8_t g, uint8_t b);
void Lights_out(void);

void WS2812_SetAllLight(uint8_t r, uint8_t g, uint8_t b);
void WS2812_Move(uint16_t num, uint16_t speed);
void WS2812_WaterFlow1(uint8_t Light_num);
void WS2812_WaterFlow2(uint8_t Light_num);

void WS2812_Cycle(uint8_t Light_num,uint8_t direct,uint16_t speed);
void Fill_Color(void);
void WS2812_AllLight(uint8_t speed);
void WS2812_Breath(uint8_t number);

#endif

