#include "stm32f10x.h"                  // Device header
#include "WS2812.h"
#include "Delay.h"
#include "Random.h"
#include "RGB_2_HSV.h"

#define Code0       2
#define Code1       6
#define CodeReset   0


uint16_t Color_Array[LED_Count][24] = {0};
 
 
void PWM_DMA_WS2812_Init(void)
{
    /*开启时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);			
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    
    /*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                 //将PA0引脚初始化为复用推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;		                //受外设控制的引脚，均需要配置为复用模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);								
																			
    /*配置时钟源选择TIM2为内部时钟，若不调用此函数，TIM默认也为内部时钟*/
	TIM_InternalClockConfig(TIM2);		                           
	
    /*时基单元初始化*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};		//定义结构体变量
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//时钟分频，选择不分频，此参数用于配置滤波器时钟，不影响时基单元功能
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	//计数器模式，选择向上计数
	TIM_TimeBaseInitStructure.TIM_Period = 8 - 1;				    //计数周期，即ARR的值
	TIM_TimeBaseInitStructure.TIM_Prescaler = 8 - 1;				//预分频器，即PSC的值
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;			//重复计数器，高级定时器才会用到
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);				//将结构体变量交给TIM_TimeBaseInit，配置TIM2的时基单元	
	
    /*输出比较初始化*/
    TIM_OCInitTypeDef TIM_OCInitStructure = {0};
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;				//输出比较模式，选择PWM模式1
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;		//输出极性，选择为高，若选择极性为低，则输出高低电平取反
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//输出使能
	TIM_OCInitStructure.TIM_Pulse = 0;								//初始的CCR值
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);						//将结构体变量交给TIM_OC1Init，配置TIM2的输出比较通道1
    TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
    
    TIM_Cmd(TIM2, DISABLE);
	
    /*DMA初始化*/
	DMA_InitTypeDef DMA_InitStructure = {0};									//定义结构体变量
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&TIM2->CCR1;			//外设基地址，给定形参AddrA
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//外设数据宽度，选择半字，对应16为的TIM2的CCR1寄存器
	DMA_InitStructure.DMA_PeripheralInc = 	DMA_PeripheralInc_Disable;			//外设地址自增，选择失能，始终以TIM2的CCR1寄存器为源
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Color_Array;				//存储器基地址
	DMA_InitStructure.DMA_MemoryDataSize = 	DMA_MemoryDataSize_HalfWord;		//存储器数据宽度，选择半字，与源数据宽度对应
	DMA_InitStructure.DMA_MemoryInc = 		DMA_MemoryInc_Enable;				//存储器地址自增，选择使能，每次转运后，数组移到下一个位置
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;							//数据传输方向，选择由存储器到外设
	DMA_InitStructure.DMA_BufferSize = LED_Count * 24;						    //转运的数据大小（转运次数）
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;								//模式，选择普通模式
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;								//存储器到存储器，选择失能
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;						//优先级，选择中等
	DMA_Init(DMA1_Channel5, &DMA_InitStructure);								//将结构体变量交给DMA_Init，配置DMA1的通道5
	
 
    /*DMA使能*/
    TIM_DMACmd(TIM2, TIM_DMA_CC1, ENABLE);
    DMA_Cmd(DMA1_Channel5, DISABLE);
                                                                                //DMA1的通道5使能 
    /*初始化Color_Array数组*/
    for(uint8_t i = 0; i < LED_Count; i++)
    {
        for(uint8_t j = 0; j < 24; j++)
        {
            Color_Array[i][j] = Code0;
        }
    }    
    		
}
 
/*颜色填充，根据缓冲数组进行更新RGB灯的显示*/
void WS2812_Update(void)
{		
	DMA_SetCurrDataCounter(DMA1_Channel5,LED_Count * 24);
	DMA_Cmd(DMA1_Channel5,ENABLE);
	TIM_Cmd(TIM2,ENABLE);
	
	while(DMA_GetFlagStatus(DMA1_FLAG_TC5) != SET);
	DMA_ClearFlag(DMA1_FLAG_TC5);
    
    TIM_SetCompare1(TIM2,0);
	Delay_us(20);
	DMA_Cmd(DMA1_Channel5,DISABLE);
	TIM_Cmd(TIM2,DISABLE);
	
}
 
/*指定灯的颜色(n从1开始)：对应操作就是赋值缓冲数组*/
void WS2812_SetColor(uint8_t n, uint8_t r, uint8_t g, uint8_t b)
{
    static uint8_t i,j,temp[3];
	temp[0] = g;
	temp[1] = r;
	temp[2] = b;
	for(j = 0; j < 3; j++)
	{
		for(i = 0; i < 8; i++)
		{
			if (temp[j] & (0x80 >> i) )
			{   
				Color_Array[n-1][i+j*8] = Code1;
			}
			else
			{
				Color_Array[n-1][i+j*8] = Code0;
			}

		}    
	}
}
 
/*熄灭所有灯*/
void Lights_out(void)
{
    for(uint8_t i = 0; i < LED_Count; i++)
    {
        for(uint8_t j = 0; j < 24; j++)
        {
            Color_Array[i][j] = Code0;
        }
    }
    WS2812_Update();
	
}

//效果：给所有灯上颜色
void WS2812_SetAllLight(uint8_t r, uint8_t g, uint8_t b)
{
	static uint8_t i;
	for(i = 1; i < 65; i ++)
	{
		WS2812_SetColor(i,r,g,b);
	}
	WS2812_Update();
}

//单灯移动流水灯：一个灯从开始位置移动到 num 位置，速度为 speed，
void WS2812_Move(uint16_t num, uint16_t speed)
{
	static uint8_t i,j;
	for(j = num; j>1; j--)
	{
		for(i = 1; i<=j; i++)
		{
			WS2812_SetColor(i ,Random_Get(255),Random_Get(255),Random_Get(255));
			if(i>1)
			{
				WS2812_SetColor(i-1 ,0,0,0);	//上一个灯珠灭
			}
//			if(i == j)  //会依次让最后的灯灭
//			{
//				WS2812_SetColor(i+1 ,0,0,0);	//最后一个灯珠灭
//			}
			WS2812_Update();
			Delay_ms(speed);	//移动速度
		}
	}
}

//流水灯1：一种颜色循环到头
void WS2812_WaterFlow1(uint8_t Light_num)
{
    uint8_t i;
    static uint8_t state;
    for(i = 1; i <= Light_num; i++) 
	{
        switch(state) 
		{
            case 0:  WS2812_SetColor(i, 255, 0, 0);  break;  // 红色
            case 1:  WS2812_SetColor(i, 0, 255, 0);  break;  // 绿色
            case 2:  WS2812_SetColor(i, 0, 0, 255);  break;  // 蓝色
        }
		//循环更新
		WS2812_Update();
		Delay_ms(40);
    }
	state = (state + 1) % 4;
	//同时更新
//    WS2812_Update();
//    Delay_ms(40);
}
//流水灯2：循环渐变
void WS2812_WaterFlow2(uint8_t Light_num)
{
	static uint8_t i, j=0, r=0, g=0, b=0;
	static uint16_t h=0;
	static float s=1, v=1;
    for(i=1; i <= 64; i++)
		{
			HSV_to_RGB(h, s, v, &r, &g, &b);
			WS2812_SetColor(i,r,g,b);
			WS2812_Update();
			h = (h + 10) % 360;
			Delay_ms(50);
		}
}
//给数组填充颜色
static uint8_t Colors_Cycle[64][3] = {0};
void Fill_Color(void)
{
	static uint8_t i, j=0, r=0, g=0, b=0;
	static uint16_t h=0;
	static float s=1, v=1;
	for(i=0; i < 61; i+=4)
	{
		HSV_to_RGB(h, s, v, &r, &g, &b);
		for(j = 0; j <= 3; j++)
		{
			Colors_Cycle[i+j][0] = r;
			Colors_Cycle[i+j][1] = g;
			Colors_Cycle[i+j][2] = b;
		}
		h = (h + 22) % 360;
	}
}

//循环递归流水灯
void WS2812_Cycle(uint8_t Light_num, uint8_t direct, uint16_t speed)
{
	static int i;
	static uint8_t temp1,temp2,temp3;
	for(i=1; i <= Light_num; i++)
	{
		WS2812_SetColor(i, Colors_Cycle[i-1][0], Colors_Cycle[i-1][1], Colors_Cycle[i-1][2]);
	}
	WS2812_Update();	//更新状态
	Delay_ms(speed);
	//递增数组元素
	if(direct == 1)		//正方向
	{
		temp1 = Colors_Cycle[Light_num-1][0];
		temp2 = Colors_Cycle[Light_num-1][1];
		temp3 = Colors_Cycle[Light_num-1][2];
		for(i=62; i >= 0; i--)
		{
			Colors_Cycle[i+1][0] = Colors_Cycle[i][0];  
			Colors_Cycle[i+1][1] = Colors_Cycle[i][1];  
			Colors_Cycle[i+1][2] = Colors_Cycle[i][2];
		}
		Colors_Cycle[0][0] = temp1;
		Colors_Cycle[0][1] = temp2;
		Colors_Cycle[0][2] = temp3;
	}
	else
	{
		temp1 = Colors_Cycle[0][0];
		temp2 = Colors_Cycle[0][1];
		temp3 = Colors_Cycle[0][2];
		for(i=0; i < Light_num-1; i++)
		{
			Colors_Cycle[i][0] = Colors_Cycle[i + 1][0];  
			Colors_Cycle[i][1] = Colors_Cycle[i + 1][1];  
			Colors_Cycle[i][2] = Colors_Cycle[i + 1][2];
		}
		Colors_Cycle[Light_num-1][0] = temp1;
		Colors_Cycle[Light_num-1][1] = temp2;
		Colors_Cycle[Light_num-1][2] = temp3;
	}

}

//全灯变化
void WS2812_AllLight(uint8_t speed)
{
	static uint8_t r=0, g=0, b=0;
	static uint8_t i,j;
	static uint16_t h=0;
	static float s=1, v=1;
	for(i=1; i <= 36; i++)
	{
		HSV_to_RGB(h, s, v, &r, &g, &b);
		for(j = 1; j <= 64; j++)
		{
			WS2812_SetColor(j,r,g,b);
		}
		WS2812_Update();
		h = (h + 10) % 360;
		Delay_ms(speed);
	}
}
/*	两面夹包
	for(i=1; i <= 35; i++)
	{
		j = (j + 1) % 49;
		HSV_to_RGB(h, s, v, &r, &g, &b);
		WS2812_SetColor(j,r,g,b);
		WS2812_SetColor(j+8,r,g,b);
		WS2812_SetColor(j+16,r,g,b);
		WS2812_Update();
		h = (h + 10) % 360;
		Delay_ms(80);
	}
*/

const uint8_t v_breath[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 
                               4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 10, 11, 11, 12, 12, 
                               13, 13, 14, 14, 15, 15, 16, 16, 17, 18, 18, 19, 20, 20, 21, 22, 23, 24, 25, 25, 26, 27, 28, 30, 31, 32, 33, 
                               34, 36, 37, 38, 40, 41, 43, 45, 46, 48, 50, 52, 54, 56, 58, 60, 62, 65, 67, 70, 72, 75, 78, 81, 84, 87, 90, 
                               94, 97, 101, 105, 109, 113, 117, 122, 126, 131, 136, 141, 146, 152, 158, 164, 170, 176, 183, 190, 197, 205, 
                               213, 221, 229, 238, 247, 255, 255, 247, 238, 229, 221, 213, 205, 197, 190, 183, 176, 170, 164, 158, 152, 146, 
                               141, 136, 131, 126, 122, 117, 113, 109, 105, 101, 97, 94, 90, 87, 84, 81, 78, 75, 72, 70, 67, 65, 62, 60, 58, 
                               56, 54, 52, 50, 48, 46, 45, 43, 41, 40, 38, 37, 36, 34, 33, 32, 31, 30, 28, 27, 26, 25, 25, 24, 23, 22, 21, 20, 
                               20, 19, 18, 18, 17, 16, 16, 15, 15, 14, 14, 13, 13, 12, 12, 11, 11, 10, 10, 10, 9, 9, 9, 8, 8, 8, 7, 7, 7, 7, 6, 
                               6, 6, 6, 6, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
                               2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
//呼吸灯
void WS2812_Breath(uint8_t number)
{
	static uint8_t r=0, g=0, b=0;
	static uint8_t i,j;
	static uint8_t num = 0;
	if(num == 0) num = number;
	static uint16_t h=0;
	static float s=1,v_temp = 1;
	static uint16_t index = 0;
	v_temp = v_breath[index] * 0.00392f;  // 1 / 255 = 0.00392
	index++;
	if(index >=300)
	{
		index = 0;
		h = (h + 10) % 360;
		num++;
	}
	HSV_to_RGB(h, s, v_temp, &r, &g, &b);
	WS2812_SetColor(num,r,g,b);
	//全灯
//	for(i = 1; i <= 64; i++)
//	{  
//		WS2812_SetColor(i,r,g,b);
//	}
//	h = (h + 10) % 360;
	WS2812_Update();
	Delay_ms(9);
}














