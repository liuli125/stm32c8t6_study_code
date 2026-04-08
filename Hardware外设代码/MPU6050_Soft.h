#ifndef __MPU6050_SOFT_H
#define __MPU6050_SOFT_H

#include "stm32f10x.h"                  // Device header


void MPU6050_Soft_WriteReg(uint8_t RegAddress,uint8_t Data);
void MPU6050_Soft_Init(void);
uint8_t MPU6050_Soft_ReadReg(uint8_t RegAddress);
uint8_t MPU6050_GetID(void);
	
#endif

