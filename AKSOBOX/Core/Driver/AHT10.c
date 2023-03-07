#include "aht10.h"
/**
 * @brief	IIC�ײ���ʱ����
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
 * @brief	IIC��ʼ������
 *
 * @param   void
 *
 * @return  void
 */
void IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_GPIOC_CLK_ENABLE();   //ʹ��GPIOCʱ��
	__HAL_RCC_GPIOD_CLK_ENABLE();   //ʹ��GPIODʱ��

    /*
		SCL - PD6		SDA-PC1
	*/
    GPIO_Initure.Pin 	= GPIO_PIN_1;
    GPIO_Initure.Mode 	= GPIO_MODE_OUTPUT_PP;	//�������
    GPIO_Initure.Pull 	= GPIO_PULLUP;        	//����
    GPIO_Initure.Speed 	= GPIO_SPEED_FAST;   	//����
    HAL_GPIO_Init(GPIOC, &GPIO_Initure);
	
	
	GPIO_Initure.Pin = GPIO_PIN_6;
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP; 	//�������
    GPIO_Initure.Pull = GPIO_PULLUP;        	//����
    GPIO_Initure.Speed = GPIO_SPEED_FAST;   	//����
    HAL_GPIO_Init(GPIOD, &GPIO_Initure);

    IIC_SDA(1);
    IIC_SCL(1);
}

/**
 * @brief	����IIC��ʼ�ź�
 *
 * @param   void
 *
 * @return  void
 */
void IIC_Start(void)
{
    SDA_OUT();     //sda�����
    IIC_SDA(1);
    IIC_SCL(1);
    IIC_Delay();
    IIC_SDA(0);//START:when CLK is high,DATA change form high to low
	 IIC_Delay();
    IIC_SCL(0);//ǯסI2C���ߣ�׼�����ͻ��������
}
/**
 * @brief	����IICֹͣ�ź�
 *
 * @param   void
 *
 * @return  void
 */
void IIC_Stop(void)
{
    SDA_OUT();//sda�����
	IIC_SDA(0);
    IIC_SCL(1);
     IIC_Delay();
	IIC_SDA(1);//STOP:when CLK is high DATA change form low to high
	 IIC_Delay();
	IIC_SCL(0);//����I2C���߽����ź�
}

/**
 * @brief	�ȴ�Ӧ���źŵ���
 *
 * @param   void
 *
 * @return  u8		1������Ӧ��ʧ��
 *					0������Ӧ��ɹ�
 */
u8 IIC_Wait_Ack(void)
{
    u16 ucErrTime = 0;
    SDA_IN();      //SDA����Ϊ����
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

    IIC_SCL(0);//ʱ�����0
    return 0;
}
/**
 * @brief	����ACKӦ��
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
 * @brief	������ACKӦ��
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
 * @brief	IIC����һ���ֽ�
 *
 * @param   txd		��Ҫ���͵�����
 *
 * @return  void
 */
void IIC_Send_Byte(u8 txd)
{
    u8 t;
    SDA_OUT();
    IIC_SCL(0);//����ʱ�ӿ�ʼ���ݴ���

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
 * @brief	��1���ֽ�����
 *
 * @param   ack		1������ACK		0������nACK
 *
 * @return  u8		���ض�ȡ����
 */
u8 IIC_Read_Byte(unsigned char ack)
{
    unsigned char i, receive = 0;
    SDA_IN();//SDA����Ϊ����

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
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK

    return receive;
}

/**
 * @brief	��ATH10д������
 *
 * @param   cmd		����
 * @param   data	Ҫд�������
 * @param   len		д�����ݴ�С
 *
 * @return  u8		0,����,����,�������
 */
u8 AHT10_Write_Data(u8 cmd, u8 *data, u8 len)
{
    IIC_Start();
    IIC_Send_Byte((AHT10_IIC_ADDR << 1) | 0); //����������ַ+д����

    if(IIC_Wait_Ack())          //�ȴ�Ӧ��
    {
        IIC_Stop();
        return 1;
    }

    IIC_Send_Byte(cmd);         //д�Ĵ�����ַ
    IIC_Wait_Ack();             //�ȴ�Ӧ��

    for(u8 i = 0; i < len; i++)
    {
        IIC_Send_Byte(data[i]);     //��������
        IIC_Wait_Ack();				//�ȴ�Ӧ��
    }

    IIC_Stop();
    return 0;
}


/**
 * @brief	��һ���ֽ�
 *
 * @param   void
 *
 * @return  u8		����������
 */
u8 AHT10_ReadOneByte(void)
{
    u8 res = 0;
    IIC_Start();
    IIC_Send_Byte((AHT10_IIC_ADDR << 1) | 0X01); //����������ַ+������

    if(IIC_Wait_Ack())          //�ȴ�Ӧ��
    {
        IIC_Stop();
        return 1;
    }

    res = IIC_Read_Byte(0);		//������,����nACK
    IIC_Stop();                 //����һ��ֹͣ����
    return res;
}

/**
 * @brief	������
 *
 * @param   data	���ݻ���
 * @param   len		�����ݴ�С
 *
 * @return  u8		0,����,����,�������
 */
u8 AHT10_Read_Data(u8 *data, u8 len)
{
    IIC_Start();
    IIC_Send_Byte((AHT10_IIC_ADDR << 1) | 0x01); //����������ַ+������

    if(IIC_Wait_Ack())          //�ȴ�Ӧ��
    {
        IIC_Stop();
        return 1;
    }

    for(u8 i = 0; i < len; i++)
    {
        if(i == (len - 1))
            data[i] = IIC_Read_Byte(0);		//������,����nACK

        else
            data[i] = IIC_Read_Byte(1);		//������,����ACK
    }

    IIC_Stop();
    return 0;
}

/**
 * @brief	��ȡ�¶�����
 *
 * @param   void
 *
 * @return  float	�¶����ݣ���λ�����϶ȣ�
 */
float AHT10_Read_Temperature(void)
{
    u8 res = 0;
    u8 cmd[2] = {0, 0};
    u8 temp[6];
    float cur_temp;

    res = AHT10_Write_Data(AHT10_GET_DATA, cmd, 2); //���Ͷ�ȡ��������

    if(res)	return 1;

    res = AHT10_Read_Data(temp, 6);				//��ȡ����

    if(res)	return 1;

    cur_temp = ((temp[3] & 0xf) << 16 | temp[4] << 8 | temp[5]) * 200.0 / (1 << 20) - 50;

    return cur_temp;
}

/**
 * @brief	��ȡʪ������
 *
 * @param   void
 *
 * @return  float	ʪ�����ݣ���λ��%RH��
 */
float AHT10_Read_Humidity(void)
{
    u8 res = 0;
    u8 cmd[2] = {0, 0};
    u8 humi[6];
    float cur_humi;

    res = AHT10_Write_Data(AHT10_GET_DATA, cmd, 2); //���Ͷ�ȡ��������

    if(res)	return 1;

    res = AHT10_Read_Data(humi, 6);				//��ȡ����

    if(res)	return 1;

    cur_humi = ((humi[1]) << 12 | humi[2] << 4 | (humi[3] & 0xF0)) * 100.0 / (1 << 20);

    return cur_humi;
}

/**
 * @brief	ATH10��������ʼ��
 *
 * @param   void
 *
 * @return  u8		0,��ʼ���ɹ�������,ʧ��
 */
u8 AHT10_Init(void)
{
    u8 res;
    u8 temp[2] = {0, 0};

    IIC_Init();		//��ʼ��IIC�ӿڣ�ע�������IIC����Ϊ��SCL-PD6 SDA-PC1

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


