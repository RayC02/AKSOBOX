/* USER CODE BEGIN Private defines */
#ifndef _DS1302_H_
#define _DS1302_H_
#include "main.h"
#define uchar unsigned char
#define uint  unsigned int
 //DS1302�Ĵ�������ָ���ʱ���ַ
#define ds1302_week_time_add 0x8a				//�����գ�����1~7	
#define ds1302_sec_add 0x80							//��
#define ds1302_min_add 0x82							//��
#define ds1302_hr_add 0x84							//ʱ
#define ds1302_day_add 0x86						  //�գ�1~30��
#define ds1302_month_add 0x88						//��							
#define ds1302_year_add 0x8c						//��
#define ds1302_control_add 0x8e					//д����
#define ds1302_charger_add 0x90         //���(1��������+8K����)
#define ds1302_clkburst_add 0xbe
 
 
#define ds1302_SDA_GPIO     GPIOC
#define ds1302_SCL_GPIO     GPIOA
#define ds1302_SDA		GPIO_PIN_6
#define ds1302_SCL		GPIO_PIN_8
 
#define ds1302_CE_GPIO		GPIOB
#define ds1302_CE_PIN		GPIO_PIN_9
 
#define DS1302_SDA_H()        (ds1302_SDA_GPIO->BSRR |= ds1302_SDA)
#define DS1302_SDA_L()        (ds1302_SDA_GPIO->BRR  |= ds1302_SDA)
#define DS1302_SCL_H()        (ds1302_SCL_GPIO->BSRR |= ds1302_SCL)
#define DS1302_SCL_L()        (ds1302_SCL_GPIO->BRR  |= ds1302_SCL)
 
#define CE_H()        (ds1302_CE_GPIO->BSRR |= ds1302_CE_PIN)
#define CE_L()        (ds1302_CE_GPIO->BRR  |= ds1302_CE_PIN)
 
typedef struct 
{
	uint8_t		year;				//��
	uint8_t		months;			//��
	uint8_t		day;			  //��
	uint8_t		hour;			  //ʱ
	uint8_t		min;			  //��
	uint8_t		second;			//��
	uint16_t	week_time;	//��ʱ�䣬����1~7
	uint8_t 	label2[16];	//ʱ����������ʾ��ʽ
}data_time_t;
extern data_time_t data_time;
void DS1302_GPIOInit(void);
void ds1302_write_time(void);
void ds1302_read_time(void);
void DS1302_init(void);
void ds1302_write_gpio_init(void);
 
#endif
 
 
 
 
/* USER CODE END Private defines */