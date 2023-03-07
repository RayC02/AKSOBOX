#include "iot_emqx.h"
#include "main.h"
#include "esp8266.h"
#include <stdio.h>
#include "string.h"
#include "stdlib.h"
#include "usart.h"
#include "cJSON.h"
#include "deel_mqtt.h"
#include "mqttkit.h"
#include "str.h"
#include "w25qxx.h"
#include "stmflash.h"

extern unsigned char esp8266_buf[1024];
extern u16 med;
extern char *medName;
extern u8 medAll;
extern u16 medNum;
extern u16 medFreq;
extern u16 medTimes;

extern u8 ct1_hour;
extern u8 ct1_minute;
extern u8 ct2_hour;
extern u8 ct2_minute;
extern u8 ct3_hour;
extern u8 ct3_minute;


char medNameOut[80] = { 0x00 };         
int memAddr=0;
int ret=0;
extern char mqtt_message[300];
char romMedAll[30];
u8 TEXT_Buffer[] = {"hello"};
int size_rom = SIZE;
_Bool EMQX_DevLink(void)
{
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					//协议包
	unsigned char *dataPtr;
	_Bool status = 1;
	
	printf( "EMQX_DevLink\r\n"
							"PROID: %s,	AUIF: %s,	DEVID:%s\r\n"
					, EMQX_USARNAME, EMQX_PASSWD, EMQX_CLIENTID);
	/*返回非0表示未能给mqttPacket协议包完全填充数据*/
	if(MQTT_PacketConnect(EMQX_USARNAME, EMQX_PASSWD, EMQX_CLIENTID, 256, 0, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0){
		ESP8266_SendBuf(mqttPacket._data, mqttPacket._len);			//将协议包中_data指针指向的数据通过串口上传平台
		dataPtr = ESP8266_GetIPD(250);									//等待平台响应
		if(dataPtr != NULL){
			if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK){
				switch(MQTT_UnPacketConnectAck(dataPtr)){
					case 0:printf( "Tips:	连接成功\r\n");status = 0;break;	
					case 1:printf( "WARN:	连接失败：协议错误\r\n");break;
					case 2:printf( "WARN:	连接失败：非法的clientid\r\n");break;
					case 3:printf( "WARN:	连接失败：服务器失败\r\n");break;
					case 4:printf( "WARN:	连接失败：用户名或密码错误\r\n");break;
					case 5:printf( "WARN:	连接失败：非法链接(比如token非法)\r\n");break;	
					default:printf( "ERR:	连接失败：未知错误\r\n");break;
				}
			}
		}
		MQTT_DeleteBuffer(&mqttPacket);								//删包
	}
	else
		printf( "WARN:	MQTT_PacketConnect Failed\r\n");
	return status;
}


//==========================================================
void EMQX_Subscribe(const char *topics[], unsigned char topic_cnt)
{
	
	unsigned char i = 0;
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							//协议包
	
	for(; i < topic_cnt; i++)
		printf( "Subscribe Topic: %s\r\n", topics[i]);
	/*返回非0表示未能给mqttPacket协议包完全填充数据*/
	if(MQTT_PacketSubscribe(MQTT_SUBSCRIBE_ID, MQTT_QOS_LEVEL0, topics, topic_cnt, &mqttPacket) == 0)
	{
		ESP8266_SendBuf(mqttPacket._data, mqttPacket._len);					//向平台发送订阅请求
		
		MQTT_DeleteBuffer(&mqttPacket);											//删包
	}

}

//==========================================================

void EMQX_Publish(const char *topic, const char *msg)
{

	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							//协议包
	
//	printf( "Publish Topic: %s, Msg: %s\r\n", topic, msg);
	/*返回非0表示未能给mqttPacket协议包完全填充数据*/
	if(MQTT_PacketPublish(MQTT_PUBLISH_ID, topic, msg, strlen(msg), MQTT_QOS_LEVEL0, 0, 1, &mqttPacket) == 0)
	{
		ESP8266_SendBuf(mqttPacket._data, mqttPacket._len);					//向平台发送订阅请求
		MQTT_DeleteBuffer(&mqttPacket);											//删包
	}

}


void EMQX_RevPro(unsigned char *cmd)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};								//协议包
	
	char *req_payload = NULL;
	char *cmdid_topic = NULL;
	
	unsigned short topic_len = 0;
	unsigned short req_len = 0;
	
	unsigned char type = 0;
	unsigned char qos = 0;
	static unsigned short pkt_id = 0;
	
	short result = 0;

	cJSON* json, *json_value, *json_Hour1,*json_Minute1
								,*json_Hour2,*json_Minute2,*json_Hour3,*json_Minute3
								,*json_medName ,*json_medAll , *json_medNum,*json_medTimes,*json_setTime;
	
	char* array[]={"led0","launch","setMed","setTime","hello","check","setMedAll"
									,"hour1","minute1"
									,"hour2","minute2"
									,"hour3","minute3"
									,"medName","medAll","medNum","medFreq", "medTimes","setTime"};//所订阅的主题即将要发送过来的数据的键
	int i=0;
	
	type = MQTT_UnPacketRecv(cmd);
	switch(type)
	{
		case MQTT_PKT_CMD:															//命令下发
			
			result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload, &req_len);	//解出topic和消息体
			if(result == 0)
			{
				printf( "cmdid: %s, req: %s, req_len: %d\r\n", cmdid_topic, req_payload, req_len);	
			}
		
		break;
			
		case MQTT_PKT_PUBLISH:														//接收的Publish消息
		
			result = MQTT_UnPacketPublish(cmd, &cmdid_topic, &topic_len, &req_payload, &req_len, &qos, &pkt_id);
			if(result == 0)
			{
				printf("topic: %s,  payload: %s, payload_len: %d\r\n",
																	cmdid_topic,  req_payload, req_len);
				json = cJSON_Parse(req_payload);

				if(!json)printf( "Error: before:[%s]\n", cJSON_GetErrorPtr());
				else{
					/*例如：获取json列表中键为array[i]的对象*/
					for(;i<20;i++){
						json_value = cJSON_GetObjectItem(json, array[i]);
						json_medName = cJSON_GetObjectItem(json, "medName");
						json_medAll =  cJSON_GetObjectItem(json, "medAll");
						json_medNum = cJSON_GetObjectItem(json, "medNum");
						json_medTimes = cJSON_GetObjectItem(json, "medTimes");
						json_setTime = cJSON_GetObjectItem(json, "setTime");
						json_Hour1 = cJSON_GetObjectItem(json,"hour1");
						json_Minute1 = cJSON_GetObjectItem(json,"minute1");
						json_Hour2 = cJSON_GetObjectItem(json,"hour2");
						json_Minute2 = cJSON_GetObjectItem(json,"minute2");
						json_Hour3 = cJSON_GetObjectItem(json,"hour3");
						json_Minute3 = cJSON_GetObjectItem(json,"minute3");
						if(json_value!=NULL)
							break;
					}
					if(strstr(json_value->string, "led0") != NULL){
						if(json_value->valueint){
							LED0(1);//开操作
							ret=1;
						}else{
							LED0(0);//关操作
							ret=1;
						}	
						sprintf(mqtt_message,"{\"SetState\": %d}",ret);
						EMQX_Publish(EMQX_PUBLISH_MSG,mqtt_message);
						ret=0;
					}
					else if(strstr(json_value->string, "launch") != NULL){
						//开始吐药函数 json_value->valueint
						
						ret=1;
						sprintf(mqtt_message,"{\"SetState\": %d}",ret);
						EMQX_Publish(EMQX_PUBLISH_MSG,mqtt_message);
						ret=0;
					}
					else if(strstr(json_value->string, "hello") != NULL){
						sprintf(mqtt_message,"{\"hello\": 1}");
						EMQX_Publish(EMQX_PUBLISH_MSG,mqtt_message);
					}
					else if(strstr(json_value->string, "setMedAll") != NULL){
						medAll=json_value->valueint;
						sprintf(TEXT_Buffer,"%d",medAll);
						W25QXX_Write((u8*)TEXT_Buffer, FLASH_Buf_Address, SIZE);
						ret=1;
						sprintf(mqtt_message,"{\"SetState\": %d}",ret);
						EMQX_Publish(EMQX_PUBLISH_MSG,mqtt_message);
						ret=0;
					}
					else if(strstr(json_value->string, "check") != NULL)
					{
						sprintf(mqtt_message,"{\"medicine\": %d}",medAll);
						EMQX_Publish(EMQX_PUBLISH_MSG,mqtt_message);
					}
					else if(strstr(json_value->string, "setMed") != NULL){
							if(strstr(json_medName->string,"medName")!=NULL)
							{
								medName = json_medName->valuestring;
								Utf_8ToGbk(medName,strlen(medName),medNameOut,&memAddr);
							}
							if(strstr(json_medAll->string,"medAll")!=NULL)
							{
								medAll = json_medAll->valueint;
								if(medAll>0)
									LED0(0);
								sprintf(TEXT_Buffer,"%d",medAll);
								W25QXX_Write((u8*)TEXT_Buffer, FLASH_Buf_Address, SIZE);
							}
							if(strstr(json_medNum->string,"medNum")!=NULL)
							{
								medNum = json_medNum->valueint;
							}
							
							if(strstr(json_medTimes->string,"medTimes")!=NULL)
							{
								medTimes = json_medTimes->valueint;
								if(medTimes==1)
								{
									int ct_hour1,ct_minute1;
									if(strstr(json_setTime->string, "setTime") != NULL)
										{
										if(strstr(json_setTime->valuestring,"clock")!=NULL)
										{
											if(strstr(json_Hour1->string,"hour1")!=NULL)
											{
												ct_hour1 = json_Hour1->valueint;
												ct1_hour = ct_hour1;
											}
											if(strstr(json_Minute1->string,"minute1")!=NULL)
											{
												ct_minute1 = json_Minute1->valueint;
												ct1_minute = ct_minute1;
												printf("Clock 1 is: %02d:%02d\r\n",ct_hour1,ct_minute1);
											}
											ret=1;
										}
									}

								}
								else if(medTimes==2)
								{
									int ct_hour1,ct_minute1,ct_hour2,ct_minute2;
									
									if(strstr(json_setTime->string, "setTime") != NULL)
										{
										if(strstr(json_setTime->valuestring,"clock")!=NULL)
										{
											if(strstr(json_Hour1->string,"hour1")!=NULL)
											{
												ct_hour1 = json_Hour1->valueint;
												ct1_hour = ct_hour1;
											}
											if(strstr(json_Minute1->string,"minute1")!=NULL)
											{
												ct_minute1 = json_Minute1->valueint;
												ct1_minute = ct_minute1;
												printf("Clock 1 is: %02d:%02d\r\n",ct_hour1,ct_minute1);
												ret=1;
											}
											if(strstr(json_Hour2->string,"hour2")!=NULL)
											{
												ct_hour2 = json_Hour2->valueint;
												ct2_hour = ct_hour2;
											}
											if(strstr(json_Minute2->string,"minute2")!=NULL)
											{
												ct_minute2 = json_Minute2->valueint;
												ct2_minute = ct_minute2;
												printf("Clock 2 is: %02d:%02d\r\n",ct_hour2,ct_minute2);
											}
											ret=1;
										}
										
									}
										
								}
								else if(medTimes==3)
								{
									int ct_hour1,ct_minute1,ct_hour2,ct_minute2,ct_hour3,ct_minute3;
									
									if(strstr(json_setTime->string, "setTime") != NULL)
										{
										if(strstr(json_setTime->valuestring,"clock")!=NULL)
										{
											if(strstr(json_Hour1->string,"hour1")!=NULL)
											{
												ct_hour1 = json_Hour1->valueint;
												ct1_hour = ct_hour1;
											}
											if(strstr(json_Minute1->string,"minute1")!=NULL)
											{
												ct_minute1 = json_Minute1->valueint;
												ct1_minute = ct_minute1;
												printf("Clock 1 is: %02d:%02d\r\n",ct_hour1,ct_minute1);
											}
											if(strstr(json_Hour2->string,"hour2")!=NULL)
											{
												ct_hour2 = json_Hour2->valueint;
												ct2_hour = ct_hour2;
											}
											if(strstr(json_Minute2->string,"minute2")!=NULL)
											{
												ct_minute2 = json_Minute2->valueint;
												ct2_minute = ct_minute2;
												printf("Clock 2 is: %02d:%02d\r\n",ct_hour2,ct_minute2);
											}
											if(strstr(json_Hour3->string,"hour3")!=NULL)
											{
												ct_hour3 = json_Hour3->valueint;
												ct3_hour = ct_hour3;
											}
											if(strstr(json_Minute3->string,"minute3")!=NULL)
											{
												ct_minute3 = json_Minute3->valueint;
												ct3_minute = ct_minute3;
												printf("Clock 3 is: %02d:%02d\r\n",ct_hour3,ct_minute3);
											}
											ret=1;
										}
									}
										
								}

							}
							sprintf(mqtt_message,"{\"SetState\":%d}",ret);
							EMQX_Publish(EMQX_PUBLISH_MSG,mqtt_message);
							ret=0;
							printf("新增药品%s,总数%d,单次服用%d颗,每日服用%d次。\r\n",medNameOut,medAll,medNum,medTimes);
					}
	
					if(json_value!=NULL){
						printf( "json_string = %s,json_int = %d\r\n", json_value->string,json_value->valueint);
					}	
				}
				cJSON_Delete(json);
			}
		
		break;
			
		case MQTT_PKT_PUBACK:														//发送Publish消息，平台回复的Ack
		
			if(MQTT_UnPacketPublishAck(cmd) == 0)
				printf( "Tips:	MQTT Publish Send OK\r\n");
			
		break;
			
		case MQTT_PKT_PUBREC:														//发送Publish消息，平台回复的Rec，设备需回复Rel消息
		
			if(MQTT_UnPacketPublishRec(cmd) == 0)
			{
				printf( "Tips:	Rev PublishRec\r\n");
				if(MQTT_PacketPublishRel(MQTT_PUBLISH_ID, &mqttPacket) == 0)
				{
					printf( "Tips:	Send PublishRel\r\n");
					ESP8266_SendBuf(mqttPacket._data, mqttPacket._len);
					MQTT_DeleteBuffer(&mqttPacket);
				}
			}
		
		break;
			
		case MQTT_PKT_PUBREL:														//收到Publish消息，设备回复Rec后，平台回复的Rel，设备需再回复Comp
			
			if(MQTT_UnPacketPublishRel(cmd, pkt_id) == 0)
			{
				printf( "Tips:	Rev PublishRel\r\n");
				if(MQTT_PacketPublishComp(MQTT_PUBLISH_ID, &mqttPacket) == 0)
				{
					printf( "Tips:	Send PublishComp\r\n");
					ESP8266_SendBuf(mqttPacket._data, mqttPacket._len);
					MQTT_DeleteBuffer(&mqttPacket);
				}
			}
		
		break;
		
		case MQTT_PKT_PUBCOMP:														//发送Publish消息，平台返回Rec，设备回复Rel，平台再返回的Comp
		
			if(MQTT_UnPacketPublishComp(cmd) == 0)
			{
				printf( "Tips:	Rev PublishComp\r\n");
			}
		
		break;
			
		case MQTT_PKT_SUBACK:														//发送Subscribe消息的Ack
		
			if(MQTT_UnPacketSubscribe(cmd) == 0)
				printf( "Tips:	MQTT Subscribe OK\r\n");
			else
				printf( "Tips:	MQTT Subscribe Err\r\n");
		
		break;
			
		case MQTT_PKT_UNSUBACK:														//发送UnSubscribe消息的Ack
		
			if(MQTT_UnPacketUnSubscribe(cmd) == 0)
				printf( "Tips:	MQTT UnSubscribe OK\r\n");
			else
				printf( "Tips:	MQTT UnSubscribe Err\r\n");
		
		break;
		
		default:
			result = -1;
		break;
	}
	
	ESP8266_Clear();									//清空缓存
	
	if(result == -1)
		return;

	if(type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
	{
		MQTT_FreeBuffer(cmdid_topic);
		MQTT_FreeBuffer(req_payload);
	}
}

