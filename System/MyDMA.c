#include "MYDMA.h"                  // Device header


/*	DMA（Direct Memory Access）直接存储器存取
	DMA可以提供外设和存储器或者存储器和存储器之间的高速数据传输，无须CPU干预，节省了CPU的资源
	12个独立可配置的通道： DMA1（7个通道）， DMA2（5个通道）

	本代码功能：把AddrA的数据传送到AddrB，传送4次
*/
uint16_t MyDMA_Size;

//AddrA：源数据地址，AddrB：目标数据地址，Size：转移数据数量
void MyDMA_Init(uint32_t AddrA, uint32_t AddrB, uint16_t Size)
{
	MyDMA_Size = Size;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	DMA_InitTypeDef DMA_InitStructure;
	//外设地址
	DMA_InitStructure.DMA_PeripheralBaseAddr = AddrA;	//传输源地址
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//数据宽度8位
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;		//地址指针自增
	//存储器地址
	DMA_InitStructure.DMA_MemoryBaseAddr = AddrB;		//传输目标地址
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;		//数据宽度8位
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;		//地址指针自增
	
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	//数据传输方向，选择由外设到存储器
	DMA_InitStructure.DMA_BufferSize = Size;			//指定传输计数器(向下计数)
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;		//指定传输计数器不自动重装
	DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;			//不用外设请求，直接连续触发DMA
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;	//优先级
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);		//DMA1的1通道
	
	DMA_Cmd(DMA1_Channel1, DISABLE);
}

//触发一次DMA传输
void MyDMA_Transfer(void)
{
	//必须暂停DMA才能重装计数器
	DMA_Cmd(DMA1_Channel1, DISABLE);
	DMA_SetCurrDataCounter(DMA1_Channel1, MyDMA_Size);	//设置DMA传输次数
	DMA_Cmd(DMA1_Channel1, ENABLE);
	
	while (DMA_GetFlagStatus(DMA1_FLAG_TC1) == RESET);
	DMA_ClearFlag(DMA1_FLAG_TC1);
}

/*	主程序代码
	uint8_t DataA[] = {0x01, 0x02, 0x03, 0x04};
	uint8_t DataB[] = {0, 0, 0, 0};
	MyDMA_Init((uint32_t)DataA, (uint32_t)DataB, 4);
	MyDMA_Transfer();
*/




