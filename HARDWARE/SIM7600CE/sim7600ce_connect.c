#include "sim7600ce_connect.h"
#include "delay.h"
#include "string.h"
#include "sysprintf.h"

FIFO_T ringgprsbuf;
uint8_t gprs_buf[GPRS_BUF_LEN];

#define sim7600ce_rx_buffer_clean	usart4_rx_buffer_clean
#define sim7600ce_rx_buffer_get	usart4_rx_buffer_get

static const char UDP_local_port[] = "4566";			//端口 
static const char UDP_ipv4_addr[] = "139.159.220.138";	//测试服务器

#define CMD_SEND_MAX_TIME						3			//每条AT循环发送次数
#define AT_ONE_DIRECTIVE_DELAY_TIME 			500		//发送完AT等待回复的时间（ms）
#define NET_CONNECT_ERR							1
#define NET_CONNECT_SUCCESS						0

extern enum NET_TYPE type;
extern enum OPERATOR Operator;

static uint8_t* check_string(const uint8_t* src,uint32_t src_len,const uint8_t* check_key,uint32_t check_num);
static uint8_t* get_gprs_num(const uint8_t* src,uint32_t* rec_num);

u8 net_connect(void)
{
	u8 i,j;
	u8* presp = NULL;
	u32 len = 0;
	u8 buffer[30] = {0};
	static u8 open_flag = 0;
	for(i=0;i<CMD_SEND_MAX_TIME;i++)
	{
		//AT
		sim7600ce_check_cmd();
		delay_ms(AT_ONE_DIRECTIVE_DELAY_TIME);
		presp = sim7600ce_rec_check((char*)(sim7600ce_rx_buffer_get(&len)),"OK");		//成功时返回OK
		sim7600ce_rx_buffer_clean();
		if(presp!=NULL)
		{
			break;
		}
		else if(i>=(CMD_SEND_MAX_TIME - 1))
		{
			return NET_CONNECT_ERR;
		}
	}
	for(i=0;i<CMD_SEND_MAX_TIME;i++)
	{
		//AT+CPIN?
		sim7600ce_request_PIN();
		delay_ms(AT_ONE_DIRECTIVE_DELAY_TIME);
		presp = sim7600ce_rec_check((char*)(sim7600ce_rx_buffer_get(&len)),"READY");	//成功时返回+CPIN: READY
		sim7600ce_rx_buffer_clean();
		if(presp!=NULL)
		{
			break;
		}
		else if(i>=(CMD_SEND_MAX_TIME - 1))
		{
			return NET_CONNECT_ERR;
		}
	}
	for(i=0;i<CMD_SEND_MAX_TIME;i++)
	{
		//AT+CSQ
		sim7600ce_query_singal_quality();
		delay_ms(AT_ONE_DIRECTIVE_DELAY_TIME);
		presp = sim7600ce_rec_check((char*)(sim7600ce_rx_buffer_get(&len)),"+CSQ:");
		sim7600ce_rx_buffer_clean();
		if(presp!=NULL)
		{
			break;
		}
		else if(i>=(CMD_SEND_MAX_TIME - 1))
		{
			return NET_CONNECT_ERR;
		}
	}
	for(i=0;i<CMD_SEND_MAX_TIME;i++)
	{
		//AT+CGREG?
		sim7600ce_query_net_status();
		delay_ms(AT_ONE_DIRECTIVE_DELAY_TIME);
		presp = sim7600ce_rec_check((char*)(sim7600ce_rx_buffer_get(&len)),"+CGREG: 0,1");
		sim7600ce_rx_buffer_clean();
		if(presp!=NULL)
		{
			break;
		}
		else if(i>=(CMD_SEND_MAX_TIME - 1))
		{
			return NET_CONNECT_ERR;
		}
	}
	//AT+SIMCOMATI
	sim7600ce_query_version();
	delay_ms(AT_ONE_DIRECTIVE_DELAY_TIME);
	presp = sim7600ce_rec_check((char*)(sim7600ce_rx_buffer_get(&len)),"OK");
	sim7600ce_rx_buffer_clean();
	for(i=0;i<CMD_SEND_MAX_TIME;i++)
	{
		//AT+CNMP?
		sim7600ce_query_preferred_mode();
		delay_ms(AT_ONE_DIRECTIVE_DELAY_TIME);
		presp = sim7600ce_rec_check((char*)(sim7600ce_rx_buffer_get(&len)),"+CNMP");
		sim7600ce_rx_buffer_clean();
		if(presp!=NULL)
		{
			break;
		}
		else if(i>=(CMD_SEND_MAX_TIME - 1))
		{
			return NET_CONNECT_ERR;
		}
	}
	for(i=0;i<CMD_SEND_MAX_TIME;i++)
	{
		//AT+CPSI?
		sim7600ce_inquiring_ue_system();
		delay_ms(AT_ONE_DIRECTIVE_DELAY_TIME);
		presp = sim7600ce_rec_check((char*)(sim7600ce_rx_buffer_get(&len)),"+CPSI");
		sim7600ce_rx_buffer_clean();
		if(presp!=NULL)
		{
			break;
		}
		else if(i>=(CMD_SEND_MAX_TIME - 1))
		{
			return NET_CONNECT_ERR;
		}
	}
	for(i=0;i<CMD_SEND_MAX_TIME;i++)
	{
		Operator = China_Mobile;
		//AT+CGSOCKCONT
		sim7600ce_select_operator(Operator);
		delay_ms(AT_ONE_DIRECTIVE_DELAY_TIME);
		presp = sim7600ce_rec_check((char*)(sim7600ce_rx_buffer_get(&len)),"OK");
		sim7600ce_rx_buffer_clean();
		if(presp!=NULL)
		{
			break;
		}
		else if(i>=(CMD_SEND_MAX_TIME - 1))
		{
			return NET_CONNECT_ERR;
		}
	}
	for(i=0;i<CMD_SEND_MAX_TIME;i++)
	{
		//AT+CSOCKSETPN=1
		sim7600ce_set_active_PDP();
		delay_ms(AT_ONE_DIRECTIVE_DELAY_TIME);
		presp = sim7600ce_rec_check((char*)(sim7600ce_rx_buffer_get(&len)),"OK");
		sim7600ce_rx_buffer_clean();
		if(presp!=NULL)
		{
			break;
		}
		else if(i>=(CMD_SEND_MAX_TIME - 1))
		{
			return NET_CONNECT_ERR;
		}
	}
	for(i=0;i<CMD_SEND_MAX_TIME;i++)
	{
		//AT+CIPMODE=0
		sim7600ce_select_app_mode();
		delay_ms(AT_ONE_DIRECTIVE_DELAY_TIME);
		presp = sim7600ce_rec_check((char*)(sim7600ce_rx_buffer_get(&len)),"OK");
		sim7600ce_rx_buffer_clean();
		if(presp!=NULL)
		{
			break;
		}
		else if(i>=(CMD_SEND_MAX_TIME - 1))
		{
			return NET_CONNECT_ERR;
		}
	}
	for(i=0;i<CMD_SEND_MAX_TIME;i++)
	{
		//AT+NETOPEN
		sim7600ce_open_socket();
		delay_ms(AT_ONE_DIRECTIVE_DELAY_TIME);
		memcpy(buffer,sim7600ce_rx_buffer_get(&len),len);
		for(j=0;j<len;j++)
		{
			if(buffer[j] == '+')
			{
				if(memcmp((buffer+j),"+NETOPEN: 0",strlen("+NETOPEN: 0")) == 0)
				{
					open_flag = 1;
					break;
				}
			}
		}
		sim7600ce_rx_buffer_clean();
		if(open_flag)
		{
			open_flag = 0;
			break;
		}
		else if(i>=(CMD_SEND_MAX_TIME - 1))
		{
			return NET_CONNECT_ERR;
		}
	}
	for(i=0;i<CMD_SEND_MAX_TIME;i++)
	{
		//AT+CSQ
		sim7600ce_query_singal_quality();
		delay_ms(AT_ONE_DIRECTIVE_DELAY_TIME);
		presp = sim7600ce_rec_check((char*)(sim7600ce_rx_buffer_get(&len)),"+CSQ:");
		sim7600ce_rx_buffer_clean();
		if(presp!=NULL)
		{
			break;
		}
		else if(i>=(CMD_SEND_MAX_TIME - 1))
		{
			return NET_CONNECT_ERR;
		}
	}
	for(i=0;i<CMD_SEND_MAX_TIME;i++)
	{
		//AT+IPADDR
		sim7600ce_inquire_ip();
		delay_ms(AT_ONE_DIRECTIVE_DELAY_TIME);
		presp = sim7600ce_rec_check((char*)(sim7600ce_rx_buffer_get(&len)),"+IPADDR");
		sim7600ce_rx_buffer_clean();
		if(presp!=NULL)
		{
			break;
		}
		else if(i>=(CMD_SEND_MAX_TIME - 1))
		{
			return NET_CONNECT_ERR;
		}
	}
	for(i=0;i<CMD_SEND_MAX_TIME;i++)
	{
		type = UDP;
		//AT+CIPOPEN=
		sim7600ce_configure_server_settings(type,(char*)UDP_ipv4_addr,(char*)UDP_local_port);
		delay_ms(AT_ONE_DIRECTIVE_DELAY_TIME);
		memcpy(buffer,sim7600ce_rx_buffer_get(&len),len);
		for(j=0;j<len;j++)
		{
			if(buffer[j] == '+')
			{
				if(memcmp((buffer+j),"+CIPOPEN: 0,0",strlen("+CIPOPEN: 0,0")) == 0)
				{
					open_flag = 1;
					break;
				}
			}
		}
		sim7600ce_rx_buffer_clean();
		if(open_flag)
		{
			open_flag = 0;
			break;
		}
		else if(i>=(CMD_SEND_MAX_TIME - 1))
		{
			return NET_CONNECT_ERR;
		}
		
	}
	return NET_CONNECT_SUCCESS;
}

u8 net_send(char* data,u32 length)
{
	u32 i,j;
	u8* presp = NULL;
	u8* check_pointer = NULL;
	u8* rec_data = NULL;
	u32 rec_num = 0;
	u32 len = 0;
	static u8 buffer[1024] = {0};
	static u8 send_ready_flag = 0;
	//static u8 rec_data[100] = {0};
	static u8 create_buf_flag = 0;
	uint8_t net_status = NET_CONNECT_ERR;
	uint32_t offset = 0;
	if(!create_buf_flag)
	{
		fifo_Create(&ringgprsbuf,gprs_buf,sizeof(gprs_buf));
		create_buf_flag = 1;
	}
	for(i=0;i<CMD_SEND_MAX_TIME;i++)
	{
		//AT+CIPSEND
		sim7600ce_send_data_cmd((char*)UDP_ipv4_addr,(char*)UDP_local_port);
		delay_ms(AT_ONE_DIRECTIVE_DELAY_TIME);
		presp = sim7600ce_rec_check((char*)(sim7600ce_rx_buffer_get(&len)),"ERROR");
		//sim7600ce_rx_buffer_clean();
		
		if(presp==NULL && len>0)
		{
			//sim7600ce_rx_buffer_clean();
			//break;
			//while(1)
			//{
				for(j=0;j<1024;j++)
				{
					buffer[j] = 0;
				}
				if(sim7600ce_rx_buffer_get(&len) != NULL)
				{
					memcpy(buffer,sim7600ce_rx_buffer_get(&len),len);
					
					//////////////////////////////////////////////////
					/*服务器主动下发时候在这里接收到数据时的处理操作*/
					/*
					check_pointer = check_string(buffer+offset,len,(const uint8_t*)"+IPD",strlen("+IPD"));
					if(check_pointer != NULL)
					{
						rec_data = get_gprs_num(check_pointer,&rec_num);
						fifo_Enqueue(&ringgprsbuf,rec_data,rec_num);
						offset = check_pointer - buffer + 1;
						check_pointer = NULL;
						rec_data = NULL;
					}
					*/
					/////////////////////////////////////////////////
					for(j=0;j<len;j++)
					{
						if(buffer[j] == '>')
						{
							send_ready_flag = 1;
							break;
						}
					}
					if(send_ready_flag)
					{
						send_ready_flag = 0;
						break;
					}
					else
					{
						//delay_ms(AT_ONE_DIRECTIVE_DELAY_TIME);
					}
				}
			//}
			offset = 0;
			printf("rec_num1=%d\r\n",rec_num);
			sim7600ce_rx_buffer_clean();
			
		}
		else
		{
			sim7600ce_rx_buffer_clean();			
			if(i>=(CMD_SEND_MAX_TIME - 1))
			{
				return NET_CONNECT_ERR;
			}
		}
	}
	sim7600ce_rx_buffer_clean();
	net_status = NET_CONNECT_ERR;
	sim7600ce_send_data(data,length);
	for(i=0;i<2;i++)
	{
		delay_ms(1000);
	}
	//presp = sim7600ce_rec_check((char*)(sim7600ce_rx_buffer_get(&len)),"OK");
	presp = sim7600ce_rx_buffer_get(&len);
	//printf("sim7600ce_rx_buffer_get_len=%d\r\n",len);
	//MEM_DUMP(SIM7600CE_PRINTF_LEVEL,"buffer<-",presp,len);
	for(i=0;i<len;i++)
	{
		if(presp[i] == 'O' && presp[i+1] == 'K')
		{
			net_status = NET_CONNECT_SUCCESS;
		}
		//if(net_status == NET_CONNECT_SUCCESS)
		{
			check_pointer = check_string(presp+offset,len,(const uint8_t*)"+IPD",strlen("+IPD"));
			if(check_pointer != NULL)
			{
				rec_data = get_gprs_num(check_pointer,&rec_num);
				fifo_Enqueue(&ringgprsbuf,rec_data,rec_num);
				//printf("fifo->rear=%d\r\n",ringgprsbuf.rear);
				offset = check_pointer - presp + 1;
				check_pointer = NULL;
				rec_data = NULL;
				//printf("rec_num2=%d\r\n",rec_num);
			}
		}
	}
	offset = 0;
	rec_num = 0;
	sim7600ce_rx_buffer_clean();
	return net_status;
}


void GPRS_init(void)
{
	GPRS_PIN_init();
}

void GPRS_reset(void)
{
	GPRS_SET_RESET_PIN();
	delay_ms(500);
	GPRS_RESET_RESET_PIN();
}

//寻找字符串
static uint8_t* check_string(const uint8_t* src,uint32_t src_len,const uint8_t* check_key,uint32_t check_num)
{
	uint32_t i,j;
	for(i=0;i<src_len - check_num;i++)
	{
		if(src[i] == check_key[0])
		{
			for(j=1;j<check_num;j++)
			{
				if(src[i+j] != check_key[j])
				{
					break;
				}
				else if(j == check_num-1)
				{
					return (uint8_t*)(src+i);
				}
			}
		}
	}
	return NULL;
}


static uint8_t* get_gprs_num(const uint8_t* src,uint32_t* rec_num)
{
	uint32_t i;
	uint32_t num = 0;
	uint32_t len = 0;
	uint8_t* rec_data = NULL;
	for(i=0;i<10;i++)
	{
		if(src[4+i] == 0x0D && src[4+i+1] == 0x0A)
		{
			num = i;
			break;
		}
	}
	if(num == 0)
	{
		rec_data = NULL;
		return 0;
	}
	for(i=0;i<num;i++)
	{
		len *= 10;
		len += (src[4+i] - 0x30);
	}
	rec_data = (uint8_t*)(src + 4 + num);
	*rec_num = len;
	return (rec_data + 2);
}


void GPRS_PIN_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPRS_PWRKEY_PIN|GPRS_SRST_PIN|GPRS_STATUS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPRS_CONTROL_PIN_PORT,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPRS_POWER_EN_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPRS_POWER_EN_PORT, &GPIO_InitStructure);
	
	GPRS_EN(1);
	GPRS_PWRKEY(0);
	GPRS_SRST(0);
}	

void GPRS_SET_RESET_PIN(void)
{
	GPRS_EN(0);
}

void GPRS_RESET_RESET_PIN(void)
{
	GPRS_EN(1);
}



