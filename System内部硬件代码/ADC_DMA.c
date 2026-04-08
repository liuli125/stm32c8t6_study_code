#include "ADC_DMA.h"                  // Device header


/*	ADC多通道转换配合DMA传输转换的数据
	ADC打开扫描模式
	
	本代码功能：ADC1的通道1234连续转换，轮流把转换结果放到 ADC1->DR寄存器，同时触发一个更新事件
	同时DMA把转换的数据从 ADC1->DR寄存器 拿出来放到 ADC_DMA_Value中
*/

uint16_t ADC_DMA_Value[4];

void ADC_DMA_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);		//打开DMA
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);	//ADC时钟6分频，72/6 = 12Mhz，并且最大支持频率是14Mhz
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;	//模拟输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//规则组：赋值ADC通道转换的序列
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime_55Cycles5);
		
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;		//ADC独立转换模式
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//12个数据通道右对齐
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//没有外部触发，用软件代码触发
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;		//无连续转换(需要不断触发转换)
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;	//扫描模式
	ADC_InitStructure.ADC_NbrOfChannel = 4;		//4个通道
	ADC_Init(ADC1, &ADC_InitStructure);
	
	//配置DMA
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;	//选择存放ADC1转换数据的寄存器地址
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//数据宽度16位(ADC数据12位)
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//地址指针不自增
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC_DMA_Value;	//选择DMA转移的最终变量地址
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;	//数据宽度16位
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;		//转运目标地址自增
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	//数据传输方向，由外设到存储器，即ADC转换数据传到变量
	DMA_InitStructure.DMA_BufferSize = 4;		//传送4个数据
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;	//循环模式DMA_Mode_Circular
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;	//ADC硬件触发（ADC触发DMA）
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;	//优先级
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);	//ADC1的硬件触发只有DMA1的通道1
	
	DMA_Cmd(DMA1_Channel1, ENABLE);		//DMA启动
	ADC_DMACmd(ADC1, ENABLE);	//打开ADC1的DMA输出通道
	ADC_Cmd(ADC1, ENABLE);		//ADC启动
	
	//ADC校准
	ADC_ResetCalibration(ADC1);
	while (ADC_GetResetCalibrationStatus(ADC1) == SET);
	ADC_StartCalibration(ADC1);
	while (ADC_GetCalibrationStatus(ADC1) == SET);
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//如果是连续转运，使用此函数
}

 /*	如果是ADC连续转换，DMA连续转运(自动重装)，则不需要此函数
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
 */

//ADC触发转换，然后DMA触发转运，一次。。连续转运则不需要此函数
void ADC_DMA_GetValue(void)
{
	DMA_Cmd(DMA1_Channel1, DISABLE);
	DMA_SetCurrDataCounter(DMA1_Channel1, 2);
	DMA_Cmd(DMA1_Channel1, ENABLE);
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//如果是连续转运，此函数放到初始化函数
	
	while (DMA_GetFlagStatus (DMA1_FLAG_TC1) == RESET)
	DMA_ClearFlag (DMA1_FLAG_TC1);
}




