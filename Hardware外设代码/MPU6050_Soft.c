#include "MPU6050_SOFT.h"                  // Device header
//#include "MPU6050_HARD.h"                  // Device header

#include "Soft_I2C.h"
#include "MPU6050_Reg.h"

/*	MPU6050 位置传感器，模块I2C地址：0xD0

	本代码使用软件I2C驱动MPU6050传感器
*/
void MPU6050_Soft_Init(void)
{
	Myi2c_Init();
	MPU6050_Soft_WriteReg(MPU6050_PWR_MGMT_1,0x01);	//解除芯片睡眠模式，选择陀螺仪时钟
	MPU6050_Soft_WriteReg(MPU6050_PWR_MGMT_2,0x00);
	MPU6050_Soft_WriteReg(MPU6050_SMPLRT_DIV,0x09);	//采样率分频10（值越小越快）
	MPU6050_Soft_WriteReg(MPU6050_CONFIG,0x06);		//滤波参数最大
	MPU6050_Soft_WriteReg(MPU6050_GYRO_CONFIG,0x18);	//陀螺仪配置，最大量程
	MPU6050_Soft_WriteReg(MPU6050_ACCEL_CONFIG,0x18);//加速度计配置，最大量程32768(16g)
}

//给从机指定寄存器写入一个字节数据
void MPU6050_Soft_WriteReg(uint8_t RegAddress,uint8_t Data)
{
	Myi2c_start();
	Myi2c_SendByte(MPU6050_ADDRESS);  //发送 从机地址(写)
	Myi2c_ReadAck();
	Myi2c_SendByte(RegAddress);		//发送 从机寄存器地址
	Myi2c_ReadAck();
	Myi2c_SendByte(Data);		//发送 写入从机寄存器的数据
	Myi2c_ReadAck();
	Myi2c_stop();
}

//读取从机指定寄存器一个字节数据
uint8_t MPU6050_Soft_ReadReg(uint8_t RegAddress)
{
	uint8_t data;
	Myi2c_start();
	Myi2c_SendByte(MPU6050_ADDRESS);  //发送 从机地址(写)
	Myi2c_ReadAck();
	Myi2c_SendByte(RegAddress);		//发送 从机寄存器地址
	Myi2c_ReadAck();

	Myi2c_start();
	Myi2c_SendByte(MPU6050_ADDRESS | 0x01);  //发送 从机地址(读)
	Myi2c_ReadAck();
	data = Myi2c_ReadByte();	//读取从机数据
	Myi2c_SendAck(1);		//发送非应答
	Myi2c_stop();
	return data;
}

//获取ID号
uint8_t MPU6050_Soft_GetID(void)
{
	return MPU6050_Soft_ReadReg(MPU6050_WHO_AM_I);
}

//获取加速度值和陀螺仪值
//int16_t AX, AY, AZ, GX, GY, GZ;
//MPU6050_GetData(&AX, &AY, &AZ, &GX, &GY, &GZ);
void MPU605_Soft0_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ, 
						int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ)
{
	uint8_t DataH, DataL;
	
	DataH = MPU6050_Soft_ReadReg(MPU6050_ACCEL_XOUT_H);
	DataL = MPU6050_Soft_ReadReg(MPU6050_ACCEL_XOUT_L);
	*AccX = (DataH << 8) | DataL;
	
	DataH = MPU6050_Soft_ReadReg(MPU6050_ACCEL_YOUT_H);
	DataL = MPU6050_Soft_ReadReg(MPU6050_ACCEL_YOUT_L);
	*AccY = (DataH << 8) | DataL;
	
	DataH = MPU6050_Soft_ReadReg(MPU6050_ACCEL_ZOUT_H);
	DataL = MPU6050_Soft_ReadReg(MPU6050_ACCEL_ZOUT_L);
	*AccZ = (DataH << 8) | DataL;
	
	DataH = MPU6050_Soft_ReadReg(MPU6050_GYRO_XOUT_H);
	DataL = MPU6050_Soft_ReadReg(MPU6050_GYRO_XOUT_L);
	*GyroX = (DataH << 8) | DataL;
	
	DataH = MPU6050_Soft_ReadReg(MPU6050_GYRO_YOUT_H);
	DataL = MPU6050_Soft_ReadReg(MPU6050_GYRO_YOUT_L);
	*GyroY = (DataH << 8) | DataL;
	
	DataH = MPU6050_Soft_ReadReg(MPU6050_GYRO_ZOUT_H);
	DataL = MPU6050_Soft_ReadReg(MPU6050_GYRO_ZOUT_L);
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



