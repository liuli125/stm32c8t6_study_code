#include "Pwm.h"                  // Device header



/*	Output Compare：输出比较
	输出比较可以通过比较 CNT 与 CCR 寄存器值的关系，来对输出电平进行置1、置0或翻转的操作，
	用于输出一定频率和占空比的PWM波形
	
	PWM输出：使用定时器计数，设置4个通道的 OC 参数，输出指定 PWM 波
	PA0使用定时器2的通道1输出PWM波
	//PWM 占空比 = CCR / (ARR + 1)
	//PWM 频率	 = 72M / (ARR + 1) / (PSC + 1)
	//PWM 分辨率 = 1 / (ARR + 1)
	
	本代码功能：定时器2通道1的PA0输出频率为 1KHz(1ms),占空比为 50% 的PWM波形
*/
void PWM_Init(void)  
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	//AFIO 用于重映射TIM2的通道1的PA0变成PA15
//	GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2, ENABLE);	//TIM2 部分重映射模式(手册8.3.7 )
//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); 
	//重映射 PA15 前需要关闭这个引脚的调试端口，变成普通GPIO口(引脚默认为调试端口才需要)
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		//复用推挽，让内部外设控制端口输出(定时器TIM2)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;		//或者GPIOA_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);		//PA0 为 TIM2 的通道1输出引脚，同时也是TIM外部触发引脚
	
	TIM_InternalClockConfig(TIM2);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//滤波器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;		//向上计数
	TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;			//ARR：计数100次
	TIM_TimeBaseInitStructure.TIM_Prescaler = 720 - 1;		//PSC：分频720
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	
//	TIM_ARRPreloadConfig(TIM1, ENABLE);	//启用预加载寄存器，改变ARR不会立即生效，要等到更新事件才生效
	
	// Output Compare：输出比较
	TIM_OCInitTypeDef TIM_OCInitStructure;	//定义结构体
	TIM_OCStructInit(&TIM_OCInitStructure);	//结构体默认赋值
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	//PWM模式1：向上计数: CNT<CCR 时,REF置有效电平,CNT>CCR 时,REF置无效电平。向下计数: CNT>CCR 时,REF置无效电平,CNT<CCR时,REF置有效电平
	//PWM模式2：向上计数: CNT<CCR 时,REF置无效电平,CNT>CCR 时,REF置有效电平
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;	//不反转输出极性
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//启动使能
	TIM_OCInitStructure.TIM_Pulse = 50;		//CCR，比较值，用于设置占空比，此时占空比为 50/100 = 50%
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);	//TIM2通道1，对应PA0
//	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);   //打开CCR的影子寄存器：改变CCR不会立即生效，要等到更新事件才生效

	TIM_Cmd(TIM2, ENABLE);	//打开定时器
}

//设置比较值CCR，也就是调整占空比，占空比 = CCR / ARR + 1
void PWM_SetCompare1(uint16_t Compare)
{
	TIM_SetCompare1(TIM2, Compare);
}

//设置分频值，调整频率
void PWM_SetPrescaler(uint16_t Prescaler)
{
	TIM_PrescalerConfig(TIM2, Prescaler, TIM_PSCReloadMode_Immediate);
}



