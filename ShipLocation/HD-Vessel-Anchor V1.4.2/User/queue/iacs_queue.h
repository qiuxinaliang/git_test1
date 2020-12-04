/*
 * @:*********************************************************************************************************: 
 * @Date: 2020-11-10 13:50:44
 * @LastEditTime: 2020-11-11 18:04:57
 * @**********************************************************************************************************: 
 */
#ifndef __IACS_QUEUE_H_
#define __IACS_QUEUE_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
//#include "sys_types.h"

typedef uint8_t    Item;  

typedef struct queue 
{
	Item *item;
	volatile int front;    //指向队列第一个元素
	volatile int rear;    //指向队列最后一个元素的下一个元素
	int maxsize; //循环队列的最大存储空间
}Queue,*pQueue;

int init_queue(Queue * queue, int maxsize);
void traverse_queue(Queue * queue);
uint8_t is_queue_full(Queue * queue);
uint8_t is_queue_empty(Queue * queue);
uint8_t en_queue(Queue * queue, Item item);
uint8_t de_queue(Queue * queue, Item *item);
int get_queue_length(Queue * queue);
void destroy_queue(Queue * queue);
void bzero_queue(Queue * queue);

#endif

