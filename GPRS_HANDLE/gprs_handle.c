#include "gprs_handle.h"
#include <string.h>
#include "crc16.h"

//GPRS接收打印等级
#define GPRS_PRINTF_LEVEL 				12


#define GPRS_CMD_PLACE					2

/*
void gprs_handle(void)
{
	u32 len;
	u32 i;
	u8* gprs_rx_buffer = NULL;
	u8* gprs_rx_curr_buffer = NULL;
	u32 gprs_rx_curr_len = 0;
	u8 cout_time = 0;
	gprs_rx_buffer = GPRS_RX_BUFFER_GET(&len);
	for(i = 0;i < len;i++)
	{
		if(gprs_rx_buffer[i] == GPRS_HEAD && gprs_rx_buffer[i+1] == GPRS_SYN_CMD)
		{
			cout_time++;
			if(cout_time >= 2)
			{
				gprs_rx_curr_len = len - i;
				gprs_rx_curr_buffer = &gprs_rx_buffer[i];
				//XPRINT(level_printf_hex,GPRS_PRINTF_LEVEL,"GPRS receive<<",gprs_rx_curr_buffer,gprs_rx_curr_len);
				cout_time = 0;
			}
		}
	}
	if(gprs_rx_curr_buffer[GPRS_CMD_PLACE] == GPRS_CMD_HEART)
	{
		
	}
	GPRS_RX_BUFFER_CLEAN();
}
*/


/*****************************************************
函数原型： 		u32 gprs_send_data_packet(GPRS_AGREEMENT *pioBuf, u8 ubCmd, u16 uwSeq, const u8 *pcMAC, const u8 *pcData, u16 dataLen)
功能：			根据与服务器通信的协议进行数据打包
输入：			u8* pioBuf,				协议包指针
				u8 ubCmd, 				命令
				u16 uwSeq,	 			seq
				const u8 *pcMAC, 		主机mca
				const u8 *pcData, 		需要发送的数据指针
				u16 dataLen				发送数据的长度
返回：			整包数据长度
*****************************************************/
#ifdef USE_SIM800A
#define LEN_EXCEPT_DATA		10 //除了数据之外的包的长度
s32 gprs_send_data_packet(u8* pioBuf, u8 ubCmd, u16 uwSeq, const u8 *pcMAC, const u8 *pcData, u16 dataLen)
{
	static u16 nFrameL;
	u16 crc16_num;
	GPRS_AGREEMENT *pFrame = NULL;
	if (NULL == pioBuf)
	{
		return -1;
	}
	pFrame = (GPRS_AGREEMENT *)pioBuf;
	pFrame->gprsHead = GPRS_HEAD;
	pFrame->gprsSynCmd = GPRS_SYN_CMD;
	pFrame->gprsCmd = ubCmd;
	pFrame->gprsSeqL = uwSeq & 0xFF;
	pFrame->gprsSeqH = (uwSeq >> 8) & 0xFF;
	pFrame->gprsLenL = (dataLen + MAC_LENTH) & 0xFF;
	pFrame->gprsLenH = ((dataLen + MAC_LENTH) >> 8) & 0xFF;
	memcpy(pFrame->grrsHostMac,pcMAC,MAC_LENTH);
//	if(pcData == NULL || dataLen == 0)
//	{
//		pFrame->gprsLenL = MAC_LENTH;
//		pFrame->gprsLenH = 0;
//	}
//	else
//	{
	if(pcData != NULL && dataLen != 0)
	{
		memcpy(pFrame->gprsData,pcData,dataLen);
	}
//	}
	crc16_num = crc16((u8*)(&(pioBuf[1])),dataLen + 6 + MAC_LENTH); 	//crc从除包头外的第二位同步命令开始计算到数据结尾
	pFrame->gprsData[dataLen] = crc16_num & 0xFF;
	pFrame->gprsData[dataLen + 1] = (crc16_num >> 8) & 0xFF;
	pFrame->gprsData[dataLen + 2] = GPRS_TAIL;
	
	nFrameL = dataLen + LEN_EXCEPT_DATA + MAC_LENTH;
	return nFrameL;
}
#endif













