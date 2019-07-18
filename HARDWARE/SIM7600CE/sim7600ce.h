#ifndef SIM7600CE_H
#define SIM7600CE_H

#include "sys.h"
#include "usart.h"
//#include "shell.h"

enum OPERATOR{
	China_Mobile,		//中国移动
	China_Unicom,		//中国联通
	China_Telecom 	//中国电信
};

enum NET_TYPE{
	UDP,
	TCP
};



#define SIM7600CE_PRINTF_LEVEL		2			//打印等级

extern void USART4_send(char *data,u8 num); //模块使用的串口
#define	sim7600ce_Send_Data USART4_send

extern enum NET_TYPE type;
extern enum OPERATOR Operator;

u8* sim7600ce_rec_check(const char *pcTarget, const char* pcFindStr);
void sim7600ce_check_cmd(void);
void sim7600ce_request_PIN(void);
void sim7600ce_query_singal_quality(void);
void sim7600ce_query_net_status(void);
void sim7600ce_query_version(void);
void sim7600ce_query_preferred_mode(void);
void sim7600ce_inquiring_ue_system(void);
void sim7600ce_select_operator(enum OPERATOR Operator);
void sim7600ce_set_active_PDP(void);
void sim7600ce_select_app_mode(void);
void sim7600ce_open_socket(void);
void sim7600ce_inquire_ip(void);
void sim7600ce_configure_server_settings(enum NET_TYPE type,char* ip_addr,char* port);
void sim7600ce_send_data_cmd(char* ip_addr,char* port);
void sim7600ce_send_data(char* data, u32 len);


#endif
