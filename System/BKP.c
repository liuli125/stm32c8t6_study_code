#include "BKP.h"                  // Device header

/*	Backup：数据备份寄存器
	有掉电/复位 数据不丢失功能，靠外电池供电给VBAT引脚
	
*/
void MyBKP_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP, ENABLE);
	
	PWR_BackupAccessCmd(ENABLE);	//打开BKP备份寄存器和RTC的访问
}

void BKP_Show(void)
{
	BKP_WriteBackupRegister(BKP_DR1, 0x1234);	//写入备份寄存器(有10个)，每个寄存器16位
	OLED_ShowHexNum(1,1,BKP_ReadBackupRegister(BKP_DR1),5);
}



/* 测试程序代码
	OLED_Init();
	Key_Init();

	uint8_t KeyNum;
	uint16_t ArrayWrite[] = {0x1234, 0x5678};
	uint16_t ArrayRead[2];

	OLED_ShowString(1, 1, "W:");
	OLED_ShowString(2, 1, "R:");
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP, ENABLE);

	PWR_BackupAccessCmd(ENABLE);

	while (1)
	{
		KeyNum = Key_GetNum();
		if (KeyNum == 1)
		{
			ArrayWrite[0] ++;
			ArrayWrite[1] ++;

			BKP_WriteBackupRegister(BKP_DR1, ArrayWrite[0]);
			BKP_WriteBackupRegister(BKP_DR2, ArrayWrite[1]);
			
			OLED_ShowHexNum(1, 3, ArrayWrite[0], 4);
			OLED_ShowHexNum(1, 8, ArrayWrite[1], 4);
		}
		
		ArrayRead[0] = BKP_ReadBackupRegister(BKP_DR1);
		ArrayRead[1] = BKP_ReadBackupRegister(BKP_DR2);
		
		OLED_ShowHexNum(2, 3, ArrayRead[0], 4);
		OLED_ShowHexNum(2, 8, ArrayRead[1], 4);
	}

*/

