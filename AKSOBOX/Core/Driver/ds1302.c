/* USER CODE BEGIN 1 */
 
/* USER CODE BEGIN 3 */
 
#include "ds1302.h"
#include "stdio.h"
data_time_t data_time=
{
		0x23,								//年
		0x02,								//月
		0x28,								//日
		0x13,								//时
		0x28,								//分
		0x00,								//秒
		0x02,								//星期日
	  0
};  //写入初始化的时间默认值
/*****************************************
* 函数名：void DS1302_delay(u8 dd)
* 描述  ：简单延时
* 输入  ：
* 输出  ：无
* 调用  ：
*************************************/
void DS1302_delay(uint8_t dd)
{
    uint8_t i;
    for(;dd>0;dd--)
		for(i=110;i>0;i--);
}
 
/*****************************************
* 函数名：void DS1302_GPIOInit(void)
* 描述  ：DS1302 GPIO配置
* 输入  ：
* 输出  ：无
* 调用  ：
	CE---PB8,
	CLK--->PB6,
  SDA--->PB7,
*************************************/
void DS1302_GPIOInit(void)
{
		GPIO_InitTypeDef GPIO_InitStructure = {0};
		__HAL_RCC_GPIOB_CLK_ENABLE();	
		__HAL_RCC_GPIOA_CLK_ENABLE();   //使能GPIOA时钟
		__HAL_RCC_GPIOC_CLK_ENABLE();   //使能GPIOC时钟
		  /*Configure GPIO pin : SDA  SCL */
   GPIO_InitStructure.Pin = ds1302_SDA | ds1302_SCL;
	 GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
   GPIO_InitStructure.Pull = GPIO_NOPULL;
   GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
   HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
		
	/*Configure GPIO pin : CE */
   GPIO_InitStructure.Pin = ds1302_CE_PIN;
   GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
   GPIO_InitStructure.Pull = GPIO_NOPULL;
   GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
   HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
}
 /*****************************************
* 函数名：void DS1302_IO_GPIO(void)
* 描述  ：DS1302 之 IO GPIO 输入输出配置
* 输入  ：FLAG标志位
* 输出  ：无
* 调用  ：OUT:表示输出，IN:表示输入
         FLAG：
*************************************/
void DS1302_IO_GPIO(uchar FLAG)
{
	GPIO_InitTypeDef GPIO_InitStructre;
	/**********配置数据IO端口 输出 **********/
	if(FLAG==0x01)
  {
      GPIO_InitStructre.Pin= ds1302_SDA;//配置IO_GPIO
			GPIO_InitStructre.Speed=GPIO_SPEED_FREQ_HIGH;
			GPIO_InitStructre.Mode=GPIO_MODE_OUTPUT_PP;
			HAL_GPIO_Init(ds1302_SDA_GPIO, &GPIO_InitStructre);
    }
      /**********配置数据IO端口 输入**********/
    else if(FLAG==0x00)
    {
       GPIO_InitStructre.Pin= ds1302_SDA;//配置IO_GPIO
			 GPIO_InitStructre.Speed=GPIO_SPEED_FREQ_HIGH;
       GPIO_InitStructre.Mode=GPIO_MODE_INPUT;  //配置上拉输入 
       HAL_GPIO_Init(ds1302_SDA_GPIO, &GPIO_InitStructre);
     }
  }
 
void ds1302_write_gpio_init(void)
{
		HAL_GPIO_WritePin(ds1302_SDA_GPIO,ds1302_SDA,GPIO_PIN_SET);								//初始化设置SDA
		HAL_GPIO_WritePin(ds1302_SCL_GPIO,ds1302_SCL,GPIO_PIN_SET);								//初始化设置SCL
		HAL_GPIO_WritePin(ds1302_CE_GPIO,ds1302_CE_PIN,GPIO_PIN_RESET);				//初始化设置CE
}
/*****************************************
* 函数名：void ds1302_write_byte(uchar addr, uchar d)
* 描述  ：DS1302写指令和数据
* 输入  ：add:发送地址，dat：所在数据
* 输出  ：无
* 调用  ：
*************************************/
void ds1302_write_byte(uchar addr, uchar d)
{
    uchar i;
    CE_H();
    for (i = 0; i < 8; i ++)						//发送地址   
    {
        if (addr & 0x01)
        {
            DS1302_SDA_H();
        }
        else
        {
            DS1302_SDA_L();
        }
				addr = addr >> 1;
        DS1302_SCL_H();
        DS1302_delay(2);
        DS1302_SCL_L();   
    }
    for (i = 0; i < 8; i ++) 						 //发送数据
    {
        if (d & 0x01)
        {
            DS1302_SDA_H();
        }
        else
        {
            DS1302_SDA_L();
        }
				d = d >> 1;
        DS1302_SCL_H();
        DS1302_delay(2);
        DS1302_SCL_L();
        
    }
    CE_L();
}
/*****************************************
* 函数名：uchar ds1302_read_byte(uchar addr)
* 描述  ：DS1302读数据
* 输入  ：add:地址
* 输出  ：无
* 调用  ：
*************************************/
uchar ds1302_read_byte(uchar addr)
{
    uchar i=0,temp=0,mm=0,nn=0,value=0;
    CE_H();
    for (i = 0; i < 8; i ++) //写地址
    {
        if (addr & 0x01)
        {
            DS1302_SDA_H();
        }
        else
        {
            DS1302_SDA_L();
        }
				addr = addr >> 1;
        DS1302_SCL_H();
        DS1302_delay(2);  
        DS1302_SCL_L();        
    }
		DS1302_IO_GPIO(0);				//配置为输入
    for (i = 0; i < 8; i ++)  //读数据
    {
        temp = temp >> 1;
        if(HAL_GPIO_ReadPin(ds1302_SDA_GPIO, ds1302_SDA)==1)
        {
            temp |= 0x80;
        }
        else
        {
            temp &= 0x7F;
        }
        DS1302_SCL_H();
        DS1302_delay(2);   
        DS1302_SCL_L();
    }	
		CE_L();		
		DS1302_IO_GPIO(1);				//配置为输出	
    mm=temp/16;
		nn=temp%16;
		value=mm*10+nn;					 //数据处理转化十进制  
    return value;
}
void ds1302_write_time(void)
{
//    printf("set time %x-%x-%x %x:%x:%x %x\r\n",data_time.year,data_time.months,data_time.day,data_time.hour,data_time.min,data_time.second,data_time.week_time);
    ds1302_write_byte(ds1302_control_add,0x00);									//关闭写保护，允许向其他寄存器写数据
	  ds1302_write_byte(ds1302_year_add,data_time.year);					//初始化年
		ds1302_write_byte(ds1302_month_add ,data_time.months);			//初始化月
		ds1302_write_byte(ds1302_day_add,data_time.day);						//初始化日
		ds1302_write_byte(ds1302_hr_add,data_time.hour);						//初始化时
		ds1302_write_byte(ds1302_min_add,data_time.min);						//初始化分
		ds1302_write_byte(ds1302_sec_add,data_time.second);					//初始化秒
		ds1302_write_byte(ds1302_week_time_add,data_time.week_time);//初始化工作日
    ds1302_write_byte(ds1302_control_add,0x80);								  //打开写保护，阻止向任何其他寄存器写数据							
}
 
/*****************************************
* 函数名：void DS1302_init(uchar *time)
* 描述  ：    DS1302初始化
* 输入  ：无
* 输出  ：无
* 调用  ：
*************************************/
void DS1302_init(void)
{
	 DS1302_GPIOInit();  //GPIO初始化配置
   DS1302_delay(2);
   CE_L() ;    				//CE=0;
   DS1302_SCL_L();    // CLK=0;
    //下面是对DS1302启动电池，不掉电   设置时间
   ds1302_write_byte(ds1302_control_add,0x00);      //WP=0 允许数据写入DS1302
   ds1302_write_byte(ds1302_charger_add,0xA7);      //充电(1个二极管+8K电阻)
   ds1302_write_byte(ds1302_control_add,0X80);      //开启保护 WP=1
   if(ds1302_read_byte(0X81)&0X80)									//查询DS302时钟是否启动,如果时钟停止走动：启动时钟+初始化时钟
   { 
      ds1302_write_time();                          //设置初始时钟，芯片第一次工作需要写入初始值，否则无法计时
   }
   else
	 {
			//ds1302_write_time();                         //第一次初始化之后需要注释该语句，否则没次reset都会重新写入初始化时间。只要纽扣电池存在，初始化只需要写入一次就可以屏蔽了。
	 }   
 }
/*****************************************
* 函数名：void ds1302_read_time(uchar *time)
* 描述  ：DS1302读取时间
* 输入  ：无
* 输出  ：无
* 调用  ：
*************************************/
void ds1302_read_time(void)
{	
    data_time.label2[0]   = ds1302_read_byte( 0x8D)/10;
		data_time.label2[1]   = ds1302_read_byte( 0x8D)%10;    //年
		data_time.label2[2]   = '/';
		data_time.label2[3]   = ds1302_read_byte( 0x89)/10;
		data_time.label2[4]   = ds1302_read_byte( 0x89)%10;    //月
		data_time.label2[5]   = '/';
	  data_time.label2[6]   = ds1302_read_byte( 0x87)/10;
		data_time.label2[7]   = ds1302_read_byte( 0x87)%10;     //日
	
	  data_time.label2[8]   = ds1302_read_byte( 0x85)/10;
		data_time.label2[9]   = ds1302_read_byte( 0x85)%10;     //时
		data_time.label2[10]   = ':';
		data_time.label2[11]   = ds1302_read_byte( 0x83)/10;
		data_time.label2[12]   = ds1302_read_byte( 0x83)%10;    //分
		data_time.label2[13]   = ':';
		data_time.label2[14]   = ds1302_read_byte( 0x81)/10;
		data_time.label2[15]   = ds1302_read_byte( 0x81)%10;    //秒
	  //printf("read time %d%d%c%d%d%c%d%d  %d%d%c%d%d%c%d%d\r\n",data_time.label2[0],data_time.label2[1],data_time.label2[2],data_time.label2[3],data_time.label2[4],data_time.label2[5],data_time.label2[6],data_time.label2[7],data_time.label2[8],data_time.label2[9],data_time.label2[10],data_time.label2[11],data_time.label2[12],data_time.label2[13],data_time.label2[14],data_time.label2[15]);
}
 
 
/* USER CODE END 1 */