#ifndef __RGB_2_HSV_H
#define __RGB_2_HSV_H

#include "stm32f10x.h"                  // Device header


void RGB_to_HSV(uint8_t red, uint8_t green, uint8_t blue, float *h, float *s, float *v);
void HSV_to_RGB(uint16_t h, float s, float v, uint8_t *r, uint8_t *g, uint8_t *b);

#endif
