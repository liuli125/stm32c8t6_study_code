#ifndef __Hard_I2C_H
#define __Hard_I2C_H

#include "stm32f10x.h"                  // Device header


void Myi2c_Hard_Init(void);
void Myi2c_Hard_start(void);
void Myi2c_Hard_stop(void);
void Myi2c_Hard_SendByte(uint8_t byte);
uint8_t Myi2c_Hard_ReadByte(void);
void Myi2c_Hard_SendAck(uint8_t ackbit);
uint8_t Myi2c_Hard_ReadAck(void);


#endif

