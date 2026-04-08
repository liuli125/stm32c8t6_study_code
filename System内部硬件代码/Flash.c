#include "FLASH.h"                  // Device header

/*	
	
*/

#define STORE_START_ADDRESS		0x0800FC00	//页地址
#define STORE_COUNT				512			//数组长度

uint16_t Store_Data[STORE_COUNT];

/*
第一次使用闪存的时候，给第一个地址设置标志位A5A5，并且全部数据设0，再把闪存数据给到数组（第一个函数），接着就可以更改数组内容了,
数组更改后，就可以把数组的数据重新放到闪存里（第二个函数）
如果前面用过闪存，并且设置第一个地址的标志位A5A5，则第一个函数里面的判断语句不成功，然后就只给闪存的数据复制到数组里面
*/
//初始化闪存
void Store_Init(void)
{
	if (MyFLASH_ReadHalfWord(STORE_START_ADDRESS) != 0xA5A5)
	{
		MyFLASH_ErasePage(STORE_START_ADDRESS);
		MyFLASH_ProgramHalfWord(STORE_START_ADDRESS, 0xA5A5);
		for (uint16_t i = 1; i < STORE_COUNT; i ++)
		{
			MyFLASH_ProgramHalfWord(STORE_START_ADDRESS + i * 2, 0x0000);
		}
	}
	//把闪存数据送到数组
	for (uint16_t i = 0; i < STORE_COUNT; i ++)
	{
		Store_Data[i] = MyFLASH_ReadHalfWord(STORE_START_ADDRESS + i * 2);
	}
}
//把数组数据送到闪存
void Store_Save(void)
{
	MyFLASH_ErasePage(STORE_START_ADDRESS);
	for (uint16_t i = 0; i < STORE_COUNT; i ++)
	{
		MyFLASH_ProgramHalfWord(STORE_START_ADDRESS + i * 2, Store_Data[i]);
	}
}
//清除数组内容
void Store_Clear(void)
{
	for (uint16_t i = 1; i < STORE_COUNT; i ++)
	{
		Store_Data[i] = 0x0000;
	}
	Store_Save();
}

// OLED_ShowHexNum(1, 1, MyFLASH_ReadWord(0x08000000), 8);
uint32_t MyFLASH_ReadWord(uint32_t Address)		//读一个字的内容，32位，XXXX XXXX
{
	return *((__IO uint32_t *)(Address));
}

uint16_t MyFLASH_ReadHalfWord(uint32_t Address)	//读半字的内容，16位，XXXX
{
	return *((__IO uint16_t *)(Address));
}

uint8_t MyFLASH_ReadByte(uint32_t Address)		//读一个字节的内容，8位，XX
{
	return *((__IO uint8_t *)(Address));
}

//全数据擦除，程序会销毁，所有闪存数据变为 1
void MyFLASH_EraseAllPages(void)
{
	FLASH_Unlock();
	FLASH_EraseAllPages();
	FLASH_Lock();
}
//页擦除，擦除大小 1k，0x0000 0400
void MyFLASH_ErasePage(uint32_t PageAddress)
{
	FLASH_Unlock();
	FLASH_ErasePage(PageAddress);
	FLASH_Lock();
}
//先擦除数据，再编程写入数据，MyFLASH_ErasePage(0x08000FC00);
//页编程，写入32位数据，MyFLASH_ProgramWord(0x08000FC00, 0x12345678);
void MyFLASH_ProgramWord(uint32_t Address, uint32_t Data)
{
	FLASH_Unlock();
	FLASH_ProgramWord(Address, Data);
	FLASH_Lock();
}
//页编程，写入16位数据，MyFLASH_ProgramWord(0x08000FC10, 0x1234);
void MyFLASH_ProgramHalfWord(uint32_t Address, uint16_t Data)
{
	FLASH_Unlock();
	FLASH_ProgramHalfWord(Address, Data);
	FLASH_Lock();
}



/* 测试程序代码
	OLED_Init();
	Key_Init();
	Store_Init();
	
	OLED_ShowString(1, 1, "Flag:");
	OLED_ShowString(2, 1, "Data:");
	
	while (1)
	{
		KeyNum = Key_GetNum();
		
		if (KeyNum == 1)
		{
			Store_Data[1] ++;
			Store_Data[2] += 2;
			Store_Data[3] += 3;
			Store_Data[4] += 4;
			Store_Save();
		}
		
		if (KeyNum == 2)
		{
			Store_Clear();
		}
		
		OLED_ShowHexNum(1, 6, Store_Data[0], 4);
		OLED_ShowHexNum(3, 1, Store_Data[1], 4);
		OLED_ShowHexNum(3, 6, Store_Data[2], 4);
		OLED_ShowHexNum(4, 1, Store_Data[3], 4);
		OLED_ShowHexNum(4, 6, Store_Data[4], 4);
	}

*/

/*
	OLED_Init();
	
	OLED_ShowString(1, 1, "F_SIZE:");	//读取闪存大小，单位Kb
	OLED_ShowHexNum(1, 8, *((__IO uint16_t *)(0x1FFFF7E0)), 4);
	
	OLED_ShowString(2, 1, "U_ID:");		//读取设备id
	OLED_ShowHexNum(2, 6, *((__IO uint16_t *)(0x1FFFF7E8)), 4);
	OLED_ShowHexNum(2, 11, *((__IO uint16_t *)(0x1FFFF7E8 + 0x02)), 4);
	OLED_ShowHexNum(3, 1, *((__IO uint32_t *)(0x1FFFF7E8 + 0x04)), 8);
	OLED_ShowHexNum(4, 1, *((__IO uint32_t *)(0x1FFFF7E8 + 0x08)), 8);
*/
	
	
	
	
