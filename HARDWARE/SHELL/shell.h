#ifndef _SHELL_H
#define _SHELL_H
#include "sys.h"
#define shell_enable 1

#define ECHO_OPEN	1
#define ECHO_CLOSE	0

void shell_handle(void);

void level_printf_char(u8 level,char *printf_head,void *printf_data,u32 printf_num);
void level_printf_hex(u8 level,char *printf_head,void *printf_data,u32 printf_num);
void level_printf_int(u8 level,char *printf_head,void *printf_data,u32 printf_num);
void XPRINT(void (*call_back_print)(u8 level,char *printf_head,void *printf_data,u32 printf_num),\
	u8 level,char *printf_head,void *printf_data,u32 printf_num);
		
extern u8 test_Flag;
extern u8 reboot_flag;
extern u8 band5_flag;
extern u8 band8_flag;
extern u8 band20_flag;
extern u8 singal_flag;
extern u8 nb_state_flag;
extern u8 test_send_flag;
extern u8 udp_close_flag;
#endif


