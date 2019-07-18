#ifndef GPRS_HANDLE_H
#define GPRS_HANDLE_H

#include "sys.h"
#include "usart.h"

#ifdef USE_SIM800A
#include "sim800a.h"
#define GPRS_REC_CHECK	sim800a_rec_check
#endif


#ifdef USE_BC95		
#include "bc95.h"
#define GPRS_REC_CHECK	bc95_rec_check
#endif

//与服务器通信协议中的特殊字符
#ifdef USE_SIM800A
#define GPRS_HEAD						0x7e
#define	GPRS_SYN_CMD					0x0e
#define GPRS_TAIL						0x7f

#define GPRS_CMD_HEART					0x01
#define GPRS_CMD_ALARM					0x02
#define GPRS_CMD_HEART_ACK				0x99
#define GPRS_CMD_SERVER_SYNC			0x03
#define GPRS_CMD_PASS_THTOUGH			0x04
#define GPRS_CMD_PASS_THTOUGH_ACK		0x07
#define GPRS_CMD_ALARM_ACK				0x05
#define GPRS_CMD_HOST_SYNC				0x06
#define GPRS_CMD_HOST_SYNC_ACK			0x99
#define GPRS_CMD_SMOKE_CHECK			0x08
#define GPRS_CMD_SMOKE_CHECK_ACK		0x09
#define GPRS_CMD_ELECTRICAL_CONTROL 	0x0a
#define GPRS_CMD_ELECTRICAL_CONTROL_ACK 0x0b
#define GPRS_CMD_ELECTRICAL_STATE		0x0c
#endif

#ifdef USE_BC95
#define GPRS_HEAD						0x7e
#define	GPRS_SYN_CMD					0x1e
#define GPRS_TAIL						0x7f

#define GPRS_CMD_HEART					0x01
#define GPRS_CMD_HEART_ACK				0x99
#define GPRS_CMD_ALARM					0x02
#define GPRS_CMD_ALARM_ACK				0x03
#define GPRS_CMD_DATA					0x04
#define GPRS_CMD_DATA_ACK				0x05
#endif


//extern u8* usart2_rx_buffer_get(u8* len);
//extern void usart2_rx_buffer_clean(void);
#define GPRS_RX_BUFFER_GET		usart2_rx_buffer_get
#define GPRS_RX_BUFFER_CLEAN	usart2_rx_buffer_clean


#define MAC_LENTH		4
typedef struct gprs_agreement
{
	u8 gprsHead;
	u8 gprsSynCmd;
	u8 gprsCmd;
	u8 gprsSeqL;
	u8 gprsSeqH;
	u8 gprsLenL;
	u8 gprsLenH;
	u8 grrsHostMac[MAC_LENTH];
	u8 gprsData[]; //data + CRC_L + CRC_H + gprsTrail
}GPRS_AGREEMENT;



void gprs_handle(void);
s32 gprs_send_data_packet(u8* pioBuf, u8 ubCmd, u16 uwSeq, const u8 *pcMAC, const u8 *pcData, u16 dataLen);

#endif
