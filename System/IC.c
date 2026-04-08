#include "IC.h"                  // Device header


/*	Input Capture：输入捕获，用于测量外部脉冲周期
	TIM_ICInit 模式：测量外部PWM的频率（使用一个通道）
	TIM_PWMIConfig 模式：测量外部PWM的频率和占空比（使用两个通道）
	
	通过截取外部信号的一次周期的两个上升沿，用定时器计算间隔时间
	每个上升沿的时候把定时器的计数值 CNT 存到捕获/比较寄存器 CCR1 中，同时主从模式把 CNT 清零
	此时测量的频率(测频法) Freq = (72Mhz / PSC) / CCR1
	
	而下降沿把定时器的计数值 CNT 存到捕获/比较寄存器 CCR2 中,CNT不清零
	此时测量的 占空比  = CCR2 / CCR1
	
	本代码功能：PA0输出PWM波，被PA6输入捕获得到
*/
void IC_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);		//PA6 为 TIM3 的输入捕获口
	
	TIM_InternalClockConfig(TIM3);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 65536 - 1;		//设置最大，防止溢出
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;		//采样频率1Mhz
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
	
/*
	配置输入捕获PWM模式，同时测量PWM的频率和占空比，
	会为通道2设置通道1配置的相反的 下降沿，交叉输入 配置
*/
	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;	//选择通道1
	TIM_ICInitStructure.TIM_ICFilter = 0xF;		//滤波器设置，频率越低，采样数越大，效果最好
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	 //上升沿触发，把 CNT 的值转到 CCR1
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;		//不分频
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;	//直连输入
	
//	TIM_ICInit(TIM3, &TIM_ICInitStructure);		//输入捕获普通模式
	TIM_PWMIConfig(TIM3, &TIM_ICInitStructure);	//输入捕获PWM模式

	TIM_SelectInputTrigger(TIM3, TIM_TS_TI1FP1);		//选择 TI1FP1 作为从触发源，定时器通道 1（TI1）经过滤波和预分频后的信号（FP1）
	TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Reset);		//配置从模式触发效果为重置计数器
	
	TIM_Cmd(TIM3, ENABLE);
}

uint32_t IC_GetFreq(void)	//获取频率
{
	return 1000000 / (TIM_GetCapture1(TIM3) + 1);
}

uint32_t IC_GetDuty(void)	//获取占空比
{	// CCR2 (高电平时间)/ CCR1(总周期时间)
	return (TIM_GetCapture2(TIM3) + 1) * 100 / (TIM_GetCapture1(TIM3) + 1);
}
