#ifndef __IOT_EMQX_H
#define __IOT_EMQX_H

#include "main.h"


_Bool EMQX_DevLink(void);													//与EMQX平台建立连接
void EMQX_Subscribe(const char *topics[], unsigned char topic_cnt);	//订阅主题
void EMQX_Publish(const char *topic, const char *msg);					//发布主题
void EMQX_RevPro(unsigned char *cmd);									//将平台返回的数据进行拆包，获取想要的数据

void setClocktime(u8 hour,u8 minute,u16 freq);  //根据频率设置早中晚时间
#endif
