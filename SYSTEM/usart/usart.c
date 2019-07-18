#include "sys.h"
#include "usart.h"
#include "sysprintf.h"
#include "sim7600ce.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					//ucos 使用	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//串口1初始化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/8/18
//版本：V1.5
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************
//V1.3修改说明 
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART1_RX的使能方式
//V1.5修改说明
//1,增加了对UCOSII的支持
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

/*使用microLib的方法*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/
 
#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN] __attribute__ ((at(0X20001000)));//接收缓冲,最大USART_REC_LEN个字节,起始地址为0X20001000.    
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       	//接收状态标记	  
u16 USART_RX_CNT=0;			//接收的字节数	  
void USART1_IRQHandler(void)
{
	u8 res;	
#ifdef OS_CRITICAL_METHOD 	//如果OS_CRITICAL_METHOD定义了,说明使用ucosII了.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)//接收到数据
	{	 
		res=USART_ReceiveData(USART1);
		if(USART_RX_CNT<USART_REC_LEN)
		{
			USART_RX_BUF[USART_RX_CNT]=res;
			USART_RX_CNT++;			 									     
		}
	}
#ifdef OS_CRITICAL_METHOD 	//如果OS_CRITICAL_METHOD定义了,说明使用ucosII了.
	OSIntExit();  											 
#endif
} 
#endif	
void uart_init(u32 bound){
    //GPIO端口设置
   GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟以及复用功能时钟
     //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  

   //Usart1 NVIC 配置


   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(USART1, &USART_InitStructure); //初始化串口
#if EN_USART1_RX		  //如果使能了接收

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断
#endif
    USART_Cmd(USART1, ENABLE);                    //使能串口 

}


void uart4_init(u32 bound){
    //GPIO端口设置
   GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);	//使能USART1，GPIOA时钟以及复用功能时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	//使能USART1，GPIOA时钟以及复用功能时钟
     //USART2_TX   PA.2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PA.2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOC, &GPIO_InitStructure);
   
    //USART2_RX	  PA.3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOC, &GPIO_InitStructure);  

   //Usart1 NVIC 配置


   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(UART4, &USART_InitStructure); //初始化串口
#if EN_USART1_RX		  //如果使能了接收

    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
    USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//开启中断
#endif
    USART_Cmd(UART4, ENABLE);                    //使能串口 

}
 

//#if EN_USART2_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   
/*
u8 USART2_RX_BUF[USART2_REC_LEN] ={0};   
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART2_RX_STA=0;       	//接收状态标记	  
u16 USART2_RX_CNT=0;			//接收的字节数	  
void USART2_IRQHandler(void)
{
	u8 res;	
#ifdef OS_CRITICAL_METHOD 	//如果OS_CRITICAL_METHOD定义了,说明使用ucosII了.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//接收到数据
	{	 
		res=USART_ReceiveData(USART2);
		if(USART2_RX_CNT<USART2_REC_LEN)
		{
			USART2_RX_BUF[USART2_RX_CNT]=res;
			USART2_RX_CNT++;			 									     
		}
	}
#ifdef OS_CRITICAL_METHOD 	//如果OS_CRITICAL_METHOD定义了,说明使用ucosII了.
	OSIntExit();  											 
#endif
} 
#endif*/	




#define USART4_MAX_RX_SIZE		1024
volatile u32 USART4_RX_STA = 0;
u8 USART4_RX_BUF[USART4_MAX_RX_SIZE];
//#define USART2_TX_FIFO_SIZE  256  //size must be power of 2. 
//static u8 Usart2_Tx_fifo[USART2_TX_FIFO_SIZE] = {0};
//#define USART2_MAX_RECV_LEN		256				//最大接收缓存字节数
//static struct __kfifo Usart2Rx_fifo_dev=
//{
//    Usart2_Rx_fifo,    //缓冲区
//    USART2_RX_FIFO_SIZE,  //大小
//    0,       	//入口位置
//    0,        	//出口位置
//};



void UART4_IRQHandler(void)                	//串口2中断服务程序
	{
	//u8 Res;
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
	if(USART_GetFlagStatus(UART4, USART_FLAG_ORE) != RESET)
	{
		USART_ClearFlag(UART4, USART_FLAG_ORE);
		USART_ReceiveData(UART4);
	}
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  
	{
		if(USART4_RX_STA < USART4_MAX_RX_SIZE)
		{
			//Res =USART_ReceiveData(USART2);	//读取接收到的数据
			USART4_RX_BUF[USART4_RX_STA++] = USART_ReceiveData(UART4);
		}
  } 
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif
} 
 

void USART_SendData8(USART_TypeDef* USARTx, uint8_t Data)
{
  /* Transmit Data */
  USARTx->DR = Data;
}



/*
void USART2_send(char *data,u8 num)
{
        u8 i;
	for(i = 0;i < num;i++)
	{
            USART_SendData8(USART2,data[i]);
            while(USART_GetFlagStatus(USART1,USART_FLAG_TC) != 1);                      //等待发送完毕
	}
}
*/

void USART4_send(char *data,u8 num)
{
	//u8 temp_buf[100];
	u8 i;
//	u8 temp_buf[2];	//发送数据缓冲区
//	if(  ring_buffer_len_used(&Usart2Tx_fifo_dev) && (USART2->SR&0X40)  )
//	{
//		ring_buffer_get(&Usart2Tx_fifo_dev, temp_buf, 1);
//		USART2->DR = temp_buf[0];
//	}
	for(i = 0;i < num;i++)
	{
		while((UART4->SR&0X40)==0);
		UART4->DR = data[i];
	}
}


u8* usart4_rx_buffer_get(u32* len)
{
	*len = USART4_RX_STA;
	return USART4_RX_BUF;
}


void usart4_rx_buffer_clean(void)
{
	u32 i;
	MEM_DUMP(SIM7600CE_PRINTF_LEVEL,"GPRS<-",USART4_RX_BUF,USART4_RX_STA);
	for(i = 0;i < USART4_REC_LEN;i++)
	{
		USART4_RX_BUF[i] = 0;
	}
	USART4_RX_STA = 0;
}


int dbg_send_char (int ch) 
{
	while (!(USART1->SR & USART_FLAG_TXE)); 
	USART1->DR = (ch & 0x1FF);

	return (ch);
}


int dbg_get_key (void) 
{
	while (!(USART1->SR & USART_FLAG_RXNE));
	return ((int)(USART1->DR & 0x1FF));
}

void dgg_uart_putc ( uint8_t ch)
{
	while (!(USART1->SR & USART_FLAG_TXE)); 
	USART1->DR = (ch & 0x1FF);
}
