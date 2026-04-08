#ifndef __RTC_H
#define __RTC_H

#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include <time.h>

extern uint16_t MyRTC_Time[];

void MyRTC_Init(void);
void MyRTC_SetTime(void);
void MyRTC_ReadTime(void);

#endif

