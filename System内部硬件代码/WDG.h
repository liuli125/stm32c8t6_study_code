#ifndef __WDG_H
#define __WDG_H

#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Key.h"

void IWDG_Init(void);
void WWDG_Init(void);
	

#endif

