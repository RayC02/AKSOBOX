#include "deel_mqtt.h"
#include "esp8266.h"
#include "iot_emqx.h"
char mqtt_message[300];	//MQTT���ϱ���Ϣ����
uint8_t temperature;
uint8_t humidity;
u8 t=0;
extern u16 med;
char temp_str[30];    // ��ʱ�Ӵ�
void ReadStrUnit(char * str,char *temp_str,int idx,int len)  // ��ĸ���л�ȡ���Ӵ�������ȵ���ʱ�Ӵ�
{
    int index;
    for(index = 0; index < len; index++)
    {
        temp_str[index] = str[idx+index];
    }
    temp_str[index] = '\0';
}
int GetSubStrPos(char *str1,char *str2)
{
    int idx = 0;
    int len1 = strlen(str1);
    int len2 = strlen(str2);

    if( len1 < len2)
    {
        //printf("error 1 \n"); // �Ӵ���ĸ����
        return -1;
    }

    while(1)
    {
        ReadStrUnit(str1,temp_str,idx,len2);    // ���ϻ�ȡ�Ĵ� ĸ���� idx λ�ô�������ʱ�Ӵ�
        if(strcmp(str2,temp_str)==0)break;      // ����ʱ�Ӵ����Ӵ�һ�£�����ѭ��
        idx++;                                  // �ı��ĸ����ȡ��ʱ�Ӵ���λ��
        if(idx>=len1)return -1;                 // �� idx �Ѿ�����ĸ�����ȣ�˵��ĸ�����������Ӵ�
    }

    return idx;    // �����Ӵ���һ���ַ���ĸ���е�λ��
}


//��Ƭ��״̬�ϱ�
void STM32AHT11_StatusReport(void)
{
	//��ȡ��ʪ����Ϣ
	if(t % 10 == 0) //ÿ100ms��ȡһ��
   {
        temperature = AHT10_Read_Temperature();
		}
				t++;
				HAL_Delay(10);
		   
		
		LCD_ShowString(30, 190, 200, 16, 16, "Temp:   . C");		
		LCD_ShowNum(30 + 48, 190, temperature, 2, 16);					//��ʾ�¶�����
    LCD_ShowNum(30 + 72, 190, (u32)(temperature * 10) % 10, 1, 16);	//��ʾ�¶�С��
	
	//�ϱ�һ������
	uint8_t led_1_status = HAL_GPIO_ReadPin(LED0_GPIO_Port,LED0_Pin) ? 0:1;
/*
	sprintf(mqtt_message,
	"{\"method\":\"thing.service.property.set\",\"id\":\"00000001\",\"params\":{\
		\"Temperature\":%.1f,\
		\"led_1\":%d\
	},\"version\":\"1.0.0\"}",
	(float)temperature,
	led_1_status
	);
*/

	sprintf(mqtt_message,
	"{\"id\":\"00000001\",\"params\":{\
		\"Temperature\":%.1f,\
		\"led_1\":%d,\
		\"Medicine\":%d\
	},\"version\":\"1.0.0\"}",
	(float)temperature,
	led_1_status,
	med
	);

	//MQTT_PublishData(ALI_PUBLISH_TOPIC,mqtt_message,0);
	//MQTT_PublishData(EMQX_PUBLISH_TOPIC,mqtt_message,0);
	EMQX_Publish(EMQX_PUBLISH_HEART,mqtt_message);
}


//����MQTT�·�����Ϣ				������ר��
void deal_MQTT_message(uint8_t* buf,uint16_t len)
{
	uint8_t data[512];
	uint16_t data_len = len;
	for(int i=0;i<data_len;i++)
	{
		data[i] = buf[i];
		HAL_UART_Transmit(&huart2,&data[i],1,100);
	}
	ESP8266_Clear();
	//user_main_info("MQTT�յ���Ϣ,���ݳ���=%d \n",data_len);
	
	//�����Ƿ������ú��
	int i = GetSubStrPos((char*)data,"LED0");
	if( i>0 )
	{
		uint8_t ledr_status = data[i+6]-'0';
		HAL_GPIO_WritePin(LED0_GPIO_Port,LED0_Pin,GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_SET);
		if(ledr_status)
			HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_RESET);
		else
			HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_SET);
	}
	
	//�����Ƿ��������̵�
	i = GetSubStrPos((char*)data,"LED1");
	if( i>0 )
	{
		uint8_t ledr_status = data[i+6]-'0';
		HAL_GPIO_WritePin(LED0_GPIO_Port,LED0_Pin,GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_SET);
		if(ledr_status)
			HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_RESET);
		else
			HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_SET);
	}
	
	
	//�����Ƿ�����������
	i = GetSubStrPos((char*)data,"LED2");
	if( i>0 )
	{
		uint8_t ledr_status = data[i+6]-'0';
		HAL_GPIO_WritePin(LED0_GPIO_Port,LED0_Pin,GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_SET);
		HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_SET);
		if(ledr_status)
			HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_RESET);
		else
			HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_SET);
	}

}
