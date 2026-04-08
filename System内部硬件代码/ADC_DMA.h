#ifndef __ADC_DMA_H
#define __ADC_DMA_H

#include "stm32f10x.h"                  // Device header

extern uint16_t ADC_DMA_Value[4];

void ADC_DMA_Init(void);
void ADC_DMA_GetValue(void);
	
#endif
