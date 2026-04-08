#include "HARD_SPI.h"                  // Device header

/*	SPI（Serial Peripheral Interface）串行外设接口
	四根通信线：SCK（Serial Clock）、MOSI（Master Output Slave Input）、
	            MISO（Master Input Slave Output）、SS（Slave Select）
	输出引脚SCK，SS，MOSI 配置为推挽输出，输入引脚MISO配置为浮空或上拉输入
	STM32C8T6有APB2的SPI1(PA4-NSS, PA5-CLK, PA6-MISO, PA7-MOSI)，72mHz
			   APB1的SPI2(PB12-NSS, PB13-CLK, PB14-MISO, PB15-MOSI)，36mHz
	
	起始条件：SS从高电平切换到低电平
	终止条件：SS从低电平切换到高电平
	数据结构：MISO和MOSI交换一个字节
		
	本代码功能：硬件使用SPI协议，
*/

void MySPI_W_SS(uint8_t value)
{
	GPIO_WriteBit(GPIOA, GPIO_Pin_4, (BitAction)value);
}


void MySPI_Init(void)	//
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//通用推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//上拉输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	SPI_InitTypeDef SPI_InitStructure;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	//主机模式
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	//双线全双工
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//数据位数8位
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;		//数据高位先行
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;	//速率72mHz/128
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		//时钟极性，clock默认低电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	//时钟相位，第一个边沿输入数据(采样)
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//软件SS
	SPI_InitStructure.SPI_CRCPolynomial = 7;		//CRC校验(不使用)
	SPI_Init(SPI1, &SPI_InitStructure);
	
	SPI_Cmd(SPI1,ENABLE);
	MySPI_W_SS(1);	
}

//起始
void MySPI_start(void)	
{
	MySPI_W_SS(0);
}

//停止
void MySPI_stop(void)	
{
	MySPI_W_SS(1);
}

//MOIS和MISO交换一个字节(模式0)
uint8_t MySPI_SwapByte(uint8_t byte)
{
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE) != SET);
	SPI_I2S_SendData(SPI1, byte);
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE) != SET);
	return SPI_I2S_ReceiveData(SPI1);
}


/*	主程序代码
	
*/



