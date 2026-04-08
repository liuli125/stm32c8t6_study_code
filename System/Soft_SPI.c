//#include "Soft_SPI.h"                  // Device header

///*	SPI（Serial Peripheral Interface）串行外设接口
//	四根通信线：SCK（Serial Clock）、MOSI（Master Output Slave Input）、
//	            MISO（Master Input Slave Output）、SS（Slave Select）
//	输出引脚SCK，SS，MOSI 配置为推挽输出，输入引脚MISO配置为浮空或上拉输入
//	STM32C8T6有APB2的SPI1(PA4-NSS, PA5-CLK, PA6-MISO, PA7-MOSI)，72mHz
//			   APB1的SPI2(PB12-NSS, PB13-CLK, PB14-MISO, PB15-MOSI)，36mHz
//	
//	起始条件：SS从高电平切换到低电平
//	终止条件：SS从低电平切换到高电平
//	数据结构：MISO和MOSI交换一个字节
//		
//	本代码功能：程序手动翻转高低电平，软件使用SPI协议，
//*/

//void MySPI_W_SS(uint8_t value)
//{
//	GPIO_WriteBit(GPIOA, GPIO_Pin_4, (BitAction)value);
//}

//void MySPI_W_SCK(uint8_t value)
//{
//	GPIO_WriteBit(GPIOA, GPIO_Pin_5, (BitAction)value);
//}

//void MySPI_W_MOSI(uint8_t value)
//{
//	GPIO_WriteBit(GPIOA, GPIO_Pin_7, (BitAction)value);
//}

//uint8_t MySPI_R_MISO(void)
//{
//	return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6);
//}

//void MySPI_Init(void)	//
//{
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
//	
//	GPIO_InitTypeDef GPIO_InitStructure;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//通用推挽输出
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//	
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//上拉输入
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//	
//	MySPI_W_SS(1);
//	MySPI_W_SCK(0);
//}

////起始
//void MySPI_start(void)	
//{
//	MySPI_W_SS(0);
//}

////停止
//void MySPI_stop(void)	
//{
//	MySPI_W_SS(1);
//}

////MOIS和MISO交换一个字节(模式0)
//uint8_t MySPI_SwapByte(uint8_t byte)
//{
////	uint8_t ByteReceive = 0x00;
////	for(uint8_t i=0; i<8; i++)
////	{
////		MySPI_W_MOSI(byte & (0x80 >> i));	//移出数据
////		MySPI_W_SCK(1);						//第一个边沿移入（采样）数据
////		if(MySPI_R_MISO() == 1)				//移入数据
////			ByteReceive |= (0x80 >> i);
////		MySPI_W_SCK(0);						//第二个边沿移出（输出）数据
////	}
////	return ByteReceive;
//	for(uint8_t i=0; i<8; i++)
//	{
//		MySPI_W_MOSI(byte & 0x80);			//移出数据
//		byte <<= 1;
//		MySPI_W_SCK(1);						//第一个边沿移入（采样）数据
//		if(MySPI_R_MISO() == 1)				//移入数据
//			byte |= 0x01;
//		MySPI_W_SCK(0);						//第二个边沿移出（输出）数据
//	}
//	return byte;
//	//模式1 (模式2只需要把模式0的SCK调反，模式3只需要把模式1的SCK调反)
////		MySPI_W_SCK(1);						//第一个边沿移入（采样）数据
////		MySPI_W_MOSI(byte & 0x80);			//移出数据
////		byte <<= 1;
////		MySPI_W_SCK(0);		
////		if(MySPI_R_MISO() == 1)				//移入数据
////			byte |= 0x01;
//}


///*	主程序代码
//	
//*/



