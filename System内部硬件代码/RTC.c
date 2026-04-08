#include "RTC.h"                  // Device header

/*	Real Time Clock：实时时钟
	时间戳（Unix Timestamp）定义为UTC/GMT的1970年1月1日0时0分0秒开始所经过的秒数
	由RTC内部寄存器存储的秒值，计算得到当前时间日期
	RTC和时钟配置系统处于后备区域，系统复位时数据不清零，断电后可借助VBAT（1.8~3.6V）供电继续走时

	time.h的函数
	time_t time_cnt ：定义秒计数器，类型unsigned int
	struct tm param ：结构体设置年月日时分秒
	time_t mktime(struct tm *) ：日期时间转换为秒计数器
	struct tm *localtime(const time_t *) ：秒计数器转换为日期时间(本地时区)
*/

uint16_t MyRTC_Time[] = {2023, 1, 1, 23, 59, 55};	//定义日期

void MyRTC_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP, ENABLE);
	PWR_BackupAccessCmd(ENABLE);	//打开BKP备份寄存器和RTC的访问
	
	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)	//如果BKP寄存器有数据，就代表备用电池还在供电，不需要重新初始化秒计数器值
	{
//		RCC_LSEConfig(RCC_LSE_ON);		//打开外部低速时钟LSE(32.768kHz)：Low Speed Entenal oscillator
		RCC_LSICmd(ENABLE);		//LSE不起震的备用方案，使用LSI，LSI无法由备用电源供电，故主电源掉电时，RTC走时会暂停
//		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) != SET);	//等待LSE启动完成
		while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) != SET);	//等待LSE启动完成
		
//		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);	//RTC选择LSE为时钟源
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
		RCC_RTCCLKCmd(ENABLE);
		
		RTC_WaitForSynchro();		//等待时钟同步
		RTC_WaitForLastTask();		//等待上一次写操作完成
		
//		RTC_SetPrescaler(32768-1);	//预分频值(重装载计数)，定时1s
		RTC_SetPrescaler(40000-1);	//LSI频率40kHz
		RTC_WaitForLastTask();
		
		MyRTC_SetTime();
		BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);	//备份寄存器写入
	}
	else
	{
		RTC_WaitForSynchro();
		RTC_WaitForLastTask();
	}
}

void MyRTC_SetTime(void)	//设置秒计数器
{
	time_t time_cnt;		//unsigned int
	struct tm time_date;	//日期时间结构体
	
	time_date.tm_year = MyRTC_Time[0] - 1900;
	time_date.tm_mon = MyRTC_Time[1] - 1;
	time_date.tm_mday = MyRTC_Time[2];
	time_date.tm_hour = MyRTC_Time[3];
	time_date.tm_min = MyRTC_Time[4];
	time_date.tm_sec = MyRTC_Time[5];
	
	//把日期时间转换为秒计数值
	time_cnt = mktime(&time_date) - 8 * 60 * 60;	//时区偏移8小时
	
	RTC_SetCounter(time_cnt);
	RTC_WaitForLastTask();
}

void MyRTC_ReadTime(void)	//读取日期时间
{
	time_t time_cnt;		//unsigned int
	struct tm time_date;	//日期时间结构体
	
	time_cnt = RTC_GetCounter() + 8 * 60 * 60;	//时区偏移8小时
	
	//秒计数器转换为日期时间
	time_date = *localtime(&time_cnt);		
	
	MyRTC_Time[0] = time_date.tm_year + 1900;
	MyRTC_Time[1] = time_date.tm_mon + 1;
	MyRTC_Time[2] = time_date.tm_mday;
	MyRTC_Time[3] = time_date.tm_hour;
	MyRTC_Time[4] = time_date.tm_min;
	MyRTC_Time[5] = time_date.tm_sec;
}


/* 测试程序代码
	OLED_Init();
	MyRTC_Init();
	
	OLED_ShowString(1, 1, "Date:XXXX-XX-XX");
	OLED_ShowString(2, 1, "Time:XX:XX:XX");
	OLED_ShowString(3, 1, "CNT :");
	OLED_ShowString(4, 1, "DIV :");
	
	while (1)
	{
		MyRTC_ReadTime();
		
		OLED_ShowNum(1, 6, MyRTC_Time[0], 4);
		OLED_ShowNum(1, 11, MyRTC_Time[1], 2);
		OLED_ShowNum(1, 14, MyRTC_Time[2], 2);
		OLED_ShowNum(2, 6, MyRTC_Time[3], 2);
		OLED_ShowNum(2, 9, MyRTC_Time[4], 2);
		OLED_ShowNum(2, 12, MyRTC_Time[5], 2);
		
		OLED_ShowNum(3, 6, RTC_GetCounter(), 10);
		OLED_ShowNum(4, 6, RTC_GetDivider(), 10);
	}
*/

