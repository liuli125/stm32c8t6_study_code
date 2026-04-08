#include "MPU6050_HARD.h"                  // Device header
#include "HARD_I2C.h"
#include "MPU6050_Reg.h"

/*	MPU6050 位置传感器，模块I2C地址：0xD0
	STM32C8T6有I2C1(PB6-SCL，PB7-SDA)，I2C2(PB10-SCL，PB11-SDA)
	
	本代码使用硬件I2C驱动MPU6050传感器
*/

//超时退出函数
void Wait_Event(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT)	
{
	uint32_t TimeOut;
	TimeOut = 10000;
	while(I2C_CheckEvent(I2Cx, I2C_EVENT) != SUCCESS)
	{
		TimeOut--;
		if(TimeOut == 0)
		{
			break;
		}
	}
}

void MPU6050_Hard_Init(void)
{
	Myi2c_Hard_Init();
	
	MPU6050_Hard_WriteReg(MPU6050_PWR_MGMT_1,0x01);	//解除芯片睡眠模式，选择陀螺仪时钟
	MPU6050_Hard_WriteReg(MPU6050_PWR_MGMT_2,0x00);
	MPU6050_Hard_WriteReg(MPU6050_SMPLRT_DIV,0x09);	//采样率分频10（值越小越快）
	MPU6050_Hard_WriteReg(MPU6050_CONFIG,0x06);		//滤波参数最大
	MPU6050_Hard_WriteReg(MPU6050_GYRO_CONFIG,0x18);	//陀螺仪配置，最大量程
	MPU6050_Hard_WriteReg(MPU6050_ACCEL_CONFIG,0x18);//加速度计配置，最大量程32768(16g)
}

//给从机指定寄存器写入一个字节数据
void MPU6050_Hard_WriteReg(uint8_t RegAddress,uint8_t Data)
{
//	Myi2c_start();
//	Myi2c_SendByte(MPU6050_ADDRESS);  //发送 从机地址(写)
//	Myi2c_ReadAck();
//	Myi2c_SendByte(RegAddress);		//发送 从机寄存器地址
//	Myi2c_ReadAck();
//	Myi2c_SendByte(Data);		//发送 写入从机寄存器的数据
//	Myi2c_ReadAck();
//	Myi2c_stop();
	
	I2C_GenerateSTART(I2C2, ENABLE);
	Wait_Event(I2C2, I2C_EVENT_MASTER_MODE_SELECT);	//等待EV5事件，选择主机模式
	
	I2C_Send7bitAddress(I2C2, MPU6050_ADDRESS, I2C_Direction_Transmitter);	//发送 从机地址(写)
	Wait_Event(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);	//等待EV6事件，选择发送模式
	
	I2C_SendData(I2C2, RegAddress);		//发送 从机寄存器地址
	Wait_Event(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING);	//等待EV8事件，字节正在发送
	
	I2C_SendData(I2C2, Data);	//发送 写入从机寄存器的数据
	Wait_Event(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED);	//等待EV8_2事件，字节发送完毕
	
	I2C_GenerateSTOP(I2C2, ENABLE);
}

//读取从机指定寄存器一个字节数据
uint8_t MPU6050_Hard_ReadReg(uint8_t RegAddress)
{
	uint8_t data;
//	Myi2c_Hard_start();
//	Myi2c_Hard_SendByte(MPU6050_ADDRESS);  //发送 从机地址(写)
//	Myi2c_Hard_ReadAck();
//	Myi2c_Hard_SendByte(RegAddress);		//发送 从机寄存器地址
//	Myi2c_Hard_ReadAck();

//	Myi2c_Hard_start();		//重复起始条件
//	Myi2c_Hard_SendByte(MPU6050_ADDRESS | 0x01);  //发送 从机地址(读)
//	Myi2c_Hard_ReadAck();
//	data = Myi2c_Hard_ReadByte();	//读取数据
	//	Myi2c_Hard_SendAck(1);	//发送非应答
//	Myi2c_Hard_stop();
	
	
	I2C_GenerateSTART(I2C2, ENABLE);
	Wait_Event(I2C2, I2C_EVENT_MASTER_MODE_SELECT);	//等待EV5事件，选择主机模式
	
	I2C_Send7bitAddress(I2C2, MPU6050_ADDRESS, I2C_Direction_Transmitter);	//发送 从机地址(写)
	Wait_Event(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);	//等待EV6事件，选择发送模式
	
	I2C_SendData(I2C2, RegAddress);		//发送 从机寄存器地址
	Wait_Event(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED);	//等待EV8事件，字节正在发送

	I2C_GenerateSTART(I2C2, ENABLE);	//重复起始条件
	Wait_Event(I2C2, I2C_EVENT_MASTER_MODE_SELECT);	//等待EV5事件，选择主机模式
	
	I2C_Send7bitAddress(I2C2, MPU6050_ADDRESS, I2C_Direction_Receiver);		//发送 从机地址(读)
	Wait_Event(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);//等待EV6事件，选择接收模式

	I2C_AcknowledgeConfig(I2C2, DISABLE);	//设置非应答(要在接收前设置)
	I2C_GenerateSTOP(I2C2,ENABLE);			//产生终止条件
	Wait_Event(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED);	//等待EV7事件，接收了一个字节
	data = I2C_ReceiveData(I2C2);
	
	I2C_AcknowledgeConfig(I2C2, ENABLE);	//修改回默认应答状态
	return data;
}

//获取ID号
uint8_t MPU6050_Hard_GetID(void)
{
	return MPU6050_Hard_ReadReg(MPU6050_WHO_AM_I);
}

//获取加速度值和陀螺仪值
//int16_t AX, AY, AZ, GX, GY, GZ;
//MPU6050_GetData(&AX, &AY, &AZ, &GX, &GY, &GZ);
void MPU6050_Hard_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ, 
						int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ)
{
	uint8_t DataH, DataL;
	
	DataH = MPU6050_Hard_ReadReg(MPU6050_ACCEL_XOUT_H);
	DataL = MPU6050_Hard_ReadReg(MPU6050_ACCEL_XOUT_L);
	*AccX = (DataH << 8) | DataL;
	
	DataH = MPU6050_Hard_ReadReg(MPU6050_ACCEL_YOUT_H);
	DataL = MPU6050_Hard_ReadReg(MPU6050_ACCEL_YOUT_L);
	*AccY = (DataH << 8) | DataL;
	
	DataH = MPU6050_Hard_ReadReg(MPU6050_ACCEL_ZOUT_H);
	DataL = MPU6050_Hard_ReadReg(MPU6050_ACCEL_ZOUT_L);
	*AccZ = (DataH << 8) | DataL;
	
	DataH = MPU6050_Hard_ReadReg(MPU6050_GYRO_XOUT_H);
	DataL = MPU6050_Hard_ReadReg(MPU6050_GYRO_XOUT_L);
	*GyroX = (DataH << 8) | DataL;
	
	DataH = MPU6050_Hard_ReadReg(MPU6050_GYRO_YOUT_H);
	DataL = MPU6050_Hard_ReadReg(MPU6050_GYRO_YOUT_L);
	*GyroY = (DataH << 8) | DataL;
	
	DataH = MPU6050_Hard_ReadReg(MPU6050_GYRO_ZOUT_H);
	DataL = MPU6050_Hard_ReadReg(MPU6050_GYRO_ZOUT_L);
	*GyroZ = (DataH << 8) | DataL;
}

/*
	OLED_Init();
	
	MPU6050_Hard_Init();
	MPU6050_Hard_WriteReg(0x6B,0x00);	//写寄存器功能。解除睡眠模式
	MPU6050_Hard_WriteReg(0x19,0xAA);	//给分频器写数据
	
	uint8_t id,div;
	div = MPU6050_Hard_ReadReg(0x19);
	id = MPU6050_Hard_ReadReg(0x75);
	
	OLED_ShowHexNum(1,1,id,3);
	OLED_ShowHexNum(2,1,div,3);

*/


