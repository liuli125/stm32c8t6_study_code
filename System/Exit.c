#include "stm32f10x.h"                  // Device header

uint16_t CountSensor_Count;


/*	External Interrupt：外部中断
	外部中断信号从GPIO->AFIO->EXTI->NVIC->CPU
*/
void Exit_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	//AFIO主要有2个功能：1.重映射配置，2.打开引脚对应的外部中断信号使其能够进入到 EXIT 模块
	
	GPIO_InitTypeDef GPIO_InitStructure;	//选择外部中断引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource14);
	//选择GPIOB 14引脚的中断线路
	
	//配置外部中断
	EXTI_InitTypeDef EXTI_InitStructure;	
	EXTI_InitStructure.EXTI_Line = EXTI_Line14;		//14线路也就是PB14的
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;		//启动中断线路
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		//中断模式
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;	//下降沿触发
	EXTI_Init(&EXTI_InitStructure);
	
	//EXTI和NVIC默认开启
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		//中断分组2
	
	//配置中断优先级
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;	//EXTI10-15通道都在这里，STM32F10X_MD
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//响应优先级
	NVIC_Init(&NVIC_InitStructure);
}

uint16_t CountSensor_Get(void)
{
	return CountSensor_Count;
}

//中断代码，startup 文件里86行
/*
void EXTI15_10_IRQHandler(void)		//外部中断函数，10 ~ 15线路整合到一起
{
	if (EXTI_GetITStatus(EXTI_Line14) == SET)	//如果是外部中断的14引脚
	{
		//如果出现数据乱跳的现象，可再次判断引脚电平，以避免抖动
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0)
		{
			CountSensor_Count ++;
		}
		EXTI_ClearITPendingBit(EXTI_Line14);
	}
}

*/

