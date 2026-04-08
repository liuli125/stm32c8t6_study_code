#ifndef __Soft_I2C_H
#define __Soft_I2C_H

#include "stm32f10x.h"                  // Device header


void Myi2c_Init(void);
void Myi2c_start(void);
void Myi2c_stop(void);
void Myi2c_SendByte(uint8_t byte);
uint8_t Myi2c_ReadByte(void);
void Myi2c_SendAck(uint8_t ackbit);
uint8_t Myi2c_ReadAck(void);


#endif

