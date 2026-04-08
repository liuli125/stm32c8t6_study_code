#ifndef __W25Q64_HARD_H
#define __W25Q64_HARD_H

#include "stm32f10x.h"                  // Device header
#include "W25Q64_INS.h"                  // Device header


void W25Q64_Init(void);
void W25Q64_GetID(uint8_t *MID, uint16_t *DID);
void W25Q64_PageProgram(uint32_t Address, uint8_t *Data, uint16_t count);
void W25Q64_SectorErase(uint32_t Address);
void W25Q64_ReadData(uint32_t Address, uint8_t *Data, uint32_t count);

#endif

