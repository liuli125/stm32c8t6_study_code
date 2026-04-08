#ifndef __LIGHT_SENSOR_H
#define __LIGHT_SENSOR_H

#include "stm32f10x.h"                  // Device header

void LightSensor_Init(void);
uint8_t LightSensor_Get(void);

#endif
