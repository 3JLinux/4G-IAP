#include "shell.h"
#include "usart.h"
#include "string.h"
#include "sys.h"
#include "delay.h"

//#include "sim800a.h" //���Ժ�ɾ��

#define FALSE 		0
#define TRUE 		1

#define EQUAL 		0

#ifdef shell_enable
#define LEVEL_NUM 	6
static u8 shell_level = 2; //��ӡ�ȼ���Ĭ��Ϊ2


const static char LEVEL_CHANGE_LEVEL_CMD[] = "s gdb ";    //�������Եȼ�����
const static char MAIN_REBOOT_CMD[] = "s reboot";    //ϵͳ��������
const static char SINGLE_ECHO[] = "s singal";			//��ʾ�ź�ǿ��
const static char NB_CONNECT_STATE[] = "s nb state";      //��ѯNB-IoT����״̬
//const static char TEST_SEND[] = "s send";				//���Ͳ���
//const static char CLOSE_UDP[] = "s close udp";			//�ر�UDP
#ifdef USE_BC95
const static char SET_BAND5_CMD[] = "s band5";		//����bandΪ5��Ƶ��Ϊ850MHz(����)
const static char SET_BAND8_CMD[] = "s band8";		//����bandΪ8��Ƶ��Ϊ900MHz(�ƶ���ͨ)
const static char SET_BAND20_CMD[] = "s band20";		//����bandΪ20��Ƶ��Ϊ800MHz
#endif
const static char TEST[] = "s test";


/*****************************************************
����ԭ�ͣ� static void sys_reboot(void)
���ܣ�			ϵͳ����
���룺			��
���أ�			��
*****************************************************/
static void sys_reboot(void)
{
	__set_FAULTMASK(1);  //���ж�
	NVIC_SystemReset();  //�����λ
}


/*****************************************************
����ԭ�ͣ� void shell_handle(void)
���ܣ�			shell����
���룺			��
���أ�			��
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
		if(memcmp(USART_RX_BUF,LEVEL_CHANGE_LEVEL_CMD,strlen(LEVEL_CHANGE_LEVEL_CMD)) == EQUAL)    //�ı���Եȼ�
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
		else if(memcmp(USART_RX_BUF,MAIN_REBOOT_CMD,strlen(MAIN_REBOOT_CMD)) == EQUAL)    //�����λ
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
����ԭ�ͣ� void level_printf_char(u8 level,char *printf_head,void *printf_data,u32 printf_num)
���ܣ�			�ַ���ӡ
���룺			u8 level  �ȼ�
				char *printf_head �����ͷ��ʶ
				void *printf_data ��ӡ������
				u32 printf_num  ���ݳ���
���أ�			��
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
����ԭ�ͣ� void level_printf_hex(u8 level,char *printf_head,void *printf_data,u32 printf_num)
���ܣ�			16���ƴ�ӡ
���룺			u8 level  �ȼ�
				char *printf_head �����ͷ��ʶ
				void *printf_data ��ӡ������
				u32 printf_num  ���ݳ���
���أ�			��
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
����ԭ�ͣ� void level_printf_int(u8 level,char *printf_head,void *printf_data,u32 printf_num)
���ܣ�			��������ӡ
���룺			u8 level  �ȼ�
				char *printf_head �����ͷ��ʶ
				void *printf_data ��ӡ������
				u32 printf_num  ���ݳ���
���أ�			��
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
����ԭ�ͣ� void XPRINT(void (*call_back_print)(u8 level,char *printf_head,void *printf_data,u32 printf_num),\
	u8 level,char *printf_head,void *printf_data,u32 printf_num)
���ܣ�			��ӡ�ص�
���룺			void (*call_back_print)(u8 level,char *printf_head,void *printf_data,u32 printf_num) �ص��Ĵ�ӡ����
				u8 level  �ȼ�
				char *printf_head �����ͷ��ʶ
				void *printf_data ��ӡ������
				u32 printf_num  ���ݳ���
���أ�			��
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

