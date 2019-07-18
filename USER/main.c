#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "stmflash.h"
#include "iap.h"
#include "sim800a.h"
#include <string.h>
#include "crc16.h"
#include "gprs_handle.h"
#include "sim7600ce_connect.h"
#include "sysprintf.h"
#include "fifo.h"

#define FLASH_APP1_MAX_ADDR				0x8035000
#define FLASH_APP2_MAX_ADDR				0x8060000

#define ATCMD_MAX_REPEAT_NUMS			3
#define NET_CONNRTION_ERR				0
#define NET_CONNRTION_SUCCEED			1
#define UDP_SEND_ERR					0
#define UDP_SEND_SUCCEED				2
#define CONFIGURE_ERR					0
#define CONFIGURE_SUCCEED				1
#define FALSE							0
#define TRUE							1

#ifdef USE_SIM800A
static u8 host_mac[4] = {0x20,0x18,0x99,0x99};
static void printf_string(u8* data,u32 len);
static void sim800a_task(void);


#define sim800a_rx_buffer_clean	usart4_rx_buffer_clean
#define sim800a_rx_buffer_get	usart4_rx_buffer_get

//static u8 no_update = 0;
static u8 update_count = 0;
#endif

//const char UDP_local_port[] = "4569";			//端口
//const char UDP_local_port[] = "4568";			//端口 
const char UDP_local_port[] = "4566";			//端口 
const char UDP_ipv4_addr[] = "139.159.220.138";	//测试服务器
//const char UDP_ipv4_addr[] = "139.199.58.208";	//万科
//const char UDP_ipv4_addr[] = "119.29.155.148";	//正式服务器


#define FLASH_ID_ADDR	(0x08035000)
#define FLASH_FLAG_ADDR (0x08000000 + 0x6F00)
static u16 FLASH1_UPDATE = 1;
static u16 FLASH2_UPDATE = 2;
static u16 flash_flag = 0;
static u8 update_max_count = 3;

int main(void)
{
#ifndef USE_SIM800A	
	u16 oldcount=0;	//老的串口接收数据值
	u16 applenth=0;	//接收到的app代码长度
	//u8 clearflag=0;
	static u8 firmware_update_flag = 0; //程序更新完成标准
	static u8 flash_update_finish_flag = 0;
	static u8 flash_update_time_count = 0;
#endif
	u8 i;
	u16 host_id_buf[20] = {0};
	uart_init(921600);	//串口初始化为256000
	delay_init();	   	 	//延时初始化 
	uart4_init(115200);
	xdev_out(dbg_send_char);
	GPRS_init();
	LED_Init();
	//LCD_Init();	
	//LED_Init();		  		//初始化与LED连接的硬件接口

 	//KEY_Init();				//按键初始化
	printf("正在检测固件更新!\r\n");
	//GPRS_EN = 0;
	STMFLASH_Read(FLASH_ID_ADDR,host_id_buf,10);
	delay_ms(1000);
	STMFLASH_Read(FLASH_FLAG_ADDR,&flash_flag,1);
	//GPRS_EN = 1;
	host_mac[0] = (host_id_buf[7]) & 0xff;
	host_mac[1] = (host_id_buf[7]>>8) & 0xff;
	host_mac[2] = (host_id_buf[8]) & 0xff;
	host_mac[3] = (host_id_buf[8]>>8) & 0xff;
	update_max_count = 3;
	for(i=0;i<10;i++)
	{
		delay_ms(1000);
	}
	while(1)
	{
#ifndef USE_SIM800A
		flash_update_time_count++;
	 	if(USART_RX_CNT)
		{
			flash_update_time_count = 0;
			if(oldcount==USART_RX_CNT)//新周期内,没有收到任何数据,认为本次数据接收完成.
			{
				applenth=USART_RX_CNT;
				oldcount=0;
				USART_RX_CNT=0;
				printf("用户程序接收完成!\r\n");
				printf("代码长度:%dBytes\r\n",applenth);
				flash_update_finish_flag = 1;
			}else oldcount=USART_RX_CNT;	
			if(flash_update_finish_flag)
			{
				//applenth = 0;
				flash_update_finish_flag = 0;
				printf("开始更新固件...\r\n");	
				//LCD_ShowString(60,210,200,16,16,"Copying APP2FLASH...");
 				if(((*(vu32*)(0X20001000+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
				{	
					//STMFLASH_Read(FLASH_FLAG_ADDR,&flash_flag,1);
					if(flash_flag != 1)
					{
						STMFLASH_Write(FLASH_FLAG_ADDR,&FLASH1_UPDATE,1);
						iap_write_appbin(FLASH_APP1_ADDR,USART_RX_BUF,applenth);//更新FLASH代码
					}
					else 
					{
						STMFLASH_Write(FLASH_FLAG_ADDR,&FLASH2_UPDATE,1);
						iap_write_appbin(FLASH_APP2_ADDR,USART_RX_BUF,applenth);//更新FLASH代码
					}
					delay_ms(100);
					//LCD_ShowString(60,210,200,16,16,"Copy APP Successed!!");
					printf("固件更新完成!\r\n");	
					firmware_update_flag = 1;
				}else 
				{
					//LCD_ShowString(60,210,200,16,16,"Illegal FLASH APP!  ");	   
					printf("非FLASH应用程序!\r\n");
				}
 			}
			else 
			{
				printf("正在更新的固件!\r\n");
				//LCD_ShowString(60,210,200,16,16,"No APP!");
			}
			if(firmware_update_flag)
			{
				firmware_update_flag = 0;
				if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
				{	
					//STMFLASH_Read(FLASH_FLAG_ADDR,&flash_flag,1);
					//if(flash_flag == 1)
					{
						printf("开始执行FLASH1用户代码!!\r\n"); 
						iap_load_app(FLASH_APP1_ADDR);//执行FLASH APP代码
					}
					/*
					else if(flash_flag == 2)
					{
						printf("开始执行FLASH2用户代码!!\r\n"); 
						iap_load_app(FLASH_APP2_ADDR);//执行FLASH APP代码
					}
					*/
				}else 
				{
					printf("非FLASH应用程序,无法执行!\r\n");
					//LCD_ShowString(60,210,200,16,16,"Illegal FLASH APP!");	   
				}
			}
		}
		delay_ms(500);
		if(flash_update_time_count>= 60)
		{
			flash_update_time_count = 0;
			STMFLASH_Read(FLASH_FLAG_ADDR,&flash_flag,1);
			if(flash_flag == 1)
			{
				printf("开始执行FLASH1用户代码!!\r\n"); 
				iap_load_app(FLASH_APP1_ADDR);//执行FLASH APP代码
			}
			else if(flash_flag == 2)
			{
				printf("开始执行FLASH2用户代码!!\r\n"); 
				iap_load_app(FLASH_APP2_ADDR);//执行FLASH APP代码
			}
			else
			{
				printf("无可执行FLASH用户代码!!\r\n"); 
			}
		}
#endif
//while(1)
{
		LED0 = !LED0;
		sim800a_task();
		delay_ms(500);
}
		//iap_load_app(FLASH_APP1_ADDR);//执行FLASH APP代码
		//iap_load_app(FLASH_APP2_ADDR);//执行FLASH APP代码
		/*
		if(no_update)
		{
			update_count++;
		}
		else
		{
			update_count = 0;
		}
		*/
		if(update_count>=update_max_count)
		{
			update_count = 0;
			STMFLASH_Read(FLASH_FLAG_ADDR,&flash_flag,1);
			for(i=0;i<10;i++)
			{
				delay_ms(1000);
			}
			if(flash_flag != FLASH2_UPDATE)
			{
				//printf_string((u8*)FLASH_APP1_ADDR,1000);
				printf("开始执行FLASH1用户代码!!\r\n"); 
				iap_load_app(FLASH_APP1_ADDR);//执行FLASH APP代码
			}
			else if(flash_flag ==FLASH2_UPDATE)
			{
				printf("开始执行FLASH2用户代码!!\r\n"); 
				iap_load_app(FLASH_APP2_ADDR);//执行FLASH APP代码
			}
		}
		
	}   	   
}




#ifdef USE_SIM800A
#define AT_ONE_DIRECTIVE_DELAY_TIME		500
#define QUERY_NETWORK_TIME				30
#define SECOND_DELAY					1000


static void printf_char(u8* date,u32 len)
{
	u32 i;
	for(i=0;i<len;i++)
	{
		printf("%c",date[i]);
	}
	printf("\r\n");
}

u8 sim800a_creat_network_connetion(void)
{
	u32 len = 0;
	u8* presp = NULL;
	u8* key_word = NULL;
	u8 i;
		//AT+CIPCLOSE 
		sim800a_UDP_close();
		delay_ms(AT_ONE_DIRECTIVE_DELAY_TIME);
		presp = sim800a_rec_check((char*)(sim800a_rx_buffer_get(&len)),"OK");
		key_word = sim800a_rec_check((char*)(sim800a_rx_buffer_get(&len)),"1");
		//XPRINT(level_printf_char,SIM800A_PRINTF_LEVEL,"sim800a receive<<",sim800a_rx_buffer_get(&len),len);
		printf_char(sim800a_rx_buffer_get(&len),len);
		sim800a_rx_buffer_clean();
		sim800a_net_close();
		delay_ms(AT_ONE_DIRECTIVE_DELAY_TIME);
		presp = sim800a_rec_check((char*)(sim800a_rx_buffer_get(&len)),"OK");
		key_word = sim800a_rec_check((char*)(sim800a_rx_buffer_get(&len)),"1");
		//XPRINT(level_printf_char,SIM800A_PRINTF_LEVEL,"sim800a receive<<",sim800a_rx_buffer_get(&len),len);
		printf_char(sim800a_rx_buffer_get(&len),len);
		sim800a_rx_buffer_clean();
	for (i = 0; i < ATCMD_MAX_REPEAT_NUMS; i++)
	{
		//AT+CGATT? 
		sim800a_check_GPRS_state();
		delay_ms(5*1000);
		presp = sim800a_rec_check((char*)(sim800a_rx_buffer_get(&len)),"OK");
		key_word = sim800a_rec_check((char*)(sim800a_rx_buffer_get(&len)),"1");
		//XPRINT(level_printf_char,SIM800A_PRINTF_LEVEL,"sim800a receive<<",sim800a_rx_buffer_get(&len),len);
		printf_char(sim800a_rx_buffer_get(&len),len);
		sim800a_rx_buffer_clean();
		if(presp != NULL && key_word != NULL)
		{
			break;
		}
		else if(i == ATCMD_MAX_REPEAT_NUMS - 1)
		{
			return NET_CONNRTION_ERR;
		}
	}
	for (i = 0; i < ATCMD_MAX_REPEAT_NUMS; i++)
	{
		//AT+CSTT=”CMNET”
		sim800a_start_task();
		delay_ms(AT_ONE_DIRECTIVE_DELAY_TIME);
		presp = sim800a_rec_check((char*)(sim800a_rx_buffer_get(&len)),"OK");
		//XPRINT(level_printf_char,SIM800A_PRINTF_LEVEL,"sim800a receive<<",sim800a_rx_buffer_get(&len),len);
		printf_char(sim800a_rx_buffer_get(&len),len);
		sim800a_rx_buffer_clean();
		if(presp != NULL)
		{
			break;
		}
		else if(i == ATCMD_MAX_REPEAT_NUMS - 1)
		{
			return NET_CONNRTION_ERR;
		}
	}
	for (i = 0; i < ATCMD_MAX_REPEAT_NUMS; i++)
	{
		//AT+CIICR 
		sim800a_bring_up_connection();
		delay_ms(AT_ONE_DIRECTIVE_DELAY_TIME);
		presp = sim800a_rec_check((char*)(sim800a_rx_buffer_get(&len)),"OK");
		//XPRINT(level_printf_char,SIM800A_PRINTF_LEVEL,"sim800a receive<<",sim800a_rx_buffer_get(&len),len);
		printf_char(sim800a_rx_buffer_get(&len),len);
		sim800a_rx_buffer_clean();
		if(presp != NULL )
		{
			break;
		}
	}
		//AT+CIFSR
		sim800a_get_ip();
		delay_ms(AT_ONE_DIRECTIVE_DELAY_TIME);
		//presp = sim800a_rec_check((char*)(sim800a_rx_buffer_get(&len)),"OK");
		//XPRINT(level_printf_char,SIM800A_PRINTF_LEVEL,"sim800a receive<<",sim800a_rx_buffer_get(&len),len);
		printf_char(sim800a_rx_buffer_get(&len),len);
		sim800a_rx_buffer_clean();
	for (i = 0; i < ATCMD_MAX_REPEAT_NUMS; i++)
	{
		//AT+CIPSTART=”UDP,
		sim800a_configure_server_settings((char*)UDP_ipv4_addr,(char*)UDP_local_port);
		delay_ms(AT_ONE_DIRECTIVE_DELAY_TIME);
		presp = sim800a_rec_check((char*)(sim800a_rx_buffer_get(&len)),"OK");
		//XPRINT(level_printf_char,SIM800A_PRINTF_LEVEL,"sim800a receive<<",sim800a_rx_buffer_get(&len),len);
		printf_char(sim800a_rx_buffer_get(&len),len);
		sim800a_rx_buffer_clean();
		if(presp != NULL)
		{
			break;
		}
		else if(i == ATCMD_MAX_REPEAT_NUMS - 1)
		{
			return NET_CONNRTION_ERR;
		}
	}
	return NET_CONNRTION_SUCCEED;	
}


u8 sim800a_UDP_send(u8* data,u32 data_len)
{
		
		//sim800a_send_data((char*)data,data_len);
		net_send((char*)data,data_len);
		//XPRINT(level_printf_hex,SIM800A_PRINTF_LEVEL,"sim800a send data>>",data,data_len);
		//printf_string(data,data_len);

	return UDP_SEND_SUCCEED;
}


#define GPRS_HEAD1					0x7e
#define GPRS_HEAD2					0x0e				

#define GPRS_CMD_UPDATE_STAR		0xdd
#define GPRE_CMD_UPDATE_DATA		0xd0
#define GPRE_CMD_UPDATE_END			0xde
#define GPRS_CMD_UPDATE_STAR_ACK	0xd2
#define GPRS_CMD_UPDATE_DATA_ACK	0xd3
#define GPRS_CMD_UPDATE_END_ACK		0xd4


static u8 rx_buf[2000] = {0};
static u8 software_versioning[4] = {0};
static u8 send_ack_buf[30] = {0};
static u8 send_end_ack_buf[9] = {0};
void sim800a_task(void)
{
	static u8 connetion_state = NET_CONNRTION_ERR;
//	static u8 first_star = TRUE;
//	u8 err;
//	u8 *p = NULL;
	u32 i;
	//u8 send_data_buf[256] = {0};
	u8 send_data_len = 0;
	u32 len = 0;
	u32 length = 0;
	u16 rx_buf_len = 0;
	u16 crc_check = 0;
//	static u8 update_flag = 0;
	static u8 seq = 0;
	u32 update_address_offset = 0;
	static u16 update_date_len = 0;
	u8 data_ack_buf[9] = {0};
	static u16 update_packet_count = 1;
	u8 update_status = 0;
	static u8 heart_time_count = 0;
	u8 data_heart_buf = 0;
	static u8 rx_buf_no_clean_flag = 0;
	static u8 rx_buf_count = 0;
	static u8 rx_buf_get[1000] = {0};
	static u8 first_send_flag = 1;
	//while(1)
	{
/*	
		if(connetion_state == NET_CONNRTION_ERR)
		{
			for(i = 0;i < 10;i++)
			{
				delay_ms(1000);
			}
			//while(sim800a_creat_network_connetion() == NET_CONNRTION_ERR);
			while(net_connect());
			connetion_state = NET_CONNRTION_SUCCEED;
			update_count++;
		}
		else
*/
		{
			if(heart_time_count == 0)
			{
				if(first_send_flag)
				{
					data_heart_buf = 0x01; 
					send_data_len = gprs_send_data_packet(send_ack_buf,GPRS_CMD_HEART,seq,host_mac,&data_heart_buf,1);
					
					sim800a_UDP_send(send_ack_buf,send_data_len);
				}
				//no_update = 1;
				update_count++;
				if(!first_send_flag)
				{
						update_max_count = 10;
				}
				printf("update_count=%d\r\n",update_count);
			}
			if(heart_time_count == 1)
			{
				if(first_send_flag)
				{
					if(flash_flag!=2)
					{
						software_versioning[3] = 1;
					}
					else
					{
						software_versioning[3] = 2;
					}
					send_data_len = gprs_send_data_packet(send_ack_buf,GPRS_CMD_UPDATE_STAR_ACK,seq,host_mac,software_versioning,4);
					sim800a_UDP_send(send_ack_buf,send_data_len);
				}
			}
			heart_time_count++;
			if(heart_time_count>=10)
			{
				heart_time_count = 0;
			}
			//printf("程序运行\r\n");
			delay_ms(10000);
			for(i=0;i<1000;i++)
			{
				rx_buf_get[i] = 0;
				len = 0;
			}
			/*
			if(sim800a_rx_buffer_get(&len) != NULL  && len > 0)
			{
				memcpy(rx_buf_get,sim800a_rx_buffer_get(&len),len);
				length = gprsDecodeFrame(rx_buf,rx_buf_get,len);
				printf("数据接收:");
				printf_string(rx_buf,length);
				printf("\r\n");
				rx_buf_no_clean_flag = 0;
				if(length>2000)
				{
					sim800a_rx_buffer_clean();
				}
			*/
			if(get_fifo_used_length(&ringgprsbuf)>0)
			{
				len = get_fifo_used_length(&ringgprsbuf);
				//MEM_DUMP(SIM7600CE_PRINTF_LEVEL,"ringgprsbuf",ringgprsbuf.source,1024);
				//printf("fifo->front = %d\r\n",ringgprsbuf.front);
				fifo_Dequeue(&ringgprsbuf,rx_buf_get,len);
				printf("rx_buf_get_len = %d\r\n",len);
				MEM_DUMP(SIM7600CE_PRINTF_LEVEL,"REC_BUF",rx_buf_get,len);
				length = gprsDecodeFrame(rx_buf,rx_buf_get,len);
				
			
			for(i=0;i<length;i++)
			{
				if(rx_buf[i] == GPRS_HEAD1 && rx_buf[i + 1] == GPRS_HEAD2)
				{
					rx_buf_count++;
					rx_buf_no_clean_flag = 1;
					rx_buf_len = rx_buf[i + 5] + (rx_buf[i + 6] << 8);
					crc_check = crc16(rx_buf+i+1,rx_buf_len+6);
					printf("crc_check:");
					printf("%X",crc_check&0xff);printf("%X",crc_check>>8);
					printf("  crc_src:");
					printf("%X",rx_buf[i + 7 + rx_buf_len]);printf("%X",rx_buf[i + 7 + rx_buf_len + 1]);
					printf("\r\n");
					if(rx_buf[i + 7 + rx_buf_len] == (crc_check & 0xff) && rx_buf[i + 7 + rx_buf_len + 1] == (crc_check>>8 & 0xff))
					{
						if(memcmp(rx_buf+i+7,host_mac,4) == 0)
						{
							seq++;
							/*
							if(seq == 0x7f)
							{
								seq++;
							}*/
							if(rx_buf[i + 2] == GPRS_CMD_UPDATE_STAR)
							{
								//update_packet_count = 1;
								update_count = 0;
								memcpy(software_versioning,rx_buf + 11,3);
								//update_flag = 1;
								STMFLASH_Read(FLASH_FLAG_ADDR,&flash_flag,1);
								send_data_len = gprs_send_data_packet(send_ack_buf,GPRS_CMD_UPDATE_STAR_ACK,seq,host_mac,software_versioning,4);
								sim800a_UDP_send(send_ack_buf,send_data_len);
							}
							else if(rx_buf[i + 2] == GPRE_CMD_UPDATE_DATA )//&& update_flag)
							{
								update_count = 0;
								first_send_flag = 0;
								//no_update = 0;
								//if(update_packet_count == (rx_buf[i + 11]<<8) + rx_buf[i + 12])
								//printf("固件更新");
								LED1 = !LED1;
								{
									//updata_packet_num = update_packet_count;
									update_packet_count = (rx_buf[i + 11]<<8) + rx_buf[i + 12];
									if(update_packet_count == 2)
									{
										STMFLASH_Write(FLASH_ID_ADDR+14,(u16*)host_mac,4);
										//delay_ms(1000);
									}
									update_packet_count++;
									update_address_offset = (rx_buf[i + 13]<<24) + (rx_buf[i + 14]<<16) + (rx_buf[i + 15]<<8) + rx_buf[i + 16];
									update_date_len = (rx_buf[i + 17]<<8) + rx_buf[i + 18];
									printf("update_date_len=%d\r\n",update_date_len);
									if(flash_flag == 2)
									{			
										//STMFLASH_Write(FLASH_FLAG_ADDR,&FLASH1_UPDATE,1);
										if(FLASH_APP1_ADDR+update_address_offset<FLASH_APP1_MAX_ADDR)
										{
											iap_write_appbin(FLASH_APP1_ADDR+update_address_offset,rx_buf+i+19,update_date_len);//更新FLASH代码
											delay_ms(1000);
											//printf("flash_flag:");
											//printf_string((u8*)(FLASH_APP1_ADDR+update_address_offset),update_date_len);
											//printf("\r\n");
										}
										else
										{
											printf("固件超出flash大小");
										}
									}
									else 
									{
										//STMFLASH_Write(FLASH_FLAG_ADDR,&FLASH2_UPDATE,1);
										if(FLASH_APP2_ADDR+update_address_offset<FLASH_APP2_MAX_ADDR)
										{
											iap_write_appbin(FLASH_APP2_ADDR+update_address_offset,rx_buf+i+19,update_date_len);//更新FLASH代码
										}
										else
										{
											printf("固件超出flash大小");
										}
									}
									delay_ms(100);
									//memcpy(data_ack_buf,&update_address_offset,2);
									//memcpy(data_ack_buf + 2,&update_date_len,2);
									data_ack_buf[0] = (update_packet_count-1)>>8 &0xff;
									data_ack_buf[1] = (update_packet_count-1) &0xff;
									data_ack_buf[2] = update_address_offset>>24 & 0xff;
									data_ack_buf[3] = update_address_offset>>16 & 0xff;
									data_ack_buf[4] = update_address_offset>>8 & 0xff;
									data_ack_buf[5] = update_address_offset & 0xff;
									data_ack_buf[6] = update_date_len>>8 & 0xff;
									data_ack_buf[7] = update_date_len & 0xff;
									data_ack_buf[8] = 1;
									send_data_len = gprs_send_data_packet(send_ack_buf,GPRS_CMD_UPDATE_DATA_ACK,seq,host_mac,data_ack_buf,9);
									sim800a_UDP_send(send_ack_buf,send_data_len);
								}
								//else
								//{
									//printf("固件丢包");
									//update_flag = 0;
								//}
							}
							else if(rx_buf[i + 2] == GPRE_CMD_UPDATE_END)
							{
								update_count = 0;
								heart_time_count = 2;
								update_packet_count -= 1;
								update_address_offset += update_date_len;
								memcpy(software_versioning,rx_buf + i + 11,3);
								if(update_packet_count == (rx_buf[i + 15]<<8) + rx_buf[i + 16])//&& (update_address_offset == (rx_buf[i + 17]<<8) + rx_buf[i + 18]))
								{
									if(flash_flag == 2)
									{
										STMFLASH_Write(FLASH_FLAG_ADDR,&FLASH1_UPDATE,1);
									}
									else 
									{
										STMFLASH_Write(FLASH_FLAG_ADDR,&FLASH2_UPDATE,1);
									}
									update_status = 1;
									
								}
								else
								{
									update_status = 0;
								}
								memcpy(send_end_ack_buf,software_versioning,3);
								send_end_ack_buf[4] = update_packet_count>>8 & 0xff;
								send_end_ack_buf[5] = update_packet_count & 0xff;
								send_end_ack_buf[6] = update_address_offset>>8 & 0xff;
								send_end_ack_buf[7] = update_address_offset & 0xff;
								send_end_ack_buf[8] = update_status;
								send_data_len = gprs_send_data_packet(send_ack_buf,GPRS_CMD_UPDATE_END_ACK,seq,host_mac,send_end_ack_buf,9);
								sim800a_UDP_send(send_ack_buf,send_data_len);
								//update_flag = 0;
								delay_ms(1000);
								if(update_status)
								{
									STMFLASH_Read(FLASH_FLAG_ADDR,&flash_flag,1);
									if(flash_flag == FLASH1_UPDATE)
									{
										STMFLASH_Write(FLASH_ID_ADDR+14,(u16*)host_mac,4);
										delay_ms(1000);
										printf("开始执行FLASH1用户代码!!\r\n"); 
										iap_load_app(FLASH_APP1_ADDR);//执行FLASH APP代码
									}
									else if(flash_flag ==FLASH2_UPDATE)
									{
										STMFLASH_Write(FLASH_ID_ADDR+14,(u16*)host_mac,4);
										delay_ms(1000);
										printf("开始执行FLASH2用户代码!!\r\n"); 
										iap_load_app(FLASH_APP2_ADDR);//执行FLASH APP代码
									}
								}
							}
						}
						if(rx_buf[i+rx_buf_len+9] == GPRS_TAIL)
						{
						//sim800a_rx_buffer_clean();
							rx_buf_no_clean_flag = 0;
							rx_buf_count = 0;
						}
						break;
					}
				}
			}
			printf("rx_buf_no_clean_flag=%d\r\n",rx_buf_no_clean_flag);
			printf("rx_buf_count=%d\r\n",rx_buf_count);
			if((!rx_buf_no_clean_flag) || (rx_buf_count>=5))
			{
				printf("sim800a_rx_buffer_clean\r\n");
				sim800a_rx_buffer_clean();
				len = 0;
				rx_buf_count = 0;
				//delay_ms(300);
			}
			//p = OSQPend(q_sim800a_send_msg,0,&err);
			//send_data_len = osqGetOut0xef(send_data_buf,p,strlen((char*)p));
			/*
			if(sim800a_UDP_send(send_data_buf,send_data_len) == UDP_SEND_SUCCEED)
			{
				connetion_state = UDP_SEND_SUCCEED;
				delay_ms(5000);
				//gprs_handle();
			delay_ms(300);
			}
			else
			{
				connetion_state = NET_CONNRTION_ERR;
			}
			*/
		}
			
		}
	
		//delay_ms(1000);
	}
	
}

#endif

void printf_string(u8* data,u32 len)
{
	u32 i;
	for(i=0;i<len;i++)
	{
		printf("%X ",data[i]);
	}
}





