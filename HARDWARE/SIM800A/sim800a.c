#include "sim800a.h"
#include "string.h"
#include "delay.h"

#ifdef USE_SIM800A
static const char double_quote[] = "\"";
static const char comma[] = ",";

#define GPRS_TRAN	0xef
#define GPRS_DSC	0xea
#define GPRS_SRC	0x1a
#define GPRS_EB		0xeb
#define GPRS_DSC1B	0x1b
#define GPRS_ETX    0xe3
#define GPRS_DSC03  0x03

static int gprsCodeGetOut0xla(u8 *pbuf, const u8 *pdata, u16 len);

static void printf_char(u8* date,u32 len)
{
	u32 i;
	for(i=0;i<len;i++)
	{
		printf("%c",date[i]);
	}
	printf("\r\n");
}

/*****************************************************
函数原型： 		void sim800a_rec_check(void)
功能：			sim800a接收检测
输入：			const char *pcTarget
				const char* pcFindStr
返回：			0
*****************************************************/
u8* sim800a_rec_check(const char *pcTarget, const char* pcFindStr)
{
	char *strx = NULL;
	strx = (char *)strstr(pcTarget,pcFindStr);
	return (u8*)strx;
}


/*****************************************************
函数原型： 		void sim800a_send_cmd(const char *cmd)
功能：			sim800a发送命令
输入：			const u8 *cmd 命令
返回：			0
*****************************************************/
static void sim800a_send_cmd(const char *cmd,u8 len)
{
	char sim800a_send_buff[64] = {0};
	u8 sim800a_send_num;
	//static u8 sendFlag = 0;
	sim800a_send_num = len;//strlen((char *)cmd);
	memcpy(sim800a_send_buff,cmd,sim800a_send_num);
	sim800a_Send_Data(sim800a_send_buff,sim800a_send_num);
	sim800a_Send_Data("\r\n",strlen("\r\n"));
	//XPRINT(level_printf_char,SIM800A_PRINTF_LEVEL,"sim800a send>>",sim800a_send_buff,sim800a_send_num);
	printf_char((u8*)sim800a_send_buff,sim800a_send_num);
}

/*************************************************Attach Network*********************************************************/

/*****************************************************
函数原型： 		void sim800a_check_GPRS_state(void)
功能：			Check if the MS is connected to the GPRS network
输入：			无
返回：			无
*****************************************************/
void sim800a_check_GPRS_state(void)
{
	const char at_buf[] = "AT+CGATT?";
	u8 at_buf_len = strlen(at_buf);
	sim800a_send_cmd(at_buf,at_buf_len);
}


/*****************************************************
函数原型： 		void sim800a_start_task(void)
功能：			Start Task and Set APN, USER NAME, PASSWORD
输入：			无
返回：			无
*****************************************************/
void sim800a_start_task(void)
{
	const char at_buf[] = "AT+CSTT=\"CMNET\"";
	u8 at_buf_len = strlen(at_buf);
	sim800a_send_cmd(at_buf,at_buf_len);
}


/*****************************************************
函数原型： 		void sim800a_bring_up_connection(void)
功能：			Bring Up Wireless Connection with GPRS or CSD
输入：			无
返回：			无
*****************************************************/
void sim800a_bring_up_connection(void)
{
	const char at_buf[] = "AT+CIICR";
	u8 at_buf_len = strlen(at_buf);
	sim800a_send_cmd(at_buf,at_buf_len);
}


/*****************************************************
函数原型： 		void sim800a_get_ip(void)
功能：			Get Local IP Address
输入：			无
返回：			无
*****************************************************/
void sim800a_get_ip(void)
{
	const char at_buf[] = "AT+CIFSR";
	u8 at_buf_len = strlen(at_buf);
	sim800a_send_cmd(at_buf,at_buf_len);
}


/*****************************************************
函数原型： 		void sim800a_configure_server_settings(char* ip_addr,char* port)
功能：			Configure UDP Server Settings
输入：			char* ip_addr ip地址
				char* port 端口
返回：			无
*****************************************************/
void sim800a_configure_server_settings(char* ip_addr,char* port)
{
	char send_cmd_buffer[30] = {0};
	const char configure_server[] = "AT+CIPSTART=\"UDP\",\"";
	memcpy(send_cmd_buffer,configure_server,strlen(configure_server));
	strcat(send_cmd_buffer,ip_addr);
	strcat(send_cmd_buffer,double_quote);
	strcat(send_cmd_buffer,comma);
	strcat(send_cmd_buffer,double_quote);
	strcat(send_cmd_buffer,port);
	strcat(send_cmd_buffer,double_quote);
	sim800a_send_cmd(send_cmd_buffer,strlen(send_cmd_buffer));
}



/*****************************************************
函数原型： 		void sim800a_get_ip(void)
功能：			Send Data Through TCP or UDP Connection 
输入：			无
返回：			无
*****************************************************/
void sim800a_send_data(char* data, u32 len)
{
	u8 sim800a_send_buf[1500] = {0};
	const char send_smd_end = 0x1a;
	u32 i;
	const char at_buf[] = "AT+CIPSEND=0,,\"139.159.220.138\",4566";
	u8 at_buf_len = strlen(at_buf);
	u8 send_buf_len = 0;
	for(i = 0;i < 1500;i++)
	{
		sim800a_send_buf[i] = 0;
	}
	sim800a_send_cmd(at_buf,at_buf_len);
	delay_ms(2000);
	send_buf_len = gprsCodeGetOut0xla(sim800a_send_buf,(u8*)data,len);
	sim800a_Send_Data((char*)sim800a_send_buf,send_buf_len);
	sim800a_send_cmd(&send_smd_end,1);
}



/*****************************************************
函数原型： 		void sim800a_UDP_close(void)
功能：			Close TCP or UDP Connection
输入：			无
返回：			无
*****************************************************/
void sim800a_UDP_close(void)
{
	const char at_buf[] = "AT+CIPCLOSE=1";
	u8 at_buf_len = strlen(at_buf);
	sim800a_send_cmd(at_buf,at_buf_len);
}



/*****************************************************
函数原型： 		void sim800a_net_close(void)
功能：			DEACTIVATE GPRS PDP CONTEXT
输入：			无
返回：			无
*****************************************************/
void sim800a_net_close(void)
{
	const char at_buf[] = "AT+CIPSHUT=1";
	u8 at_buf_len = strlen(at_buf);
	sim800a_send_cmd(at_buf,at_buf_len);
}


//#define GPRS_TRAN		0xef
//#define GPRS_DSC		0xea
//#define GPRS_SRC		0x1a
//#define GPRS_EB			0xeb
//#define GPRS_DSC1B	0x1b
//#define GPRS_ETX    0xe3
//#define GPRS_DSC03  0x03
int gprsCodeGetOut0xla(u8 *pbuf, const u8 *pdata, u16 len)
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

int gprsDecodeFrame(u8 *pbuf, const u8 *pdata, u32 len)
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
}

#endif







