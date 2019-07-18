/*
���е�����ʵ��
2019.3.15
jjj
*/


#include <string.h>
#include "fifo.h"



/*****************************************************
����ԭ�ͣ� 		void fifo_Create(FIFO_T *fifo,uint8_t *buf,uint32_t buf_length)
���ܣ�			��ʼ��fifo
���룺			FIFO_T *fifo fifo�ṹ��
				uint8_t *buf fifo����
				uint32_t buf_length �����С
���أ�			��
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
����ԭ�ͣ� 		uint8_t fifo_ISEmpty(FIFO_T *fifo)
���ܣ�			���fifo�Ƿ�Ϊ��
���룺			FIFO_T *fifo fifo�ṹ��
���أ�			1���� 0���ǿ�
*****************************************************/
uint8_t fifo_ISEmpty(FIFO_T *fifo)
{
	return (fifo->used_length == 0);
}


/*****************************************************
����ԭ�ͣ� 		void fifo_cleanEmpty(FIFO_T *fifo)
���ܣ�			���fifo
���룺			FIFO_T *fifo fifo�ṹ��
���أ�			��
*****************************************************/
void fifo_cleanEmpty(FIFO_T *fifo)
{
	fifo->front = 0;
	fifo->rear = 0;
	fifo->used_length = 0;
}


/*****************************************************
����ԭ�ͣ� 		uint8_t fifo_IsFull(FIFO_T *fifo)
���ܣ�			���fifo�Ƿ���
���룺			FIFO_T *fifo fifo�ṹ��
���أ�			1��full 0��not full
*****************************************************/
uint8_t fifo_IsFull(FIFO_T *fifo)
{
	return (fifo->fifo_length == fifo->used_length);
}


/*****************************************************
����ԭ�ͣ� 		uint32_t fifo_HaveSpace(FIFO_T *fifo,uint32_t size)
���ܣ�			���fifo�Ƿ���
���룺			FIFO_T *fifo fifo�ṹ��
				uint32_t size ��Ҫд��fifo�ĳ���
���أ�			���ؿ���д��fifo�����ݳ��ȣ�0��ʾ����
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
����ԭ�ͣ� 		uint32_t fifo_Enqueue(FIFO_T *fifo,uint8_t *front_buf,uint32_t size)
���ܣ�			�������
���룺			FIFO_T *fifo fifo�ṹ��
				uint8_t *front_buf ������е�����
				uint32_t size ������е����ݵĳ���
���أ�			�ɹ����뵽���еĳ��ȣ�0��ʾ������
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
����ԭ�ͣ� 		uint32_t fifo_Dequeue(FIFO_T *fifo,uint8_t *front_buf,uint32_t size)
���ܣ�			����
���룺			FIFO_T *fifo fifo�ṹ��
				uint32_t size ��Ҫ���ӵĳ���
���أ�			����ʵ�ʳ��ӵ����ݳ��ȣ�0��ʾ���п�
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
����ԭ�ͣ� 		uint32_t get_fifo_used_length(FIFO_T *fifo)
���ܣ�			������ʹ�ó���
���룺			FIFO_T *fifo fifo�ṹ��
���أ�			��ʹ�ó���
*****************************************************/
uint32_t get_fifo_used_length(FIFO_T *fifo)
{
	return fifo->used_length;
}


/*****************************************************
����ԭ�ͣ� 		uint32_t get_fifo_length(FIFO_T *fifo)
���ܣ�			�����ܳ���
���룺			FIFO_T *fifo fifo�ṹ��
���أ�			����
*****************************************************/
uint32_t get_fifo_length(FIFO_T *fifo)
{
	return fifo->fifo_length;
}









