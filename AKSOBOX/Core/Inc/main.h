/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void enableKey(void);
void disableKey(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define KEY3_Pin GPIO_PIN_13
#define KEY3_GPIO_Port GPIOC
#define KEY3_EXTI_IRQn EXTI15_10_IRQn
#define AHT10_SDA_Pin GPIO_PIN_1
#define AHT10_SDA_GPIO_Port GPIOC
#define BEEP_Pin GPIO_PIN_2
#define BEEP_GPIO_Port GPIOB
#define LED0_Pin GPIO_PIN_7
#define LED0_GPIO_Port GPIOE
#define LED1_Pin GPIO_PIN_8
#define LED1_GPIO_Port GPIOE
#define LED2_Pin GPIO_PIN_9
#define LED2_GPIO_Port GPIOE
#define COUNT_Pin GPIO_PIN_12
#define COUNT_GPIO_Port GPIOB
#define KEY2_Pin GPIO_PIN_8
#define KEY2_GPIO_Port GPIOD
#define KEY2_EXTI_IRQn EXTI9_5_IRQn
#define KEY1_Pin GPIO_PIN_9
#define KEY1_GPIO_Port GPIOD
#define KEY1_EXTI_IRQn EXTI9_5_IRQn
#define KEY0_Pin GPIO_PIN_10
#define KEY0_GPIO_Port GPIOD
#define KEY0_EXTI_IRQn EXTI15_10_IRQn
#define DS1302_DAT_Pin GPIO_PIN_6
#define DS1302_DAT_GPIO_Port GPIOC
#define DS1302_CLK_Pin GPIO_PIN_8
#define DS1302_CLK_GPIO_Port GPIOA
#define AHT10_SCL_Pin GPIO_PIN_6
#define AHT10_SCL_GPIO_Port GPIOD
#define LCD_CS_Pin GPIO_PIN_7
#define LCD_CS_GPIO_Port GPIOD
#define LCD_DC_Pin GPIO_PIN_4
#define LCD_DC_GPIO_Port GPIOB
#define LCD_RST_Pin GPIO_PIN_6
#define LCD_RST_GPIO_Port GPIOB
#define LCD_PWR_Pin GPIO_PIN_7
#define LCD_PWR_GPIO_Port GPIOB
#define DS1302_CE_Pin GPIO_PIN_9
#define DS1302_CE_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
#define BEEP(n)	(n?HAL_GPIO_WritePin(BEEP_GPIO_Port,BEEP_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(BEEP_GPIO_Port,BEEP_Pin,GPIO_PIN_RESET))
#define BEEP_TogglePin	HAL_GPIO_TogglePin(BEEP_GPIO_Port,BEEP_Pin)
#define LED1(n)	(n?HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_RESET):HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,GPIO_PIN_SET))
#define LED2(n)	(n?HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_RESET):HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,GPIO_PIN_SET))
#define LED0(n)	(n?HAL_GPIO_WritePin(LED0_GPIO_Port,LED0_Pin,GPIO_PIN_RESET):HAL_GPIO_WritePin(LED0_GPIO_Port,LED0_Pin,GPIO_PIN_SET))
#define LED1_TogglePin	HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin)
#define LED2_TogglePin	HAL_GPIO_TogglePin(LED2_GPIO_Port,LED2_Pin)
#define LED0_TogglePin	HAL_GPIO_TogglePin(LED0_GPIO_Port,LED0_Pin)

#define SIZE sizeof(TEXT_Buffer)
#define FLASH_Buf_Address		0x00803000			//测试数据存储地址

//此处是阿里云服务器的登陆配置
#define ALI_BROKERADDRESS "iot-06z00ftuy7gr45l.mqtt.iothub.aliyuncs.com"
#define ALI_CLIENTID "i1z2R3abMri.devicet_test1|securemode=2,signmethod=hmacsha256,timestamp=1670772273739|"
#define ALI_USARNAME "devicet_test1&i1z2R3abMri"
#define ALI_PASSWD "c4be682747fa7e5b94e54dc66d484502e8191fc18159114999629c2257a38777"
#define	ALI_PUBLISH_TOPIC "/sys/i1z2R3abMri/devicet_test1/thing/event/property/post"
#define ALI_SUBSCRIBE_TOPIC "/sys/i1z2R3abMri/devicet_test1/thing/event/property/post_reply"

//此处是EMQX服务器的登陆配置
#define EMQX_BROKERADDRESS "122.9.7.191"
#define EMQX_USARNAME "RayC02"
#define EMQX_CLIENTID "device1"
#define EMQX_PASSWD "587FF020E80FC95D1EBE0864A6E734D45F6D6D8E"
#define	EMQX_PUBLISH_HEART "heart/device1"
#define	EMQX_PUBLISH_MSG "message/device1"
#define	EMQX_PUBLISH_ERR "err/device1"
#define	EMQX_PUBLISH_PUBLIC "public/akso"

//定义一些常用的数据类型短关键字
typedef int32_t  s32;
typedef int16_t s16;
typedef int8_t  s8;

typedef const int32_t sc32;
typedef const int16_t sc16;
typedef const int8_t sc8;

typedef __IO int32_t  vs32;
typedef __IO int16_t  vs16;
typedef __IO int8_t   vs8;

typedef __I int32_t vsc32;
typedef __I int16_t vsc16;
typedef __I int8_t vsc8;

typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef const uint32_t uc32;
typedef const uint16_t uc16;
typedef const uint8_t uc8;

typedef __IO uint32_t  vu32;
typedef __IO uint16_t vu16;
typedef __IO uint8_t  vu8;

typedef __I uint32_t vuc32;
typedef __I uint16_t vuc16;
typedef __I uint8_t vuc8;

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
