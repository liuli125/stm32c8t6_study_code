#include "Encoder.h"                  // Device header


/*	Encoder Interface：编码器接口
	编码器接口可接收增量（正交）编码器的信号，根据编码器旋转产生的正交信号脉冲，
	自动控制CNT自增或自减，从而指示编码器的位置、旋转方向和旋转速度
	
	每个高级定时器和通用定时器都拥有1个编码器接口
	两个输入引脚借用了输入捕获的通道1和通道2

	本代码功能：利用编码器接口测速，用定时器捕获计量编码器旋转参数(转一次4个)
				然后定时1秒，此时获得的值为速度，即 V = N / T
*/
void Encoder_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	//TIM3 的两个输入捕获通道 PA6，PA7
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 65536 - 1;		//计数值ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler = 1 - 1;		//分频PSC
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
	
	//配置捕获
	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	TIM_ICInitStructure.TIM_ICFilter = 0xF;
	TIM_ICInit(TIM3, &TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
	TIM_ICInitStructure.TIM_ICFilter = 0xF;
	TIM_ICInit(TIM3, &TIM_ICInitStructure);
	
	//编码器接口配置：使用 TIM3 的通道1 2
	TIM_EncoderInterfaceConfig(
	TIM3, 					
	TIM_EncoderMode_TI12, 	//TI1 和 TI2 都计数
	TIM_ICPolarity_Rising, 	//上升延代表 通道1 高低电平极性不反向
	TIM_ICPolarity_Rising	//上升延代表 通道2 高低电平极性不反向
	);		
	
	TIM_Cmd(TIM3, ENABLE);
}

//得到旋转编码器计数
int16_t Encoder_Get(void)
{
	int16_t Temp;
	Temp = TIM_GetCounter(TIM3);	//取出定时器计数CNT
	TIM_SetCounter(TIM3, 0);	//定时器计数CNT设为0
	return Temp;
}
