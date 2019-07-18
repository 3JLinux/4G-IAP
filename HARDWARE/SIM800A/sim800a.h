#ifndef SIM800A_H
#define SIM800A_H

#include "sys.h"
#include "usart.h"
//#include "shell.h"

#define SIM800A_PRINTF_LEVEL		8

extern void USART4_send(char *data,u8 num);
#define	sim800a_Send_Data USART4_send

u8* sim800a_rec_check(const char *pcTarget, const char* pcFindStr);
void sim800a_check_GPRS_state(void);
void sim800a_start_task(void);
void sim800a_bring_up_connection(void);
void sim800a_get_ip(void);
void sim800a_configure_server_settings(char* ip_addr,char* port);
void sim800a_send_data(char* data, u32 len);
void sim800a_UDP_close(void);
void sim800a_net_close(void);
int gprsDecodeFrame(u8 *pbuf, const u8 *pdata, u32 len);
#endif



