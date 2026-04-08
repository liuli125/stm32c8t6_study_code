#include "BKP.h"                  // Device header

/*	PWR：Power Control 电源控制
	睡眠模式：关闭CPU时钟
	停机模式：SRAM和寄存器内容被保留下来
	待机模式：SRAM和寄存器内容丢失，只有备份的寄存器和待机电路维持供电
	
	系统在执行main函数的时候，会先执行system_stm32f10x.c里面的SystemInit函数
	system_stm32f10x.c 中的110行左右，#define SYSCLK_FREQ_72MHz  72000000 可以定义系统主频
	SystemCoreClock 变量可以查看当前系统主频，
	SystemCoreClockUpdate() 更新修改的系统主频
	
	以下函数可以进入低功耗模式
	__WFI();	//中断唤醒
	__WFI();	//事件唤醒
	SCB -> SCR |= SCB_SCR_SLEEPONEXIT;	//中断完再进入睡眠模式
	SCB -> SCR &= !SCB_SCR_SLEEPDEEP;   
	  
	
*/

/*	睡眠模式(任意中断)

	OLED_Init();
	OLED_ShowString(1, 1, "RxData:");
	Serial_Init();
	while (1)
	{
		if (Serial_GetRxFlag() == 1)
		{
			RxData = Serial_GetRxData();
			Serial_SendByte(RxData);
			OLED_ShowHexNum(1, 8, RxData, 2);
		}
		
		OLED_ShowString(2, 1, "Running");
		Delay_ms(100);
		OLED_ShowString(2, 1, "       ");
		Delay_ms(100);
		
		__WFI();	//执行这个函数进入睡眠模式，主程序停止运行，串口中断一次就主程序循环一次
	}
*/


/* 	停机模式(外部中断)
	PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);	 //进入停机模式，开启电压调节器，外部中断唤醒
	SystemInit();	//配置为72mHz(唤醒后为HSI 8mHz)
*/


/* 待机模式
	OLED_Init();
	MyRTC_Init();
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	
	OLED_ShowString(1, 1, "CNT :");
	OLED_ShowString(2, 1, "ALR :");
	OLED_ShowString(3, 1, "ALRF:");
	
	PWR_WakeUpPinCmd(ENABLE);		//开启外部唤醒引脚PA0，上升沿唤醒CPU
	
	uint32_t Alarm = RTC_GetCounter() + 10;	
	RTC_SetAlarm(Alarm);			//设定10秒闹钟
	OLED_ShowNum(2, 6, Alarm, 10);
	
	while (1)
	{
		OLED_ShowNum(1, 6, RTC_GetCounter(), 10);
		OLED_ShowNum(3, 6, RTC_GetFlagStatus(RTC_FLAG_ALR), 1);
		
		OLED_ShowString(4, 1, "Running");
		Delay_ms(100);
		OLED_ShowString(4, 1, "       ");
		Delay_ms(100);
		
		OLED_ShowString(4, 9, "STANDBY");
		Delay_ms(1000);
		OLED_ShowString(4, 9, "       ");
		Delay_ms(100);
		
		OLED_Clear();
		
		PWR_EnterSTANDBYMode();	
		//进入待机模式，唤醒后程序从头开始执行，此代码后面的程序不会执行，实际上while(1)也可以去掉
	}
	
*/



