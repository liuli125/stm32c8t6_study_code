#include "timer.h"                  // Device header


/*	定时器 TIM 中断
	STM32C8T6有TIM1，TIM2，TIM3，TIM4
	高级定时器为APB2的TIM1，TIM8，多了重复计数器，死区生成，互补输出，刹车输入
	通用定时器为APB1的TIM2，TIM3，TIM4，TIM5，多了内外时钟源选择，输入捕获，输出比较，编码器接口，主从触发模式
	基本定时器为APB1的TIM6，TIM7，可以定时中断，主模式触发DAC
	
	总定时时间 T = (ARR + 1) * (PSC + 1) / 72M
	
	ARR：自动重装寄存器，PSC：预分频器
	c8t6时钟频率72MHz，定时器引脚位置 手册8.3.7
	
	
	本代码功能：定时 1s
*/
void Timer_Init(void)	
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//打开定时器TIM2
	TIM_InternalClockConfig(TIM2);	//定时器使用内部时钟，默认也是
	
	//定时器参数配置
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//滤波器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	 //上升延计数
	TIM_TimeBaseInitStructure.TIM_Period = 10000 - 1;		//ARR：计数10000次，也就是1s
	TIM_TimeBaseInitStructure.TIM_Prescaler = 7200 - 1;		//PSC：定时器分频7200(也可以认为计多少数)
	//定时 T = 1/(72Mhz/7200) = 1/(72 000 000 / 7200) = 1 / 10000 = 0.0001s
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;	//重复计数器，高级定时器才有
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	
	TIM_SetCounter(TIM2,0);	//计数器清零
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);	//清除定时器更新中断标志位，更新事件(满ARR计数值)发生时，TIM_FLAG_Update置1
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);	//允许TIM_FLAG_Update置1时，定时器更新中断
	//ITConfig配置函数选择打开某一种中断触发
	
	//配置中断优先级
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;		//定时器TIM2中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//中断开启
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	
	TIM_Cmd(TIM2, ENABLE);	//开启定时器
}

void TimerETR_Init(void)	//定时器外部计数
{
	Timer_Init();	//定时器初始化
	
	//选择外部触发引脚PA0
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//默认上拉输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//PA0为 TIM2 的通道1输出引脚，同时也是 TIM 外部触发引脚
	
	TIM_ETRClockMode2Config(TIM2, TIM_ExtTRGPSC_OFF, TIM_ExtTRGPolarity_NonInverted, 0x0F);	//模式2
	//外部时钟模式1使用外部输入脚(TIx)作为计数器时钟的时钟源，
	//外部时钟模式2使用外部触发输入(ETR)或内部触发输入(ITRx)作为计数器时钟的时钟源1。
	
	//定时器参数配置
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 10 - 1;		//计数10次
	TIM_TimeBaseInitStructure.TIM_Prescaler = 1 - 1;	//不分频，72M
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	
}

//返回定时器计数值ARR
uint16_t Timer_GetCounter(void)
{
	return TIM_GetCounter(TIM2);
}

/*	定时器中断代码  IRQ : Interrupt Request
	TIM_GenerateEvent(TIM,TIM_EventSource_Update);  //软件产生更新中断
void TIM2_IRQHandler(void)	
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);	
	}
}
*/


