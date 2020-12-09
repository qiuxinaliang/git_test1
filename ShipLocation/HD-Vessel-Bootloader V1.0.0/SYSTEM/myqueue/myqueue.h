/*
 * @:*********************************************************************************************************: 
 * @Date: 2020-08-17 15:35:11
 * @LastEditTime: 2020-08-24 14:31:52
 * @**********************************************************************************************************: 
 */
#ifndef __MY_QUEUE_H_
#define __MY_QUEUE_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#define Msg_Queue_Length 2
typedef uint8_t    Item;  

//#package(1)
typedef struct Exception_for_msg
{
  uint8_t msg_type;
  uint8_t msg_value;
}Exception_for_msg;
//#package()

//#package(1)
typedef struct queue 
{
	Item *item;
	volatile int front;    //指向队列第一个元素
	volatile int rear;    //指向队列最后一个元素的下一个元素
	int maxsize; //循环队列的最大存储空间
}Queue,*pQueue;
//#package()

int init_queue(Queue * queue, int maxsize);
void traverse_queue(Queue * queue);
uint8_t is_queue_full(Queue * queue);
uint8_t is_queue_empty(Queue * queue);
uint8_t en_queue(Queue * queue, Item item);
uint8_t de_queue(Queue * queue, Item *item);
int get_queue_length(Queue * queue);
void destroy_queue(Queue * queue);
void bzero_queue(Queue * queue);

extern Queue Msg_For_Exception_queue;

#define MSG_FOR_EXCEPTION_QUEUE_INIT(queue_size)\
{\
    Msg_For_Exception_queue.item = NULL;\
    init_queue(&Msg_For_Exception_queue,queue_size);\
}

//void Msg_Queue_Send(uint8_t msg_num);
void Msg_Queue_Send(uint8_t *msg_num);
uint32_t Msg_Queue_Rev(uint8_t* data);

#endif
