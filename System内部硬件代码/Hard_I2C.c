#include "Hard_I2C.h"                  // Device header
#include "Delay.h" 

/*	I2C（Inter IC Bus）通信协议
	两根通信线：时钟线SCL（Serial Clock）、双向的数据线SDA（Serial Data）
	STM32C8T6有I2C1(PB6-SCL，PB7-SDA)，I2C2(PB10-SCL，PB11-SDA)

*/

void MyI2C_Hard_W_SCL(uint8_t value)
{
	GPIO_WriteBit(GPIOB, GPIO_Pin_10, (BitAction)value);
	Delay_us(10);
}

void MyI2C_Hard_W_SDA(uint8_t value)
{
	GPIO_WriteBit(GPIOB, GPIO_Pin_11, (BitAction)value);
	Delay_us(10);
}

uint8_t MyI2C_Hard_R_SDA(void)
{
	uint8_t value;
	value = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11);
	Delay_us(10);
	return value;
}

void Myi2c_Hard_Init(void)	
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);	//打开I2C 2
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;		//复用开漏，复用：让硬件接管IO口
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	I2C_InitTypeDef I2C_InitStucture;
	I2C_InitStucture.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStucture.I2C_ClockSpeed = 50000;	//SCL时钟速度，最大400kHz，高于100kHz为快速模式
	I2C_InitStucture.I2C_DutyCycle = I2C_DutyCycle_2;	//时钟占空比，低电平:高电平时间，快速模式时有效，标准模式为1:1
	I2C_InitStucture.I2C_Ack = I2C_Ack_Enable;		//应答位配置
	I2C_InitStucture.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; //作为从机模式可以响应的地址位数
	I2C_InitStucture.I2C_OwnAddress1 = 0x00;	//作为从机时的地址
	I2C_Init(I2C2, &I2C_InitStucture);
	
	I2C_Cmd(I2C2, ENABLE);
}


void Myi2c_Hard_start(void)	//产生起始条件
{
	I2C_GenerateSTART(I2C2, ENABLE);
	while(I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS);	//等待EV5事件
}

void Myi2c_Hard_stop(void)
{
	MyI2C_Hard_W_SDA(0);
	MyI2C_Hard_W_SCL(1);
	MyI2C_Hard_W_SDA(1);
}

//发送一个字节（函数进来时，SCL低电平）
void Myi2c_Hard_SendByte(uint8_t byte)
{
	for(uint8_t i=0; i<8; i++)
	{
		MyI2C_Hard_W_SDA(byte & (0x80 >> i));	//主机写入数据，依次从高位读取8位
		MyI2C_Hard_W_SCL(1);		//从机读取
		MyI2C_Hard_W_SCL(0);		//主机写入数据，保证最后SCL为低
	}
}

//读取一个字节（函数进来时，SCL低电平）
uint8_t Myi2c_Hard_ReadByte(void)
{
	uint8_t value  = 0x00;
	MyI2C_Hard_W_SDA(1);		//主机释放SDA
	for(uint8_t i=0; i<8; i++)
	{
		MyI2C_Hard_W_SCL(1);		//主机读取从机数据
		if (MyI2C_Hard_R_SDA() == 1){value |= (0x80 >> i);}
		MyI2C_Hard_W_SCL(0); 	//从机输入数据
	}
	return value;
}

//主机发送应答，0表示应答，1表示非应答（函数进来时，SCL低电平）
void Myi2c_Hard_SendAck(uint8_t ackbit)
{
	MyI2C_Hard_W_SDA(ackbit);	//主机写入数据，依次从高位读取8位
	MyI2C_Hard_W_SCL(1);		//从机读取
	MyI2C_Hard_W_SCL(0);		//主机写入数据，保证最后SCL为低
}

//主机接收应答（函数进来时，SCL低电平）
uint8_t Myi2c_Hard_ReadAck(void)
{
	uint8_t ackbit;
	MyI2C_Hard_W_SDA(1);		//主机释放SDA
	MyI2C_Hard_W_SCL(1);		//主机读取从机数据
	ackbit = MyI2C_Hard_R_SDA();
	MyI2C_Hard_W_SCL(0); 	//从机输入数据
	return ackbit;
}



/*	主程序代码
	uint8_t ack;
	Myi2c_start();
	Myi2c_sendbyte(0xD0);
	ack = Myi2c_readack();

	Myi2c_stop();
*/



