#include "BKP.h"                  // Device header

/*	WDG（Watchdog）看门狗
	看门狗可以监控程序的运行状态，当程序因为设计漏洞、硬件故障、电磁干扰等原因，
	出现卡死或跑飞现象时，看门狗能及时复位程序，避免程序陷入长时间的罢工状态，保证系统的可靠性和安全性

	当看门狗定时器超时并触发系统复位后，程序会从头开始运行
	
	STM32内置两个看门狗
	独立看门狗（IWDG）：独立工作，对时间精度要求较低（独立：使用内置LSI时钟40kHz）
	窗口看门狗（WWDG）：要求看门狗在精确计时窗口起作用（在特定时间区间喂狗，使用APB1时钟）

	IWDG超时时间(ms)
           TIWDG = 1/40 × PR预分频系数 × (RL + 1)
	WWDG超时时间
			TWWDG = TPCLK1 × 4096 × WDGTB预分频系数 × (T[5:0] + 1)
		窗口时间：
			TWIN = TPCLK1 × 4096 × WDGTB预分频系数 × (T[5:0] - W[5:0])

	
*/
void IWDG_Init(void)	//设置独立看门狗1000ms的定时间隔
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);	//解除写保护
	IWDG_SetPrescaler(IWDG_Prescaler_16);			//设置预分频系数
	IWDG_SetReload(2499);		//设置重装计数器
	IWDG_ReloadCounter();		//喂狗
	IWDG_Enable();				//开启看门狗后，系统自动开始LSI 40kHz时钟
}

void WWDG_Init(void)	//设置独立看门狗1000ms的定时间隔
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
	WWDG_SetPrescaler(IWDG_Prescaler_16);		//设置预分频系数
	WWDG_SetWindowValue(0x40 | 21);		//设置窗口值30ms
	WWDG_Enable(0x40 | 54);			//开启看门狗，设置计数值20ms(需要设置第6位为1)
//	整个循环周期为50ms，其中前面30ms为非窗口期，喂狗会产生复位，
//	后面20ms就是窗口期，喂狗重置计数器，再50ms后就溢出复位了
//	WWDG_SetCounter();		//喂狗
}


/* 测试程序代码 1 独立看门狗
	OLED_Init();
	Key_Init();
	
	OLED_ShowString(1, 1, "IWDG TEST");
	
	if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) == SET)
	{
		OLED_ShowString(2, 1, "IWDGRST");
		Delay_ms(500);
		OLED_ShowString(2, 1, "       ");
		Delay_ms(100);
		
		RCC_ClearFlag();
	}
	else
	{
		OLED_ShowString(3, 1, "RST");
		Delay_ms(500);
		OLED_ShowString(3, 1, "   ");
		Delay_ms(100);
	}
	
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(IWDG_Prescaler_16);
	IWDG_SetReload(2499);					//1000ms
	IWDG_ReloadCounter();
	IWDG_Enable();
	
	while (1)
	{
		Key_GetNum();
		
		IWDG_ReloadCounter();	//循环喂狗
		
		OLED_ShowString(4, 1, "FEED");
		Delay_ms(200);
		OLED_ShowString(4, 1, "    ");
		Delay_ms(600);
	}

*/

/* 测试程序代码 2 窗口看门狗
	OLED_Init();
	Key_Init();
	
	OLED_ShowString(1, 1, "WWDG TEST");
	
	if (RCC_GetFlagStatus(RCC_FLAG_WWDGRST) == SET)
	{
		OLED_ShowString(2, 1, "WWDGRST");
		Delay_ms(500);
		OLED_ShowString(2, 1, "       ");
		Delay_ms(100);
		
		RCC_ClearFlag();
	}
	else
	{
		OLED_ShowString(3, 1, "RST");
		Delay_ms(500);
		OLED_ShowString(3, 1, "   ");
		Delay_ms(100);
	}
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
	
	WWDG_SetPrescaler(WWDG_Prescaler_8);
	WWDG_SetWindowValue(0x40 | 21);		//30ms
	WWDG_Enable(0x40 | 54);				//50ms
	
	while (1)
	{
		Key_GetNum();
		
		OLED_ShowString(4, 1, "FEED");
		Delay_ms(20);
		OLED_ShowString(4, 1, "    ");
		Delay_ms(20);
		
		WWDG_SetCounter(0x40 | 54);
	}

*/



