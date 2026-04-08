#include "Soft_I2C.h"                  // Device header
#include "Delay.h" 

/*	I2C（Inter IC Bus）通信协议
	两根通信线：时钟线SCL（Serial Clock）、双向的数据线SDA（Serial Data）
	选择开漏输出，并且外接通常为4.7KΩ的强上拉
	STM32C8T6有I2C1(PB6-SCL，PB7-SDA)，I2C2(PB10-SCL，PB11-SDA)
	
	起始条件：SCL高电平期间，SDA从高电平切换到低电平(下降沿)
	终止条件：SCL高电平期间，SDA从低电平切换到高电平(上升沿)
	数据结构：SCL低电平期间，主/从机将数据位依次放到SDA线上（高位先行），SCL高电平期间，主/从机接收数据，
		接收完一个字节之后，主/从机在下一个时钟发送一位数据，数据0表示应答，数据1表示非应答

	本代码功能：程序手动翻转高低电平，软件使用I2C协议，
*/

void MyI2C_W_SCL(uint8_t value)
{
	GPIO_WriteBit(GPIOB, GPIO_Pin_10, (BitAction)value);
	Delay_us(10);
}

void MyI2C_W_SDA(uint8_t value)
{
	GPIO_WriteBit(GPIOB, GPIO_Pin_11, (BitAction)value);
	Delay_us(10);
}

uint8_t MyI2C_R_SDA(void)
{
	uint8_t value;
	value = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11);
	Delay_us(10);
	return value;
}

void Myi2c_Init(void)	//
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;	//通用开漏输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB, GPIO_Pin_10 | GPIO_Pin_11);
}

//发送起始位
void Myi2c_start(void)	
{
	MyI2C_W_SDA(1);
	MyI2C_W_SCL(1);
	MyI2C_W_SDA(0);
	MyI2C_W_SCL(0);
}

//发送停止位
void Myi2c_stop(void)	
{
	MyI2C_W_SDA(0);
	MyI2C_W_SCL(1);
	MyI2C_W_SDA(1);
}

//发送一个字节（函数进来时，SCL低电平）
void Myi2c_SendByte(uint8_t byte)
{
	for(uint8_t i=0; i<8; i++)
	{
		MyI2C_W_SDA(byte & (0x80 >> i));	//主机写入数据，依次从高位读取8位
		MyI2C_W_SCL(1);		//从机读取
		MyI2C_W_SCL(0);		//主机写入数据，保证最后SCL为低
	}
}

//读取一个字节（函数进来时，SCL低电平）
uint8_t Myi2c_ReadByte(void)
{
	uint8_t value  = 0x00;
	MyI2C_W_SDA(1);		//主机释放SDA
	for(uint8_t i=0; i<8; i++)
	{
		MyI2C_W_SCL(1);		//主机读取从机数据
		if (MyI2C_R_SDA() == 1){value |= (0x80 >> i);}
		MyI2C_W_SCL(0); 	//从机输入数据
	}
	return value;
}

//主机发送应答，0表示应答，1表示非应答（函数进来时，SCL低电平）
void Myi2c_SendAck(uint8_t ackbit)
{
	MyI2C_W_SDA(ackbit);	//主机写入数据，依次从高位读取8位
	MyI2C_W_SCL(1);		//从机读取
	MyI2C_W_SCL(0);		//主机写入数据，保证最后SCL为低
}

//主机接收应答（函数进来时，SCL低电平）
uint8_t Myi2c_ReadAck(void)
{
	uint8_t ackbit;
	MyI2C_W_SDA(1);		//主机释放SDA
	MyI2C_W_SCL(1);		//主机读取从机数据
	ackbit = MyI2C_R_SDA();
	MyI2C_W_SCL(0); 	//从机输入数据
	return ackbit;
}



/*	主程序代码
	uint8_t ack;
	Myi2c_start();
	Myi2c_sendbyte(0xD0);
	ack = Myi2c_readack();

	Myi2c_stop();
*/



