#ifndef __AHT10_H
#define __AHT10_H
#include "main.h"

//IO方向设置
#define SDA_IN()  {GPIOC->MODER&=~(3<<(1*2));GPIOC->MODER|=0<<(1*2);}	//PC1输入模式
#define SDA_OUT() {GPIOC->MODER&=~(3<<(1*2));GPIOC->MODER|=1<<(1*2);} 	//PC1输出模式
//IO操作
#define IIC_SCL(n)		(n?HAL_GPIO_WritePin(GPIOD,GPIO_PIN_6,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOD,GPIO_PIN_6,GPIO_PIN_RESET))//SCL
#define IIC_SDA(n)		(n?HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_RESET))//SDA
#define READ_SDA  		HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_1)//输入SDA

//IIC所有操作函数
void IIC_Init(void);                //初始化IIC的IO口
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(u8 txd);			//IIC发送一个字节
u8 IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号

void IIC_Write_One_Byte(u8 daddr, u8 addr, u8 data);
u8 IIC_Read_One_Byte(u8 daddr, u8 addr);


#define AHT10_IIC_ADDR	0x38			//AHT10 IIC地址

#define AHT10_CALIBRATION_CMD 	0xE1 	//校准命令(上电后只需要发送一次)
#define AHT10_NORMAL_CMD 		0xA8 	//正常工作模式
#define AHT10_GET_DATA 			0xAC 	//读取数据命令

u8 AHT10_Init(void);
float AHT10_Read_Temperature(void);
float AHT10_Read_Humidity(void);

#endif



