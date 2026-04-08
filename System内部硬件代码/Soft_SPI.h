#ifndef __Soft_SPI_H
#define __Soft_SPI_H

#include "stm32f10x.h"                  // Device header


void MySPI_Init(void);
void MySPI_start(void);
void MySPI_stop(void);
uint8_t MySPI_SwapByte(uint8_t byte);


#endif

