#ifndef SIM7600CE_CONNECT_H
#define SIM7600CE_CONNECT_H

#include "sys.h"
#include "fifo.h"
#include "sim7600ce.h"

u8 net_connect(void);
u8 net_send(char* data,u32 length);
void GPRS_reset(void);
void GPRS_init(void);

#define GPRS_BUF_LEN	1024
extern FIFO_T ringgprsbuf;
extern uint8_t gprs_buf[GPRS_BUF_LEN];

#define sysSetPinStat(port,pin,stat) GPIO_WriteBit(port,pin,(BitAction)((stat)!=0))
#define sysSetPinStatReverse(port,pin) GPIO_WriteBit(port,pin,GPIO_ReadOutputDataBit(port,pin))


//模块的控制引脚
#define GPRS_STATUS_PIN					GPIO_Pin_5
#define GPRS_PWRKEY_PIN					GPIO_Pin_4
#define GPRS_SRST_PIN					GPIO_Pin_3
#define GPRS_CONTROL_PIN_PORT			GPIOB

#define GPRS_POWER_EN_PIN				GPIO_Pin_5
#define GPRS_POWER_EN_PORT				GPIOB 


#define GPRS_STATUS(a)		sysSetPinStat(GPRS_CONTROL_PIN_PORT,GPRS_STATUS_PIN,a)
#define GPRS_PWRKEY(a)		sysSetPinStat(GPRS_CONTROL_PIN_PORT,GPRS_PWRKEY_PIN,a)
#define GPRS_SRST(a)		sysSetPinStat(GPRS_CONTROL_PIN_PORT,GPRS_SRST_PIN,a)
#define GPRS_EN(a)			sysSetPinStat(GPRS_POWER_EN_PORT, GPRS_POWER_EN_PIN, a)
#define GPRS_ENOLD(a)		sysSetPinStat(GPRS_POWER_EN_PORT_O, GPRS_POWER_EN_PIN_O, a)

void GPRS_PIN_init(void);
void GPRS_SET_RESET_PIN(void);
void GPRS_RESET_RESET_PIN(void);

void GPRS_init(void);
void GPRS_reset(void);

#endif
