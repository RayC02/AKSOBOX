#ifndef __IOT_ONENET_H
#define __IOT_ONENET_H

#include "main.h"
#include "stdio.h"
#define BYTE0(dwTemp)       (*( char *)(&dwTemp))
#define BYTE1(dwTemp)       (*((char *)(&dwTemp) + 1))
#define BYTE2(dwTemp)       (*((char *)(&dwTemp) + 2))
#define BYTE3(dwTemp)       (*((char *)(&dwTemp) + 3))

extern u8 esp8266_txbuf[1024]; 
//MQTT���ӷ�����
extern uint8_t MQTT_Connect(char *ClientID,char *Username,char *Password);
//MQTT��Ϣ����
extern uint8_t MQTT_SubscribeTopic(char *topic,uint8_t qos,uint8_t whether);
//MQTT��Ϣ����
extern uint8_t MQTT_PublishData(char *topic, char *message, uint8_t qos);
//MQTT����������
extern void MQTT_SentHeart(void);

#endif
