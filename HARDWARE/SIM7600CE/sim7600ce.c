#include "sim7600ce.h"
#include "string.h"
#include "delay.h"
#include "sysprintf.h"

static const char double_quote[] = "\"";
static const char comma[] = ",";


enum OPERATOR Operator;

enum NET_TYPE type;

#define GPRS_TRAN	0xef
#define GPRS_DSC	0xea
#define GPRS_SRC	0x1a
#define GPRS_EB		0xeb
#define GPRS_DSC1B	0x1b
#define GPRS_ETX    0xe3
#define GPRS_DSC03  0x03

static int gprsCodeGetOut0xla(u8 *pbuf, const u8 *pdata, u16 len);


/*****************************************************
函数原型： 		void sim7600ce_rec_check(void)
功能：			sim7600ce接收检测
输入：			const char *pcTarget
				const char* pcFindStr
返回：			0
*****************************************************/
u8* sim7600ce_rec_check(const char *pcTarget, const char* pcFindStr)
{
	char *strx = NULL;
	strx = (char *)strstr(pcTarget,pcFindStr);
	return (u8*)strx;
}


/*****************************************************
函数原型： 		void sim7600ce_send_cmd(const char *cmd)
功能：			sim7600ce发送命令
输入：			const u8 *cmd 命令
返回：			0
*****************************************************/
static void sim7600ce_send_cmd(const char *cmd,u8 len)
{
	static char sim7600ce_send_buff[64] = {0};
	u8 sim7600ce_send_num;
	//static u8 sendFlag = 0;
	sim7600ce_send_num = len;//strlen((char *)cmd);
	memcpy(sim7600ce_send_buff,cmd,sim7600ce_send_num);
	sim7600ce_Send_Data(sim7600ce_send_buff,sim7600ce_send_num);
	sim7600ce_Send_Data("\r\n",strlen("\r\n"));
	//printf("GPRS->");
	//printf_char((u8*)sim7600ce_send_buff,sim7600ce_send_num);
#ifdef USER_DEBUG
	MEM_DUMP(SIM7600CE_PRINTF_LEVEL,"GPRS->",sim7600ce_send_buff,sim7600ce_send_num);
#endif
}


/*****************************************************
函数原型： 		void sim7600ce_check_cmd(void)
功能：			Check if the AT command repeatedly
输入：			无
返回：			无
*****************************************************/
void sim7600ce_check_cmd(void)
{
	const char at_buf[] = "AT";
	u8 at_buf_len = strlen(at_buf);
	sim7600ce_send_cmd(at_buf,at_buf_len);
}


/*****************************************************
函数原型： 		void sim7600ce_request_PIN(void)
功能：			PIN request
输入：			无
返回：			无
*****************************************************/
void sim7600ce_request_PIN(void)
{
	const char at_buf[] = "AT+CPIN?";
	u8 at_buf_len = strlen(at_buf);
	sim7600ce_send_cmd(at_buf,at_buf_len);
}


/*****************************************************
函数原型： 		void sim7600ce_query_singal_quality(void)
功能：			Query singal quality
输入：			无
返回：			无
*****************************************************/
void sim7600ce_query_singal_quality(void)
{
	const char at_buf[] = "AT+CSQ";
	u8 at_buf_len = strlen(at_buf);
	sim7600ce_send_cmd(at_buf,at_buf_len);
}


/*****************************************************
函数原型： 		void sim7600ce_query_net_status(void)
功能：			Query GPRS network registration status
输入：			无
返回：			无
*****************************************************/
void sim7600ce_query_net_status(void)
{
	const char at_buf[] = "AT+CGREG?";
	u8 at_buf_len = strlen(at_buf);
	sim7600ce_send_cmd(at_buf,at_buf_len);
}


/*****************************************************
函数原型： 		void sim7600ce_query_version(void)
功能：			Query version number 
输入：			无
返回：			无
*****************************************************/
void sim7600ce_query_version(void)
{
	const char at_buf[] = "AT+SIMCOMATI";
	u8 at_buf_len = strlen(at_buf);
	sim7600ce_send_cmd(at_buf,at_buf_len);
}


/*****************************************************
函数原型： 		void sim7600ce_query_preferred_mode(void)
功能：			Query preferred mode selection 
输入：			无
返回：			无
*****************************************************/
void sim7600ce_query_preferred_mode(void)
{
	const char at_buf[] = "AT+CNMP?";
	u8 at_buf_len = strlen(at_buf);
	sim7600ce_send_cmd(at_buf,at_buf_len);
}


/*****************************************************
函数原型： 		void sim7600ce_inquiring_ue_system(void)
功能：			Inquiring UE system information
输入：			无
返回：			无
*****************************************************/
void sim7600ce_inquiring_ue_system(void)
{
	const char at_buf[] = "AT+CPSI?";
	u8 at_buf_len = strlen(at_buf);
	sim7600ce_send_cmd(at_buf,at_buf_len);
}


/*****************************************************
函数原型： 		void sim7600ce_select_operator(void)
功能：			
输入：			无
返回：			无
*****************************************************/
void sim7600ce_select_operator(enum OPERATOR Operator)
{
	char at_buf[] = "AT+CGSOCKCONT=1,\"IP\",\"CMNET\"";
	u8 at_buf_len = strlen(at_buf);
	switch (Operator)
	{
		case China_Mobile:memcpy(at_buf,"AT+CGSOCKCONT=1,\"IP\",\"CMNET\"",at_buf_len);break;
		case China_Unicom:memcpy(at_buf,"AT+CGSOCKCONT=1,\"IP\",\"3GNET\"",at_buf_len);break;
		case China_Telecom:memcpy(at_buf,"AT+CGSOCKCONT=1,\"IP\",\"CTNET\"",at_buf_len);break;
		default:break;
	}
	sim7600ce_send_cmd(at_buf,at_buf_len);
}


/*****************************************************
函数原型： 		void sim7600ce_set_active_PDP(void)
功能：			Set active PDP context's profile number
输入：			无
返回：			无
*****************************************************/
void sim7600ce_set_active_PDP(void)
{
	const char at_buf[] = "AT+CSOCKSETPN=1";
	u8 at_buf_len = strlen(at_buf);
	sim7600ce_send_cmd(at_buf,at_buf_len);
}


/*****************************************************
函数原型： 		void sim7600ce_select_app_mode(void)
功能：			Select TCP/IP application mode
输入：			无
返回：			无
*****************************************************/
void sim7600ce_select_app_mode(void)
{
	const char at_buf[] = "AT+CIPMODE=0";
	u8 at_buf_len = strlen(at_buf);
	sim7600ce_send_cmd(at_buf,at_buf_len);
}


/*****************************************************
函数原型： 		void sim7600ce_open_socket(void)
功能：			Open socket
输入：			无
返回：			无
*****************************************************/
void sim7600ce_open_socket(void)
{
	const char at_buf[] = "AT+NETOPEN";
	u8 at_buf_len = strlen(at_buf);
	sim7600ce_send_cmd(at_buf,at_buf_len);
}


/*****************************************************
函数原型： 		void sim7600ce_inquire_ip(void)
功能：			Inquire socket PDP address
输入：			无
返回：			无
*****************************************************/
void sim7600ce_inquire_ip(void)
{
	const char at_buf[] = "AT+IPADDR";
	u8 at_buf_len = strlen(at_buf);
	sim7600ce_send_cmd(at_buf,at_buf_len);
}


/*****************************************************
函数原型： 		void sim7600ce_configure_server_settings(char* type,char* ip_addr,char* port)
功能：			Establish connection in multi-socket mode
输入：			char* type UDP/TCP
						char* ip_addr ip地址
						char* port 端口
返回：			无
*****************************************************/
void sim7600ce_configure_server_settings(enum NET_TYPE type,char* ip_addr,char* port)
{
	char send_cmd_buffer[50] = {0};
	const char configure_server[] = "AT+CIPOPEN=0,\"";
	memcpy(send_cmd_buffer,configure_server,strlen(configure_server));
	switch((u8)type)
	{
		case UDP:
			strcat(send_cmd_buffer,"UDP");
			strcat(send_cmd_buffer,double_quote);
			strcat(send_cmd_buffer,comma);
			strcat(send_cmd_buffer,comma);
			strcat(send_cmd_buffer,comma);
			strcat(send_cmd_buffer,port);
			break;
		case TCP:
			strcat(send_cmd_buffer,"TCP");
			strcat(send_cmd_buffer,double_quote);
			strcat(send_cmd_buffer,comma);
			strcat(send_cmd_buffer,double_quote);
			strcat(send_cmd_buffer,ip_addr);
			strcat(send_cmd_buffer,double_quote);
			strcat(send_cmd_buffer,comma);
			strcat(send_cmd_buffer,port);
			break;
		default:break;
	}
	sim7600ce_send_cmd(send_cmd_buffer,strlen(send_cmd_buffer));
}


/*****************************************************
函数原型： 		void sim7600ce_send_data_cmd(char* ip_addr,char* port)
功能：			Send Data Through TCP or UDP Connection
输入：			char* ip_addr ip地址
						char* port 端口
返回：			无
*****************************************************/
void sim7600ce_send_data_cmd(char* ip_addr,char* port)
{
	u8 i;
	static char send_cmd_buffer[50] = {0};
	const char at_buf[] = "AT+CIPSEND=0,,\"";
	u8 at_buf_len = strlen(at_buf);
	for(i=0;i<50;i++)
	{
		send_cmd_buffer[i] = 0;
	}
	memcpy(send_cmd_buffer,at_buf,strlen(at_buf));
	strcat(send_cmd_buffer,ip_addr);
	strcat(send_cmd_buffer,double_quote);
	strcat(send_cmd_buffer,comma);
	strcat(send_cmd_buffer,port);
	sim7600ce_send_cmd(send_cmd_buffer,strlen(send_cmd_buffer));
}


/*****************************************************
函数原型： 		void sim7600ce_send_data(char* data, u32 len)
功能：			Send Data Through TCP or UDP Connection
输入：			
返回：			无
*****************************************************/
void sim7600ce_send_data(char* data, u32 len)
{
	static u8 sim7600ce_send_buf[200] = {0};
	const char send_smd_end = 0x1a;
	u8 i;
	u8 send_buf_len = 0;
	for(i = 0;i < 200;i++)
	{
		sim7600ce_send_buf[i] = 0;
	}
	send_buf_len = gprsCodeGetOut0xla(sim7600ce_send_buf,(u8*)data,len);
	sim7600ce_Send_Data((char*)sim7600ce_send_buf,send_buf_len);
	MEM_DUMP(SIM7600CE_PRINTF_LEVEL,"GPRS->",sim7600ce_send_buf,send_buf_len);
	sim7600ce_send_cmd(&send_smd_end,1);
}



/*****************************************************
函数原型： 		int gprsCodeGetOut0xla(u8 *pbuf, const u8 *pdata, u16 len)
功能：			特殊字符转义
输入：			
返回：			转义后的数据长度
*****************************************************/
//#define GPRS_TRAN		0xef
//#define GPRS_DSC		0xea
//#define GPRS_SRC		0x1a
//#define GPRS_EB		0xeb
//#define GPRS_DSC1B	0x1b
//#define GPRS_ETX    0xe3
//#define GPRS_DSC03  0x03
static int gprsCodeGetOut0xla(u8 *pbuf, const u8 *pdata, u16 len)
{
	int i = 0;
	int j = 0;

	for (i = 0; i < len; i++)
	{
		if (pdata[i] == GPRS_SRC)
		{
			pbuf[j++] = GPRS_TRAN;
			pbuf[j++] = GPRS_DSC;
		}
		else if (pdata[i] == GPRS_TRAN)
		{ 
			pbuf[j++] = GPRS_TRAN;
			pbuf[j++] = GPRS_TRAN;			
		}
		
		else if (pdata[i] == GPRS_DSC1B)
		{
			pbuf[j++] = GPRS_TRAN;
			pbuf[j++] = GPRS_EB;				
		}
		else if(pdata[i] == GPRS_DSC03)
		{
			pbuf[j++] = GPRS_TRAN;
			pbuf[j++] = GPRS_ETX;
		}
		else
		{
			pbuf[j++] = pdata[i];
		}
	}
	//XPRINTF((6, "CODE 0X1A\n"));
	return j;
}


/*****************************************************
函数原型： 		int gprsCodeGetOut0xla(u8 *pbuf, const u8 *pdata, u16 len)
功能：			特殊字符转义回原来字符
输入：			
返回：			数据长度
*****************************************************/
/*
static int gprsDecodeFrame(u8 *pbuf, const u8 *pdata, u32 len)
{
	u32 i = 0;
	u32 j = 0;	

	for (i = 0; i < len-1; i++)
	{
		if (pdata[i]==GPRS_TRAN && (pdata[i+1]==GPRS_DSC || pdata[i+1] == GPRS_TRAN || pdata[i+1] == GPRS_EB || pdata[i+1] == GPRS_ETX))
		{
			if (pdata[i+1] == GPRS_DSC)
			{
				pbuf[j++] = GPRS_SRC;
			}
			else if (pdata[i+1] == GPRS_TRAN)
			{
				pbuf[j++] = GPRS_TRAN;
			}
			else if (pdata[i+1] == GPRS_EB)
			{
				pbuf[j++] = GPRS_DSC1B;	
			}
			else if (pdata[i+1] == GPRS_ETX)
			{
				pbuf[j++] = GPRS_DSC03;	
			}
			i = i+1;
		}
		else
		{
			pbuf[j++] = pdata[i];
		}
	}
	pbuf[j++] = pdata[len-1];
	return j;
}*/

