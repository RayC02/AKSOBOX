#ifndef __IOT_EMQX_H
#define __IOT_EMQX_H

#include "main.h"


_Bool EMQX_DevLink(void);													//��EMQXƽ̨��������
void EMQX_Subscribe(const char *topics[], unsigned char topic_cnt);	//��������
void EMQX_Publish(const char *topic, const char *msg);					//��������
void EMQX_RevPro(unsigned char *cmd);									//��ƽ̨���ص����ݽ��в������ȡ��Ҫ������

void setClocktime(u8 hour,u8 minute,u16 freq);  //����Ƶ������������ʱ��
#endif
