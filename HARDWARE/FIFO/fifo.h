#ifndef FIFO_H
#define FIFO_H

#include "sys.h"
typedef struct fifo_t
{
	uint8_t *source;
	uint32_t front;
	uint32_t rear;
	uint32_t used_length;
	uint32_t fifo_length;
}FIFO_T;

void fifo_Create(FIFO_T *fifo,uint8_t *buf,uint32_t buf_length);
uint8_t fifo_ISEmpty(FIFO_T *fifo);
void fifo_cleanEmpty(FIFO_T *fifo);
uint8_t fifo_IsFull(FIFO_T *fifo);
uint32_t fifo_HaveSpace(FIFO_T *fifo,uint32_t size);
uint32_t fifo_Enqueue(FIFO_T *fifo,uint8_t *enqueue_buf,uint32_t size);
uint32_t fifo_Dequeue(FIFO_T *fifo,uint8_t *dequeue_buf,uint32_t size);
uint32_t get_fifo_used_length(FIFO_T *fifo);
uint32_t get_fifo_length(FIFO_T *fifo);


#endif
