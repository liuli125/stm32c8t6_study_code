#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>

/*	USART 通用同步/异步收发器
	串口发送数据：PA9接RXD，PA10接TXD (原PA9是TXD，PA10是RXD)
	
	本代码功能：通过USART装载数据到移位寄存器，然后发送到串口，蓝牙也可以用
*/

uint8_t Serial_RxData;
uint8_t Serial_RxFlag;
uint8_t Serial_RxPacket[4];
uint8_t Serial_TxPacket[4];

void Serial_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//打开USART
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//打开GPIOA
	
	//定义USART输出引脚，PA9是TXD，PA10是RXD
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽，让内部器件控制PA9(TXD)输出串口数据
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;		//波特率
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //不选择流控制
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;		//开启接收和发送
	USART_InitStructure.USART_Parity = USART_Parity_No;  //没有校验位
	USART_InitStructure.USART_StopBits = USART_StopBits_1; //停止位1位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;  //8位数据长度(有校验就9位)
	USART_Init(USART1, &USART_InitStructure);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	//开启接收中断
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART1, ENABLE);
}
//发送一个字节数据
void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART1, Byte);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	//当发送数据寄存器空时(数据转移完了)，结束循环
}
//发送数组
void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(Array[i]);
	}
}
//发送字符串
void Serial_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)
	{
		Serial_SendByte(String[i]);
	}
}

uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y --)
	{
		Result *= X;
	}
	return Result;
}
//发送数字
void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');
	}
}
//重定义prinf，需要设置打开Use MicroLIB
int fputc(int ch, FILE *f)
{
	Serial_SendByte(ch);
	return ch;
}

void Serial_Printf(char *format, ...)
{
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	Serial_SendString(String);
}
//获取RX接收数据状态
uint8_t Serial_GetRxFlag(void)
{
	if (Serial_RxFlag == 1)
	{
		Serial_RxFlag = 0;
		return 1;
	}
	return 0;
}
//获取接收数据
uint8_t Serial_GetRxData(void)
{
	return Serial_RxData;
}

//发送数据包
void Serial_SendPacket(void)
{
	Serial_SendByte(0xFF);	//包头
	Serial_SendArray(Serial_TxPacket,4);
	Serial_SendByte(0xFE);	//包尾
}

//中断代码
void USART1_IRQHandler(void)
{
	static uint8_t RxState = 0;		//状态
	static uint8_t pRxPacket = 0;	//接收数据的数组指针
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)	//rx不为空时(接收到数据)，触发中断
	{
		uint8_t RxData = USART_ReceiveData(USART1);		//把USART1接收的数据送到RxData
		if(RxState == 0)	//状态0：检测包头0xFF
		{
			if(RxData == 0xFF) //检测包头
			{
				RxState = 1;
				pRxPacket = 0;
			}
		}
		else if(RxState == 1)	//状态1：接收真正的数据
		{
			Serial_RxPacket[pRxPacket] = RxData;
			pRxPacket ++;
			if(pRxPacket >= 4)
			{
				RxState = 2;
			}
		}
		else if(RxState == 2)	//状态0：检测包头0xFE
		{
			if(RxData == 0xFE) //检测包尾
			{
				RxState = 0;
				Serial_RxFlag = 1;
			}
		}
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);	//清除接收RX标志位
	}
}
/*	数据包程序	#include "string.h"
	//发送
	Serial_TxPacket[0] = 0x01;
	Serial_TxPacket[1] = 0x02;
	Serial_TxPacket[2] = 0x03;
	Serial_TxPacket[3] = 0x04;
	Serial_SendPacket();	//发送数据包
	OLED_ShowHexNum(4, 1, Serial_RxPacket[0], 2);
	OLED_ShowHexNum(4, 4, Serial_RxPacket[1], 2);
	OLED_ShowHexNum(4, 7, Serial_RxPacket[2], 2);
	OLED_ShowHexNum(4, 10, Serial_RxPacket[3], 2);
	printf("%d",x);

	//接收
	if (Serial_RxFlag == 1)
	{
		OLED_ShowString(4, 1, "                ");
		OLED_ShowString(4, 1, Serial_RxPacket);		//显示接收的数据
		
		if (strcmp(Serial_RxPacket, "LED_ON") == 0)		//相等返回0
		{
			LED1_ON();
			Serial_SendString("LED_ON_OK\r\n");		//返回数据
			OLED_ShowString(2, 1, "                ");
			OLED_ShowString(2, 1, "LED_ON_OK");
		}
		else if (strcmp(Serial_RxPacket, "LED_OFF") == 0)
		{
			LED1_OFF();
			Serial_SendString("LED_OFF_OK\r\n");
			OLED_ShowString(2, 1, "                ");
			OLED_ShowString(2, 1, "LED_OFF_OK");
		}
		else
		{
			Serial_SendString("ERROR_COMMAND\r\n");
			OLED_ShowString(2, 1, "                ");
			OLED_ShowString(2, 1, "ERROR_COMMAND");
		}
		Serial_RxFlag = 0;
*/


/*	单数据程序
	中断代码
void USART1_IRQHandler(void)
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)	//rx不为空时，接收到数据，触发中断
	{
		Serial_RxData = USART_ReceiveData(USART1);			//把USART1接收的数据送到RxData
		Serial_RxFlag = 1;									//rx接收标志位置11
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);		//清除接收RX标志位
	}
}

	主程序应用代码(接收)
	uint16_t RxData;
	if (Serial_GetRxFlag() == 1)	//当接收到数据时
	{
		RxData = Serial_GetRxData();
		Serial_SendByte(RxData);
		OLED_ShowHexNum(1, 8, RxData, 2);
	}
*/

