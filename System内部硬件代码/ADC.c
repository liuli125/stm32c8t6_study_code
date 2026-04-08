#include "ADC.h"                  // Device header

/*	Analog Digital Convert：ADC模拟-数字转换器
	ADC可以将引脚上连续变化的模拟电压转换为内存中存储的数字变量，建立模拟电路到数字电路的桥梁
	输入电压范围：0~3.3V，转换结果范围：0~4095 (12位)
	18个输入通道，可测量16个外部(GPIO口)和2个内部信号源(温度和内部校准电压)
	其中规则组可以选择16个通道，只有1个存放数据寄存器(16位)，转换结束后有EOC(end of convert)信号，可以进入中断
	注入组可以选择4个通道，有4个存放数据寄存器(16位)，转换结束后有JEOC信号，可以进入中断
	可以选择外部硬件触发，也可以内部代码软件触发
	
	对于ADC1 PA0 ~ PA7，依次对应通道0-7总共8个外部接收通道，PB0,PB1对应通道8,9(stm32只有前面10个通道)
	PC0 ~ PC5对应通道10-15，内部温度传感器通道16，内部参考电压通道17
	
	单次转换模式：触发一次转换一次，下一次需要再触发
	连续转换模式：触发一次就连续转换，不需要再触发
	扫描模式：触发一次就对多个通道依次转换
	(单次非扫描，连续非扫描，单次扫描，连续扫描)
	
	本代码功能：接收 ADC1 的通道1 PA0的模拟量，转换为数字量
*/
void AD_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);	//打开ADC
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//打开GPIO A
	
	//ADC时钟6分频，72/6 = 12Mhz，并且最大支持频率是14Mhz，8分频是9Mhz
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;	//模拟输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;		//PA0
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;		//ADC独立转换模式（单ADC转换）
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//12个数据通道右对齐(使用低12位数据)，左对齐则是使用高12位数据，低4位不用
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//外部触发源选择：没有外部触发，用软件代码触发
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;		//无连续转换(需要不断触发转换)
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;		//非扫描模式(单通道只有序列1有效)
	ADC_InitStructure.ADC_NbrOfChannel = 1;		//只有一个转换通道
	ADC_Init(ADC1, &ADC_InitStructure);
	
	ADC_Cmd(ADC1, ENABLE);	//打开ADC
	
	//ADC校准
	ADC_ResetCalibration(ADC1);		
	while (ADC_GetResetCalibrationStatus(ADC1) == SET);
	ADC_StartCalibration(ADC1);
	while (ADC_GetCalibrationStatus(ADC1) == SET);
	//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//软件触发一次ADC即可连续转换运行
}

// 获取ADC转换的结果。
// parameter：ADC_Channel_0 ~ ADC_Channel_10
uint16_t AD_GetValue(uint8_t ADC_Channel)
{
	//ADC的转换序列位置1(共16个)放置通道ADC_Channel ，
	//采样时间55.5个周期，加上转换时间12.5个周期，总时间为 68/12M
	ADC_RegularChannelConfig(ADC1, ADC_Channel, 1, ADC_SampleTime_55Cycles5);
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//软件触发一次ADC转换(连续转换只需要初始化触发一次)
	while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);		//判断ADC转换完成(EOC标志位需要清除，读取DR会自动清除)
	return ADC_GetConversionValue(ADC1);		//读取转换数据ADC->DR(连续转换只需要这一行代码)
}

//ADC连续转换时的函数
uint16_t AD_GetValue_Continue(uint8_t ADC_Channel)
{
	ADC_RegularChannelConfig(ADC1, ADC_Channel, 1, ADC_SampleTime_55Cycles5);
	return ADC_GetConversionValue(ADC1);		//读取转换数据ADC->DR(连续转换只需要这一行代码)
}


/*	主程序代码
	uint16_t ADValue = AD_GetValue(ADC_Channel_0);  //ADC的转换序列位置1放置通道0，PA0
	float Voltage = (float)ADValue / 4095 * 3.3;	//转换实际数值
	OLED_ShowNum(1, 9, ADValue, 4);
	OLED_ShowNum(2, 9, Voltage, 1);
	OLED_ShowNum(2, 11, (uint16_t)(Voltage * 100) % 100, 2);	//取两位小数
	Delay_ms(100);
*/


