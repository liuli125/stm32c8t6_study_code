#include "sr04.h"


/*
	SR-04 超声波测距
	TRIG 引脚需要10us高电平时间激活，接PA0
	ECHO 引脚高电平时间代表测出来的距离，接PA1
	
	本代码使用外部中断测量ECHO引脚电平情况，用TIM3计时高电平时间

*/


volatile uint16_t count;		//时间计数
volatile uint8_t	Flag_edge;	//判断边缘
float distance;

void SR04_Init(void)	//引脚初始化
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    //TRIG触发信号引脚	PA0
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;	//上拉高电平
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    GPIO_ResetBits(GPIOA, GPIO_Pin_0); // 初始拉低
	//ECHO输出距离数据引脚	PA1
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//浮空输入
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	TIM3_Init_SR04();	//定时器初始化
	EXTI_Init_SR04();	//外部中断初始化
}

void Trig_Send_SR04(void)	//TRIG触发10us高电平信号
{
	GPIO_SetBits(GPIOA,GPIO_Pin_0);
	Delay_us(15);                   
	GPIO_ResetBits(GPIOA,GPIO_Pin_0);
}

void TIM3_Init_SR04(void)	//定时器TIM3 初始化
{	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	TIM_InternalClockConfig(TIM3);
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//滤波器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	 //上升延计数
	TIM_TimeBaseInitStructure.TIM_Period = 0xFFFF;		//ARR：计65535数次，也就是65ms
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;		//PSC：定时器分频72
	//定时 T = 1/(72Mhz/72) = 1/(72 000 000 / 72) = 1 / 10000 = 0.000001s
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;	//重复计数器，高级定时器才有
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
	TIM_SetCounter(TIM3,0);		//定时器计数设置为0
}

void EXTI_Init_SR04(void)	//外部中断 PA1 初始化
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	//AFIO主要有2个功能：1.重映射配置，2.打开引脚对应的外部中断信号使其能够进入到 EXIT 模块
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);		//选择GPIOA 1引脚的中断线路
	
	//配置外部中断
	EXTI_InitTypeDef EXTI_InitStructure;	
	EXTI_InitStructure.EXTI_Line = EXTI_Line1;		//中断线路1 也就是PA1的
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;		//启用中断线路
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		//中断模式
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;	//上下降沿触发
	EXTI_Init(&EXTI_InitStructure);
	
	//配置中断优先级
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;	//中断通道1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//打开中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//响应优先级
	NVIC_Init(&NVIC_InitStructure);
}

float Get_Length_SR04(void)		//获取距离数据
{
	Trig_Send_SR04();	//发送10us高电平触发信号
	if(Flag_edge == 0 && count > 0)
	{
		distance = (float)count * 17000.0 * 0.000001;		//单位cm
        if(distance < 2.0f || distance > 400.0f)	// 检查测量范围是否合理（SR04测量范围2cm-400cm）
            return 0;  // 超出范围
        return distance;
	}
	return 0;
}
// uint16_t SR04_Array[8];
void Array_fill_SR04(float *array)	//得到8个数据
{
	for(uint8_t i=0; i<=7;i++)
	{
		array[i] = Get_Length_SR04();
		Delay_ms(50);
	}
}


/*滑动滤波获取数据
	*array：存放数据的数组
	length：数组长度
*/
float Get_Length_Filter(float *array, uint8_t length)	
{
	static uint8_t i;
	float temp=0.0f;
	for(i=0; i<length-1; i++)	//滑动替换数据
		array[i] = array[i+1];
	array[7] = Get_Length_SR04();	//新数据
	for(i=0; i<length; i++)	//求平均
		temp +=array[i];
	return	temp/8.0f;
}

void EXTI1_IRQHandler(void)		//外部中断1 函数
{
	if(EXTI_GetITStatus(EXTI_Line1) != RESET)
    {
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)==1)	//上升沿，开始记录高电平时间，转换为距离数据
		{
			Flag_edge = 1;
			count = 0;
			TIM_SetCounter(TIM3, 0);
			TIM_Cmd(TIM3, ENABLE);
		}
		else	//下降沿，结束计时
		{
			TIM_Cmd(TIM3, DISABLE);
			Flag_edge = 0;
			count = TIM_GetCounter(TIM3);
		}
		EXTI_ClearITPendingBit(EXTI_Line1);		// 清除中断标志
	}
}



