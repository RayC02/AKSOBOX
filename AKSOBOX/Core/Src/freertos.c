/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "iot_aliyun.h"
#include "iot_emqx.h"
#include "esp8266.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>
#include "stdlib.h"
#include "AHT10.h"
#include "lcd.h"
#include "w25qxx.h"
#include "stmflash.h"
#include "ds1302.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
u16 med=0;
char *medName;
extern u8 medAll;
u16 medNum=0;
u16 medFreq=0;
u16 medTimes=0;
char mqtt_heart[300];	//MQTT的上报消息缓存
extern uint8_t temperature;
extern uint8_t humidity;
char countMed[64];

extern u8 TEXT_Buffer[];
extern int size_rom;
u8 ct1_hour;
u8 ct1_minute;
u8 ct2_hour;
u8 ct2_minute;
u8 ct3_hour;
u8 ct3_minute;



/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId processInformatHandle;
osThreadId countMedHandle;
osThreadId StatusReportHandle;
osThreadId timeHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */


/* USER CODE END FunctionPrototypes */

void processInformation(void const * argument);
void countMedicine(void const * argument);
void STM32_StatusReport(void const * argument);
void showTime(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of processInformat */
  osThreadDef(processInformat, processInformation, osPriorityNormal, 0, 128);
  processInformatHandle = osThreadCreate(osThread(processInformat), NULL);

  /* definition and creation of countMed */
  osThreadDef(countMed, countMedicine, osPriorityIdle, 0, 128);
  countMedHandle = osThreadCreate(osThread(countMed), NULL);

  /* definition and creation of StatusReport */
  osThreadDef(StatusReport, STM32_StatusReport, osPriorityIdle, 0, 128);
  StatusReportHandle = osThreadCreate(osThread(StatusReport), NULL);

  /* definition and creation of time */
  osThreadDef(time, showTime, osPriorityIdle, 0, 128);
  timeHandle = osThreadCreate(osThread(time), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_processInformation */
/**
  * @brief  Function implementing the processInformat thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_processInformation */
void processInformation(void const * argument)
{
  /* USER CODE BEGIN processInformation */
  /* Infinite loop */
  for(;;)
  {
   if(esp8266_cnt)
		{
			printf("接收到数据为：");
			EMQX_RevPro(esp8266_buf);
			ESP8266_Clear();
		}
    osDelay(100);
  }
  /* USER CODE END processInformation */
}

/* USER CODE BEGIN Header_countMedicine */
/**
* @brief Function implementing the countMed thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_countMedicine */
void countMedicine(void const * argument)
{
  /* USER CODE BEGIN countMedicine */
  /* Infinite loop */
  for(;;)
  {
		LCD_ShowNum(20+110,170, medAll, 3, 32);
		if(medAll<=0)
		{
			LED0(1);
		}
		else if(medAll>0)
		{
			LED0(0);
		}
		while(HAL_GPIO_ReadPin(COUNT_GPIO_Port,COUNT_Pin)==1)
		{
			
			if(HAL_GPIO_ReadPin(COUNT_GPIO_Port,COUNT_Pin)==0)
			{
				osDelay(15);
				med++;
				sprintf(countMed,"Number of Medicine:%d \r\n",med);
				HAL_UART_Transmit(&huart1,(u8*)countMed,strlen(countMed),HAL_MAX_DELAY);
				
				if(medAll>0)
				{
					medAll--;
					if(med==medNum)
					{
						printf("stop\r\n");
						//停止吐药函数
						//失能吐药按键
						disableKey();
						EMQX_Publish(EMQX_PUBLISH_MSG,"{\"WorkState\":1}");
					}					
					sprintf(TEXT_Buffer,"%d",medAll);		
					W25QXX_Write((u8*)TEXT_Buffer, FLASH_Buf_Address, size_rom);
					
				}
				else if(medAll<=0)
				{
					medAll = 0;
					LED0(1);
					EMQX_Publish(EMQX_PUBLISH_ERR,"{\"Err\":\"Empty\"}");
				}
			}
		}
		
  }
  /* USER CODE END countMedicine */
}

/* USER CODE BEGIN Header_STM32_StatusReport */
/**
* @brief Function implementing the StatusReport thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_STM32_StatusReport */
void STM32_StatusReport(void const * argument)
{
  /* USER CODE BEGIN STM32_StatusReport */
  /* Infinite loop */
  for(;;)
  {
		
		//获取温湿度信息
    temperature = AHT10_Read_Temperature();
	
	//上报一次数据
	sprintf(mqtt_heart,
	"{\"Temperature\":%.1f,\
		\"MedicineCount\":%d}",
	(float)temperature,
		medAll
	);
	//MQTT_PublishData(ALI_PUBLISH_TOPIC,mqtt_message,0);
	EMQX_Publish(EMQX_PUBLISH_HEART,mqtt_heart);
	ESP8266_Clear();
		
  osDelay(59000);			//心跳59s发送一次
  }
  /* USER CODE END STM32_StatusReport */
}

/* USER CODE BEGIN Header_showTime */
/**
* @brief Function implementing the time thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_showTime */
void showTime(void const * argument)
{
  /* USER CODE BEGIN showTime */
	char curTime[30];
	int curHour;
	int curMinute;
	int curSecond;


  /* Infinite loop */
  for(;;)
  {
		ds1302_read_time();	
		printf("time %d%d%c%d%d%c%d%d  %d%d%c%d%d%c%d%d\r\n",data_time.label2[0],data_time.label2[1],data_time.label2[2],data_time.label2[3],data_time.label2[4],data_time.label2[5],data_time.label2[6],data_time.label2[7],data_time.label2[8],data_time.label2[9],data_time.label2[10],data_time.label2[11],data_time.label2[12],data_time.label2[13],data_time.label2[14],data_time.label2[15]);
		sprintf(curTime,"%d%d",data_time.label2[8],data_time.label2[9]);
		curHour = atoi(curTime);
		sprintf(curTime,"%d%d",data_time.label2[11],data_time.label2[12]);
		curMinute = atoi(curTime);
		sprintf(curTime,"%d%d",data_time.label2[14],data_time.label2[15]);
		curSecond = atoi(curTime);
		
		if(medTimes==1)
		{
			if(curHour==ct1_hour)
			{
				if(curMinute==ct1_minute)
				{
					printf("enable");
					enableKey();
				}
			}
		}
		if(medTimes==2)
		{
			if(curHour==ct1_hour)
			{
				if(curMinute==ct1_minute)
				{
					printf("enable");
					enableKey();
				}
			}
			if(curHour==ct2_hour)
			{
				if(curMinute==ct2_minute)
				{
					printf("enable");
					enableKey();
				}
			}
		}
		if(medTimes==3)
		{
			if(curHour==ct1_hour)
			{
				if(curMinute==ct1_minute)
				{
					printf("enable");
					enableKey();
				}
			}
			if(curHour==ct2_hour)
			{
				if(curMinute==ct2_minute)
				{
					printf("enable");
					enableKey();
				}
			}
			if(curHour==ct3_hour)
			{
				if(curMinute==ct3_minute)
				{
					printf("enable");
					enableKey();
				}
			}

		}
    osDelay(1000);
  }
  /* USER CODE END showTime */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
