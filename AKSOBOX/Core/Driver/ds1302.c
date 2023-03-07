/* USER CODE BEGIN 1 */
 
/* USER CODE BEGIN 3 */
 
#include "ds1302.h"
#include "stdio.h"
data_time_t data_time=
{
		0x23,								//��
		0x02,								//��
		0x28,								//��
		0x13,								//ʱ
		0x28,								//��
		0x00,								//��
		0x02,								//������
	  0
};  //д���ʼ����ʱ��Ĭ��ֵ
/*****************************************
* ��������void DS1302_delay(u8 dd)
* ����  ������ʱ
* ����  ��
* ���  ����
* ����  ��
*************************************/
void DS1302_delay(uint8_t dd)
{
    uint8_t i;
    for(;dd>0;dd--)
		for(i=110;i>0;i--);
}
 
/*****************************************
* ��������void DS1302_GPIOInit(void)
* ����  ��DS1302 GPIO����
* ����  ��
* ���  ����
* ����  ��
	CE---PB8,
	CLK--->PB6,
  SDA--->PB7,
*************************************/
void DS1302_GPIOInit(void)
{
		GPIO_InitTypeDef GPIO_InitStructure = {0};
		__HAL_RCC_GPIOB_CLK_ENABLE();	
		__HAL_RCC_GPIOA_CLK_ENABLE();   //ʹ��GPIOAʱ��
		__HAL_RCC_GPIOC_CLK_ENABLE();   //ʹ��GPIOCʱ��
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
* ��������void DS1302_IO_GPIO(void)
* ����  ��DS1302 ֮ IO GPIO �����������
* ����  ��FLAG��־λ
* ���  ����
* ����  ��OUT:��ʾ�����IN:��ʾ����
         FLAG��
*************************************/
void DS1302_IO_GPIO(uchar FLAG)
{
	GPIO_InitTypeDef GPIO_InitStructre;
	/**********��������IO�˿� ��� **********/
	if(FLAG==0x01)
  {
      GPIO_InitStructre.Pin= ds1302_SDA;//����IO_GPIO
			GPIO_InitStructre.Speed=GPIO_SPEED_FREQ_HIGH;
			GPIO_InitStructre.Mode=GPIO_MODE_OUTPUT_PP;
			HAL_GPIO_Init(ds1302_SDA_GPIO, &GPIO_InitStructre);
    }
      /**********��������IO�˿� ����**********/
    else if(FLAG==0x00)
    {
       GPIO_InitStructre.Pin= ds1302_SDA;//����IO_GPIO
			 GPIO_InitStructre.Speed=GPIO_SPEED_FREQ_HIGH;
       GPIO_InitStructre.Mode=GPIO_MODE_INPUT;  //������������ 
       HAL_GPIO_Init(ds1302_SDA_GPIO, &GPIO_InitStructre);
     }
  }
 
void ds1302_write_gpio_init(void)
{
		HAL_GPIO_WritePin(ds1302_SDA_GPIO,ds1302_SDA,GPIO_PIN_SET);								//��ʼ������SDA
		HAL_GPIO_WritePin(ds1302_SCL_GPIO,ds1302_SCL,GPIO_PIN_SET);								//��ʼ������SCL
		HAL_GPIO_WritePin(ds1302_CE_GPIO,ds1302_CE_PIN,GPIO_PIN_RESET);				//��ʼ������CE
}
/*****************************************
* ��������void ds1302_write_byte(uchar addr, uchar d)
* ����  ��DS1302дָ�������
* ����  ��add:���͵�ַ��dat����������
* ���  ����
* ����  ��
*************************************/
void ds1302_write_byte(uchar addr, uchar d)
{
    uchar i;
    CE_H();
    for (i = 0; i < 8; i ++)						//���͵�ַ   
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
    for (i = 0; i < 8; i ++) 						 //��������
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
* ��������uchar ds1302_read_byte(uchar addr)
* ����  ��DS1302������
* ����  ��add:��ַ
* ���  ����
* ����  ��
*************************************/
uchar ds1302_read_byte(uchar addr)
{
    uchar i=0,temp=0,mm=0,nn=0,value=0;
    CE_H();
    for (i = 0; i < 8; i ++) //д��ַ
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
		DS1302_IO_GPIO(0);				//����Ϊ����
    for (i = 0; i < 8; i ++)  //������
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
		DS1302_IO_GPIO(1);				//����Ϊ���	
    mm=temp/16;
		nn=temp%16;
		value=mm*10+nn;					 //���ݴ���ת��ʮ����  
    return value;
}
void ds1302_write_time(void)
{
//    printf("set time %x-%x-%x %x:%x:%x %x\r\n",data_time.year,data_time.months,data_time.day,data_time.hour,data_time.min,data_time.second,data_time.week_time);
    ds1302_write_byte(ds1302_control_add,0x00);									//�ر�д�����������������Ĵ���д����
	  ds1302_write_byte(ds1302_year_add,data_time.year);					//��ʼ����
		ds1302_write_byte(ds1302_month_add ,data_time.months);			//��ʼ����
		ds1302_write_byte(ds1302_day_add,data_time.day);						//��ʼ����
		ds1302_write_byte(ds1302_hr_add,data_time.hour);						//��ʼ��ʱ
		ds1302_write_byte(ds1302_min_add,data_time.min);						//��ʼ����
		ds1302_write_byte(ds1302_sec_add,data_time.second);					//��ʼ����
		ds1302_write_byte(ds1302_week_time_add,data_time.week_time);//��ʼ��������
    ds1302_write_byte(ds1302_control_add,0x80);								  //��д��������ֹ���κ������Ĵ���д����							
}
 
/*****************************************
* ��������void DS1302_init(uchar *time)
* ����  ��    DS1302��ʼ��
* ����  ����
* ���  ����
* ����  ��
*************************************/
void DS1302_init(void)
{
	 DS1302_GPIOInit();  //GPIO��ʼ������
   DS1302_delay(2);
   CE_L() ;    				//CE=0;
   DS1302_SCL_L();    // CLK=0;
    //�����Ƕ�DS1302������أ�������   ����ʱ��
   ds1302_write_byte(ds1302_control_add,0x00);      //WP=0 ��������д��DS1302
   ds1302_write_byte(ds1302_charger_add,0xA7);      //���(1��������+8K����)
   ds1302_write_byte(ds1302_control_add,0X80);      //�������� WP=1
   if(ds1302_read_byte(0X81)&0X80)									//��ѯDS302ʱ���Ƿ�����,���ʱ��ֹͣ�߶�������ʱ��+��ʼ��ʱ��
   { 
      ds1302_write_time();                          //���ó�ʼʱ�ӣ�оƬ��һ�ι�����Ҫд���ʼֵ�������޷���ʱ
   }
   else
	 {
			//ds1302_write_time();                         //��һ�γ�ʼ��֮����Ҫע�͸���䣬����û��reset��������д���ʼ��ʱ�䡣ֻҪŦ�۵�ش��ڣ���ʼ��ֻ��Ҫд��һ�ξͿ��������ˡ�
	 }   
 }
/*****************************************
* ��������void ds1302_read_time(uchar *time)
* ����  ��DS1302��ȡʱ��
* ����  ����
* ���  ����
* ����  ��
*************************************/
void ds1302_read_time(void)
{	
    data_time.label2[0]   = ds1302_read_byte( 0x8D)/10;
		data_time.label2[1]   = ds1302_read_byte( 0x8D)%10;    //��
		data_time.label2[2]   = '/';
		data_time.label2[3]   = ds1302_read_byte( 0x89)/10;
		data_time.label2[4]   = ds1302_read_byte( 0x89)%10;    //��
		data_time.label2[5]   = '/';
	  data_time.label2[6]   = ds1302_read_byte( 0x87)/10;
		data_time.label2[7]   = ds1302_read_byte( 0x87)%10;     //��
	
	  data_time.label2[8]   = ds1302_read_byte( 0x85)/10;
		data_time.label2[9]   = ds1302_read_byte( 0x85)%10;     //ʱ
		data_time.label2[10]   = ':';
		data_time.label2[11]   = ds1302_read_byte( 0x83)/10;
		data_time.label2[12]   = ds1302_read_byte( 0x83)%10;    //��
		data_time.label2[13]   = ':';
		data_time.label2[14]   = ds1302_read_byte( 0x81)/10;
		data_time.label2[15]   = ds1302_read_byte( 0x81)%10;    //��
	  //printf("read time %d%d%c%d%d%c%d%d  %d%d%c%d%d%c%d%d\r\n",data_time.label2[0],data_time.label2[1],data_time.label2[2],data_time.label2[3],data_time.label2[4],data_time.label2[5],data_time.label2[6],data_time.label2[7],data_time.label2[8],data_time.label2[9],data_time.label2[10],data_time.label2[11],data_time.label2[12],data_time.label2[13],data_time.label2[14],data_time.label2[15]);
}
 
 
/* USER CODE END 1 */