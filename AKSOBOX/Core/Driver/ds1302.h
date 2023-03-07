/* USER CODE BEGIN Private defines */
#ifndef _DS1302_H_
#define _DS1302_H_
#include "main.h"
#define uchar unsigned char
#define uint  unsigned int
 //DS1302寄存器操作指令定义时间地址
#define ds1302_week_time_add 0x8a				//工作日，星期1~7	
#define ds1302_sec_add 0x80							//秒
#define ds1302_min_add 0x82							//分
#define ds1302_hr_add 0x84							//时
#define ds1302_day_add 0x86						  //日，1~30日
#define ds1302_month_add 0x88						//月							
#define ds1302_year_add 0x8c						//年
#define ds1302_control_add 0x8e					//写保护
#define ds1302_charger_add 0x90         //充电(1个二极管+8K电阻)
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
	uint8_t		year;				//年
	uint8_t		months;			//月
	uint8_t		day;			  //日
	uint8_t		hour;			  //时
	uint8_t		min;			  //分
	uint8_t		second;			//秒
	uint16_t	week_time;	//周时间，星期1~7
	uint8_t 	label2[16];	//时间存放数组显示格式
}data_time_t;
extern data_time_t data_time;
void DS1302_GPIOInit(void);
void ds1302_write_time(void);
void ds1302_read_time(void);
void DS1302_init(void);
void ds1302_write_gpio_init(void);
 
#endif
 
 
 
 
/* USER CODE END Private defines */