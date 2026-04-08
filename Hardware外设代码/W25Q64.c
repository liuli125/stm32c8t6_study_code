#include "W25Q64.h"
//#include "Soft_SPI.h"
#include "HARD_SPI.h"

/*	W25Q64：8M 容量非易失性存储器
	页(page)地址为256字节，扇区(sector)大小4kb，块(block)大小64kb
	xx yz 00，xx选择块，y选择扇区，z选择哪一个页，00就是一页
	每个数据位只能由1改写为0，不能由0改写为1
	写入数据前必须先擦除，擦除后，所有数据位变为1
	擦除操作的最小区域为扇区4kb
	读出的数据=原始数据&写入的数据
	
	
	本代码使用软件I2C驱动W25Q64
*/
void W25Q64_Init(void)
{
	MySPI_Init();
}

//获取ID号
//uint8_t MID; uint16_t DID;
void W25Q64_GetID(uint8_t *MID, uint16_t *DID)
{
	MySPI_start();
	MySPI_SwapByte(W25Q64_JEDEC_ID);
	*MID = MySPI_SwapByte(W25Q64_DUMMY_BYTE);
	*DID = MySPI_SwapByte(W25Q64_DUMMY_BYTE);
	*DID <<= 8;
	*DID |= MySPI_SwapByte(W25Q64_DUMMY_BYTE);
	MySPI_stop();
}

//写使能
void W25Q64_WriteEnable(void)
{
	MySPI_start();
	MySPI_SwapByte(W25Q64_WRITE_ENABLE);
	MySPI_stop();
}

//等待芯片处于忙状态
void W25Q64_WaitBusy(void)
{
	uint32_t Timeout = 100000;
	MySPI_start();
	MySPI_SwapByte(W25Q64_READ_STATUS_REGISTER_1);
	while((MySPI_SwapByte(W25Q64_DUMMY_BYTE) & 0x01) == 1)
	{
		Timeout--;
		if(Timeout == 0) break;
	}
	MySPI_stop();
}

//向指定地址写入数据，最大256个(一页)，超过会从0覆写
void W25Q64_PageProgram(uint32_t Address, uint8_t *Data, uint16_t count)
{
	W25Q64_WaitBusy();
	W25Q64_WriteEnable();
	MySPI_start();
	MySPI_SwapByte(W25Q64_PAGE_PROGRAM);
	MySPI_SwapByte(Address >> 16);
	MySPI_SwapByte(Address >> 8);
	MySPI_SwapByte(Address);
	for(uint16_t i=0; i<count; i++)
		MySPI_SwapByte(Data[i]);
	MySPI_stop();
}

//擦除指定地址所在扇区的数据，一个扇区4kb，xxx000-xxxFFF
void W25Q64_SectorErase(uint32_t Address)
{
	W25Q64_WaitBusy();
	W25Q64_WriteEnable();
	MySPI_start();
	MySPI_SwapByte(W25Q64_SECTOR_ERASE_4KB);
	MySPI_SwapByte(Address >> 16);
	MySPI_SwapByte(Address >> 8);
	MySPI_SwapByte(Address);
	MySPI_stop();
}

//向指定地址读取数据，不限制读取数量
void W25Q64_ReadData(uint32_t Address, uint8_t *Data, uint32_t count)
{
	W25Q64_WaitBusy();
	MySPI_start();
	MySPI_SwapByte(W25Q64_READ_DATA);
	MySPI_SwapByte(Address >> 16);
	MySPI_SwapByte(Address >> 8);
	MySPI_SwapByte(Address);
	for(uint32_t i=0; i<count; i++)
		Data[i] = MySPI_SwapByte(W25Q64_DUMMY_BYTE);
	MySPI_stop();
}


/*	主程序代码
	OLED_Init();
	W25Q64_Init();
	
	OLED_ShowString(1,1,"MID:   DID:");
	OLED_ShowString(2,1,"W:");
	OLED_ShowString(3,1,"R:");

	uint8_t ArrayWrate[]={0xA1,0xB2,0xC3,0x04};	//4个数据占4个地址
	uint8_t ArrayRead[4];
	uint8_t MID; 
	uint16_t DID;
	W25Q64_GetID(&MID, &DID);
	OLED_ShowHexNum(1,5,MID,2);
	OLED_ShowHexNum(1,12,DID,4);
	
	//验证掉电不丢失要注释下面两行代码
	W25Q64_SectorErase(0x000000);
	W25Q64_PageProgram(0x000000, ArrayWrate, 4);

	W25Q64_ReadData(0x000001, ArrayRead, 4);
	OLED_ShowHexNum(2,3,ArrayWrate[0],2);
	OLED_ShowHexNum(2,6,ArrayWrate[1],2);
	OLED_ShowHexNum(2,9,ArrayWrate[2],2);
	OLED_ShowHexNum(2,12,ArrayWrate[3],2);

	OLED_ShowHexNum(3,3,ArrayRead[0],2);
	OLED_ShowHexNum(3,6,ArrayRead[1],2);
	OLED_ShowHexNum(3,9,ArrayRead[2],2);
	OLED_ShowHexNum(3,12,ArrayRead[3],2);
	
	输出结果为
	MID：EF   DID：4017
	W：A1 B2 C3 04
	R：B2 C3 04 FF
*/




