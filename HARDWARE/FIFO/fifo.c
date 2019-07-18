/*
队列的数组实现
2019.3.15
jjj
*/


#include <string.h>
#include "fifo.h"



/*****************************************************
函数原型： 		void fifo_Create(FIFO_T *fifo,uint8_t *buf,uint32_t buf_length)
功能：			初始化fifo
输入：			FIFO_T *fifo fifo结构体
				uint8_t *buf fifo数组
				uint32_t buf_length 数组大小
返回：			无
*****************************************************/
void fifo_Create(FIFO_T *fifo,uint8_t *buf,uint32_t buf_length)
{
	fifo->source = buf;
	fifo->front = 0;
	fifo->rear = 0;
	fifo->used_length = 0;
	fifo->fifo_length = buf_length;
}


/*****************************************************
函数原型： 		uint8_t fifo_ISEmpty(FIFO_T *fifo)
功能：			检测fifo是否为空
输入：			FIFO_T *fifo fifo结构体
返回：			1：空 0：非空
*****************************************************/
uint8_t fifo_ISEmpty(FIFO_T *fifo)
{
	return (fifo->used_length == 0);
}


/*****************************************************
函数原型： 		void fifo_cleanEmpty(FIFO_T *fifo)
功能：			清空fifo
输入：			FIFO_T *fifo fifo结构体
返回：			无
*****************************************************/
void fifo_cleanEmpty(FIFO_T *fifo)
{
	fifo->front = 0;
	fifo->rear = 0;
	fifo->used_length = 0;
}


/*****************************************************
函数原型： 		uint8_t fifo_IsFull(FIFO_T *fifo)
功能：			检测fifo是否满
输入：			FIFO_T *fifo fifo结构体
返回：			1：full 0：not full
*****************************************************/
uint8_t fifo_IsFull(FIFO_T *fifo)
{
	return (fifo->fifo_length == fifo->used_length);
}


/*****************************************************
函数原型： 		uint32_t fifo_HaveSpace(FIFO_T *fifo,uint32_t size)
功能：			检测fifo是否满
输入：			FIFO_T *fifo fifo结构体
				uint32_t size 需要写入fifo的长度
返回：			返回可以写入fifo的数据长度，0表示已满
*****************************************************/
uint32_t fifo_HaveSpace(FIFO_T *fifo,uint32_t size)
{
	if(!fifo_IsFull(fifo))
	{
		if((fifo->fifo_length - fifo->used_length) < size)
		{
			return (fifo->fifo_length - fifo->used_length);
		}
		else
		{
			return size;
		}
	}
	return 0;
}


/*****************************************************
函数原型： 		uint32_t fifo_Enqueue(FIFO_T *fifo,uint8_t *front_buf,uint32_t size)
功能：			插入队列
输入：			FIFO_T *fifo fifo结构体
				uint8_t *front_buf 输入队列的内容
				uint32_t size 输入队列的内容的长度
返回：			成功输入到队列的长度，0表示队列满
*****************************************************/
uint32_t fifo_Enqueue(FIFO_T *fifo,uint8_t *enqueue_buf,uint32_t size)
{
	uint32_t space = 0;
	space = fifo_HaveSpace(fifo,size);
	if(space != 0)
	{
		if((space + fifo->rear) <= fifo->fifo_length)
		{
			memcpy(fifo->source + fifo->rear,enqueue_buf,space);
		}
		else
		{
			memcpy(fifo->source + fifo->rear,enqueue_buf,fifo->fifo_length - fifo->rear);
			memcpy(fifo->source,enqueue_buf + (fifo->fifo_length - fifo->rear),space - (fifo->fifo_length - fifo->rear));
		}
		fifo->rear = (fifo->rear + space) % fifo->fifo_length;
		fifo->used_length += space;
	}
	return space;
}


/*****************************************************
函数原型： 		uint32_t fifo_Dequeue(FIFO_T *fifo,uint8_t *front_buf,uint32_t size)
功能：			出队
输入：			FIFO_T *fifo fifo结构体
				uint32_t size 需要出队的长度
返回：			返回实际出队的数据长度，0表示队列空
*****************************************************/
uint32_t fifo_Dequeue(FIFO_T *fifo,uint8_t *dequeue_buf,uint32_t size)
{
	uint32_t len = 0;
	if(fifo_ISEmpty(fifo))
	{
		return 0;
	}
	else
	{
		if(size > fifo->used_length)
		{
			len = fifo->used_length;
		}
		else
		{
			len = size;
		}
		
		if(len + fifo->front <= fifo->fifo_length)
		{
			memcpy(dequeue_buf,fifo->source + fifo->front,len);
		}
		else
		{
			memcpy(dequeue_buf,fifo->source + fifo->front,fifo->fifo_length - fifo->front);
			memcpy(dequeue_buf+fifo->fifo_length - fifo->front,fifo->source,len - (fifo->fifo_length - fifo->front));
		}
	}
	fifo->front = (fifo->front + len) % fifo->fifo_length;
	fifo->used_length -= len;
	return len;
}


/*****************************************************
函数原型： 		uint32_t get_fifo_used_length(FIFO_T *fifo)
功能：			队列已使用长度
输入：			FIFO_T *fifo fifo结构体
返回：			已使用长度
*****************************************************/
uint32_t get_fifo_used_length(FIFO_T *fifo)
{
	return fifo->used_length;
}


/*****************************************************
函数原型： 		uint32_t get_fifo_length(FIFO_T *fifo)
功能：			队列总长度
输入：			FIFO_T *fifo fifo结构体
返回：			长度
*****************************************************/
uint32_t get_fifo_length(FIFO_T *fifo)
{
	return fifo->fifo_length;
}









