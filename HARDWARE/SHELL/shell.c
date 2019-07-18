#include "shell.h"
#include "usart.h"
#include "string.h"
#include "sys.h"
#include "delay.h"

//#include "sim800a.h" //测试后删除

#define FALSE 		0
#define TRUE 		1

#define EQUAL 		0

#ifdef shell_enable
#define LEVEL_NUM 	6
static u8 shell_level = 2; //打印等级，默认为2


const static char LEVEL_CHANGE_LEVEL_CMD[] = "s gdb ";    //调整调试等级命令
const static char MAIN_REBOOT_CMD[] = "s reboot";    //系统重启命令
const static char SINGLE_ECHO[] = "s singal";			//显示信号强度
const static char NB_CONNECT_STATE[] = "s nb state";      //查询NB-IoT连接状态
//const static char TEST_SEND[] = "s send";				//发送测试
//const static char CLOSE_UDP[] = "s close udp";			//关闭UDP
#ifdef USE_BC95
const static char SET_BAND5_CMD[] = "s band5";		//设置band为5，频段为850MHz(电信)
const static char SET_BAND8_CMD[] = "s band8";		//设置band为8，频段为900MHz(移动联通)
const static char SET_BAND20_CMD[] = "s band20";		//设置band为20，频段为800MHz
#endif
const static char TEST[] = "s test";


/*****************************************************
函数原型： static void sys_reboot(void)
功能：			系统重启
输入：			无
返回：			无
*****************************************************/
static void sys_reboot(void)
{
	__set_FAULTMASK(1);  //关中断
	NVIC_SystemReset();  //软件复位
}


/*****************************************************
函数原型： void shell_handle(void)
功能：			shell处理
输入：			无
返回：			无
*****************************************************/
u8 test_Flag = FALSE;
u8 reboot_flag = FALSE;
u8 band5_flag = FALSE;
u8 band8_flag = FALSE;
u8 band20_flag = FALSE;
u8 singal_flag = FALSE;
u8 nb_state_flag = FALSE;
//u8 test_send_flag = FALSE;
//u8 udp_close_flag = FALSE;
void shell_handle(void)
{
	//u8 i;
	//u8 level_change_ok = FALSE;
	u8 usart_rx_status;
	static u8 mcu_star = 1;
	if(mcu_star)
	{
		printf("System init OK\r\n");
		printf("\\(^jjj^)/\r\n");
		printf("The init level is: %d\r\n",shell_level);
		mcu_star = 0;
	}
	usart_rx_status = USART_RX_DATA();
	if(usart_rx_status != USART_RX_NO_OK)
	{
//		if((usart_rx_count == (strlen(LEVEL_CHANGE_LEVEL_CMD) + 2)) || (usart_rx_count == (strlen(LEVEL_CHANGE_LEVEL_CMD) + 1)))
//		{
//			for(i = 0;i < strlen(LEVEL_CHANGE_LEVEL_CMD);i++)
//			{
//				if(USART_RX_BUF[i] != LEVEL_CHANGE_LEVEL_CMD[i])
//				{
//					level_change_ok = FALSE;break;
//				}
//				else level_change_ok = TRUE;
//			}
//			if(level_change_ok)
//			{
//				printf("The old level is: %d\r\n",shell_level);
//				if(usart_rx_count == (strlen(LEVEL_CHANGE_LEVEL_CMD))+1)
//				{
//					shell_level = (USART_RX_BUF)[LEVEL_NUM] - '0';
//				}
//				else
//				{
//					shell_level = ((USART_RX_BUF)[LEVEL_NUM] - '0') * 10 + (USART_RX_BUF)[LEVEL_NUM+1] - '0';
//					
//				}
//				printf("The new level is: %d\r\n",shell_level);
//				XPRINT(level_printf_char,8,"test->","\\(jjj)/",7);
//			}
//			else return;
//		}
		if(memcmp(USART_RX_BUF,LEVEL_CHANGE_LEVEL_CMD,strlen(LEVEL_CHANGE_LEVEL_CMD)) == EQUAL)    //改变调试等级
		{
			printf("The old level is: %d\r\n",shell_level);
			if(usart_rx_status == (strlen(LEVEL_CHANGE_LEVEL_CMD))+1)
			{
				shell_level = (USART_RX_BUF)[LEVEL_NUM] - '0';
			}
			else
			{
				shell_level = ((USART_RX_BUF)[LEVEL_NUM] - '0') * 10 + (USART_RX_BUF)[LEVEL_NUM+1] - '0';
					
			}
			printf("The new level is: %d\r\n",shell_level);
			XPRINT(level_printf_char,8,"test->","\\(^jjj^)/",9);
		}
		else if(memcmp(USART_RX_BUF,MAIN_REBOOT_CMD,strlen(MAIN_REBOOT_CMD)) == EQUAL)    //软件复位
		{
			reboot_flag = TRUE;
			delay_ms(500);
			printf("System Reboot\r\n");
			sys_reboot();
		}
		else if(memcmp(USART_RX_BUF,SINGLE_ECHO,strlen(SINGLE_ECHO)) == EQUAL)
		{
			singal_flag = TRUE;
		}
#ifdef USE_BC95
		else if(memcmp(USART_RX_BUF,TEST,strlen(TEST)) == EQUAL)
		{
			test_Flag = TRUE;
			
		}
		else if(memcmp(USART_RX_BUF,SET_BAND5_CMD,strlen(SET_BAND5_CMD)) == EQUAL)
		{
			band5_flag = TRUE;
		}
		else if(memcmp(USART_RX_BUF,SET_BAND5_CMD,strlen(SET_BAND8_CMD)) == EQUAL)
		{
			band8_flag = TRUE;
		}
		else if(memcmp(USART_RX_BUF,SET_BAND5_CMD,strlen(SET_BAND20_CMD)) == EQUAL)
		{
			band20_flag = TRUE;
		}
		else if(memcmp(USART_RX_BUF,NB_CONNECT_STATE,strlen(NB_CONNECT_STATE)) == EQUAL)
		{
			nb_state_flag = TRUE;
		}
//		else if(memcmp(USART_RX_BUF,TEST_SEND,strlen(TEST_SEND)) == EQUAL)
//		{
//			test_send_flag = TRUE;
//		}
//		else if(memcmp(USART_RX_BUF,CLOSE_UDP,strlen(CLOSE_UDP)) == EQUAL)
//		{
//			udp_close_flag = TRUE;
//		}
#endif
	}
}



/*****************************************************
函数原型： void level_printf_char(u8 level,char *printf_head,void *printf_data,u32 printf_num)
功能：			字符打印
输入：			u8 level  等级
				char *printf_head 输出的头标识
				void *printf_data 打印的数据
				u32 printf_num  数据长度
返回：			无
*****************************************************/
void level_printf_char(u8 level,char *printf_head,void *printf_data,u32 printf_num)
{
	u8 i;
	if(level <= shell_level)
	{
		for(i = 0;i < strlen((char *)printf_head);i++)
		{
			printf("%c",printf_head[i]); 
		}
		printf("	");
		//printf("\r\n");
		if(printf_num <= strlen((char*)printf_data))
		{
			for(i = 0;i < printf_num;i++)
			{
				printf("%c",((char *)printf_data)[i]);
			}
			printf("\r\n");
		}
	}
}


/*****************************************************
函数原型： void level_printf_hex(u8 level,char *printf_head,void *printf_data,u32 printf_num)
功能：			16进制打印
输入：			u8 level  等级
				char *printf_head 输出的头标识
				void *printf_data 打印的数据
				u32 printf_num  数据长度
返回：			无
*****************************************************/
void level_printf_hex(u8 level,char *printf_head,void *printf_data,u32 printf_num)
{
	u8 i;
	if(level <= shell_level)
	{
		for(i = 0;i < strlen((char *)printf_head);i++)
		{
			printf("%c",printf_head[i]); 
		}
		printf("	");
		for(i = 0;i < printf_num;i++)
		{
			if(i%16 == 0)
			{
				printf("\n");
			}
			printf("0x%X ",((u8 *)printf_data)[i]);
		}
		printf("\r\n");
	}
}


/*****************************************************
函数原型： void level_printf_int(u8 level,char *printf_head,void *printf_data,u32 printf_num)
功能：			整形数打印
输入：			u8 level  等级
				char *printf_head 输出的头标识
				void *printf_data 打印的数据
				u32 printf_num  数据长度
返回：			无
*****************************************************/
void level_printf_int(u8 level,char *printf_head,void *printf_data,u32 printf_num)
{
	u8 i;
	if(level <= shell_level)
	{
		for(i = 0;i < strlen((char *)printf_head);i++)
		{
			printf("%c",printf_head[i]); 
		}
		printf("	");
		for(i = 0;i < printf_num;i++)
		{
			if(i%16 == 0)
			{
				printf("\n");
			}
			printf("%d ",((u8 *)printf_data)[i]);
		}
		printf("\r\n");
	}
}
#endif

/*****************************************************
函数原型： void XPRINT(void (*call_back_print)(u8 level,char *printf_head,void *printf_data,u32 printf_num),\
	u8 level,char *printf_head,void *printf_data,u32 printf_num)
功能：			打印回调
输入：			void (*call_back_print)(u8 level,char *printf_head,void *printf_data,u32 printf_num) 回调的打印函数
				u8 level  等级
				char *printf_head 输出的头标识
				void *printf_data 打印的数据
				u32 printf_num  数据长度
返回：			无
*****************************************************/
void XPRINT(void (*call_back_print)(u8 level,char *printf_head,void *printf_data,u32 printf_num),\
	u8 level,char *printf_head,void *printf_data,u32 printf_num)
{
	#ifdef shell_enable
	if((printf_num <= strlen((char*)printf_data)) && (printf_head != NULL) &&( printf_data != NULL) && (printf_num > 0))
	{
		call_back_print(level,printf_head,printf_data,printf_num);
	}
	#endif
}

