#include "sg90.h"                  // Device header


/*	驱动舵机：输出周期 20ms 的 PWM 波，
	其中高电平时间：0.5ms 为-90°，1ms 为-45°，1.5ms 为0°，2ms 为45°，2.5ms 为90°
	棕色线：Gnd，红色线：5V，黄色线：PWM控制口
	
	本代码功能：定时器设置自动重装值ARR 2000 用于计时 20ms，其中 CCR 为 50 对应高电平 0.5s
				250 对应高电平 2.5s
*/
void SG90_Init(void)  
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		//复用推挽，让内部外设控制端口(定时器TIM2)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;		//GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);		
	//PA0为 TIM2 的通道1输出引脚，同时也是 TIM 外部触发引脚
	
	TIM_InternalClockConfig(TIM2);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;		//向上计数
	TIM_TimeBaseInitStructure.TIM_Period = 2000 - 1;	//2000对应20ms，PWM频率50Hz
	TIM_TimeBaseInitStructure.TIM_Prescaler = 720 - 1;	//分频后100khZ
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	//向上计数:CNT<CCR时,REF置有效电平,CNT>CCR时,REF置无效电平。
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;	//不反转输出极性
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//启动使能
	TIM_OCInitStructure.TIM_Pulse = 0;		
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);
	
	TIM_Cmd(TIM2, ENABLE);
}

//舵机旋转角度 Angle ：0 ~ 180.
void SG90_SetAngle(float Angle)
{
	TIM_SetCompare1(TIM2, Angle / 180 * 200 + 50);
}


