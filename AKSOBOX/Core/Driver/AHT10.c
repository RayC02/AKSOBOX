#include "aht10.h"
/**
 * @brief	IIC底层延时函数
 *
 * @param   void
 *
 * @return  void
 */
void IIC_Delay(void)
{
	HAL_Delay(0.5);
}

/**
 * @brief	IIC初始化函数
 *
 * @param   void
 *
 * @return  void
 */
void IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_GPIOC_CLK_ENABLE();   //使能GPIOC时钟
	__HAL_RCC_GPIOD_CLK_ENABLE();   //使能GPIOD时钟

    /*
		SCL - PD6		SDA-PC1
	*/
    GPIO_Initure.Pin 	= GPIO_PIN_1;
    GPIO_Initure.Mode 	= GPIO_MODE_OUTPUT_PP;	//推挽输出
    GPIO_Initure.Pull 	= GPIO_PULLUP;        	//上拉
    GPIO_Initure.Speed 	= GPIO_SPEED_FAST;   	//快速
    HAL_GPIO_Init(GPIOC, &GPIO_Initure);
	
	
	GPIO_Initure.Pin = GPIO_PIN_6;
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP; 	//推挽输出
    GPIO_Initure.Pull = GPIO_PULLUP;        	//上拉
    GPIO_Initure.Speed = GPIO_SPEED_FAST;   	//快速
    HAL_GPIO_Init(GPIOD, &GPIO_Initure);

    IIC_SDA(1);
    IIC_SCL(1);
}

/**
 * @brief	产生IIC起始信号
 *
 * @param   void
 *
 * @return  void
 */
void IIC_Start(void)
{
    SDA_OUT();     //sda线输出
    IIC_SDA(1);
    IIC_SCL(1);
    IIC_Delay();
    IIC_SDA(0);//START:when CLK is high,DATA change form high to low
	 IIC_Delay();
    IIC_SCL(0);//钳住I2C总线，准备发送或接收数据
}
/**
 * @brief	产生IIC停止信号
 *
 * @param   void
 *
 * @return  void
 */
void IIC_Stop(void)
{
    SDA_OUT();//sda线输出
	IIC_SDA(0);
    IIC_SCL(1);
     IIC_Delay();
	IIC_SDA(1);//STOP:when CLK is high DATA change form low to high
	 IIC_Delay();
	IIC_SCL(0);//发送I2C总线结束信号
}

/**
 * @brief	等待应答信号到来
 *
 * @param   void
 *
 * @return  u8		1，接收应答失败
 *					0，接收应答成功
 */
u8 IIC_Wait_Ack(void)
{
    u16 ucErrTime = 0;
    SDA_IN();      //SDA设置为输入
    IIC_SDA(1);
     IIC_Delay();
    IIC_SCL(1);
     IIC_Delay();

    while(READ_SDA)
    {
        ucErrTime++;
		
        if(ucErrTime > 1000)
        {
            IIC_Stop();
            return 1;
        }
    }

    IIC_SCL(0);//时钟输出0
    return 0;
}
/**
 * @brief	产生ACK应答
 *
 * @param   void
 *
 * @return  void
 */
void IIC_Ack(void)
{
    IIC_SCL(0);
    SDA_OUT();
    IIC_SDA(0);
    IIC_Delay();
    IIC_SCL(1);
     IIC_Delay();
    IIC_SCL(0);
}
/**
 * @brief	不产生ACK应答
 *
 * @param   void
 *
 * @return  void
 */
void IIC_NAck(void)
{
    IIC_SCL(0);
    SDA_OUT();
    IIC_SDA(1);
    IIC_Delay();
    IIC_SCL(1);
     IIC_Delay();
    IIC_SCL(0);
}
/**
 * @brief	IIC发送一个字节
 *
 * @param   txd		需要发送的数据
 *
 * @return  void
 */
void IIC_Send_Byte(u8 txd)
{
    u8 t;
    SDA_OUT();
    IIC_SCL(0);//拉低时钟开始数据传输

    for(t = 0; t < 8; t++)
    {
        IIC_SDA((txd & 0x80) >> 7);
        txd <<= 1;
        IIC_SCL(1);
         IIC_Delay();
        IIC_SCL(0);
         IIC_Delay();
    }
}
/**
 * @brief	读1个字节数据
 *
 * @param   ack		1，发送ACK		0，发送nACK
 *
 * @return  u8		返回读取数据
 */
u8 IIC_Read_Byte(unsigned char ack)
{
    unsigned char i, receive = 0;
    SDA_IN();//SDA设置为输入

    for(i = 0; i < 8; i++)
    {
        IIC_SCL(0);
         IIC_Delay();
        IIC_SCL(1);
        receive <<= 1;
        if(READ_SDA)receive++;
        IIC_Delay();
    }

    if(!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK

    return receive;
}

/**
 * @brief	向ATH10写入数据
 *
 * @param   cmd		命令
 * @param   data	要写入的数据
 * @param   len		写入数据大小
 *
 * @return  u8		0,正常,其他,错误代码
 */
u8 AHT10_Write_Data(u8 cmd, u8 *data, u8 len)
{
    IIC_Start();
    IIC_Send_Byte((AHT10_IIC_ADDR << 1) | 0); //发送器件地址+写命令

    if(IIC_Wait_Ack())          //等待应答
    {
        IIC_Stop();
        return 1;
    }

    IIC_Send_Byte(cmd);         //写寄存器地址
    IIC_Wait_Ack();             //等待应答

    for(u8 i = 0; i < len; i++)
    {
        IIC_Send_Byte(data[i]);     //发送数据
        IIC_Wait_Ack();				//等待应答
    }

    IIC_Stop();
    return 0;
}


/**
 * @brief	读一个字节
 *
 * @param   void
 *
 * @return  u8		读到的数据
 */
u8 AHT10_ReadOneByte(void)
{
    u8 res = 0;
    IIC_Start();
    IIC_Send_Byte((AHT10_IIC_ADDR << 1) | 0X01); //发送器件地址+读命令

    if(IIC_Wait_Ack())          //等待应答
    {
        IIC_Stop();
        return 1;
    }

    res = IIC_Read_Byte(0);		//读数据,发送nACK
    IIC_Stop();                 //产生一个停止条件
    return res;
}

/**
 * @brief	读数据
 *
 * @param   data	数据缓存
 * @param   len		读数据大小
 *
 * @return  u8		0,正常,其他,错误代码
 */
u8 AHT10_Read_Data(u8 *data, u8 len)
{
    IIC_Start();
    IIC_Send_Byte((AHT10_IIC_ADDR << 1) | 0x01); //发送器件地址+读命令

    if(IIC_Wait_Ack())          //等待应答
    {
        IIC_Stop();
        return 1;
    }

    for(u8 i = 0; i < len; i++)
    {
        if(i == (len - 1))
            data[i] = IIC_Read_Byte(0);		//读数据,发送nACK

        else
            data[i] = IIC_Read_Byte(1);		//读数据,发送ACK
    }

    IIC_Stop();
    return 0;
}

/**
 * @brief	读取温度数据
 *
 * @param   void
 *
 * @return  float	温度数据（单位：摄氏度）
 */
float AHT10_Read_Temperature(void)
{
    u8 res = 0;
    u8 cmd[2] = {0, 0};
    u8 temp[6];
    float cur_temp;

    res = AHT10_Write_Data(AHT10_GET_DATA, cmd, 2); //发送读取数据命令

    if(res)	return 1;

    res = AHT10_Read_Data(temp, 6);				//读取数据

    if(res)	return 1;

    cur_temp = ((temp[3] & 0xf) << 16 | temp[4] << 8 | temp[5]) * 200.0 / (1 << 20) - 50;

    return cur_temp;
}

/**
 * @brief	读取湿度数据
 *
 * @param   void
 *
 * @return  float	湿度数据（单位：%RH）
 */
float AHT10_Read_Humidity(void)
{
    u8 res = 0;
    u8 cmd[2] = {0, 0};
    u8 humi[6];
    float cur_humi;

    res = AHT10_Write_Data(AHT10_GET_DATA, cmd, 2); //发送读取数据命令

    if(res)	return 1;

    res = AHT10_Read_Data(humi, 6);				//读取数据

    if(res)	return 1;

    cur_humi = ((humi[1]) << 12 | humi[2] << 4 | (humi[3] & 0xF0)) * 100.0 / (1 << 20);

    return cur_humi;
}

/**
 * @brief	ATH10传感器初始化
 *
 * @param   void
 *
 * @return  u8		0,初始化成功，其他,失败
 */
u8 AHT10_Init(void)
{
    u8 res;
    u8 temp[2] = {0, 0};

    IIC_Init();		//初始化IIC接口：注意这里的IIC总线为：SCL-PD6 SDA-PC1

    res = AHT10_Write_Data(AHT10_NORMAL_CMD, temp, 2);

    if(res != 0)	return 1;

    HAL_Delay(300);

    temp[0] = 0x08;
    temp[1] = 0x00;
    res = AHT10_Write_Data(AHT10_CALIBRATION_CMD, temp, 2);

    if(res != 0)	return 1;

    HAL_Delay(300);

    return 0;
}


