#ifndef __DEEL_MQTT_H_
#define __DEEL_MQTT_H_


#include "main.h"
#include "iot_aliyun.h"
#include "iot_emqx.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>
#include "stdlib.h"
#include "AHT10.h"
#include "lcd.h"

//这是阿里云的mqtt处理
void STM32AHT11_StatusReport(void);
void deal_MQTT_message(uint8_t* buf,uint16_t len);

#endif
