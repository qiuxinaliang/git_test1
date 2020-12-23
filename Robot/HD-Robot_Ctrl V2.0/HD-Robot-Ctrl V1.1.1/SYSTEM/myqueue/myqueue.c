#include "common.h"

/***********************************************
Function: Create a queue;
************************************************/
int init_queue(Queue * queue, int maxsize)
{
//    queue->item = (Item *)malloc(sizeof(Item) * maxsize);
    queue->item = (Item *)mymalloc(SRAMIN, sizeof(Item) * maxsize);
    if(!queue->item) {
        log_print( DEBUG, ("Memory allocation failure\r\n"));
        return (-1);
    }
    queue->front = queue->rear = 0;
    queue->maxsize = maxsize;
    mymemset(queue->item, 0x00, sizeof(Item) * maxsize);
    return 0;
}

void bzero_queue(Queue * queue)
{
    queue->front = queue->rear = 0;
    mymemset(queue->item, 0x00, queue->maxsize);
    return;
}

/***********************************************
Function: Print the queue element;
************************************************/
void traverse_queue(Queue * queue)
{
    if(!queue->item) {
        return;
    }
    if(is_queue_empty(queue)) {
        log_print( DEBUG, ("the queue is the queue is empty\r\n"));
        return;
    }

    int i = queue->front;
    log_print( DEBUG, ("the element in the queue is:\r\n"));
    while(i % queue->maxsize != queue->rear) {
        log_print( DEBUG, ("%d ", queue->item[i]));
        i++;
    }
    log_print( DEBUG, ("\r\n"));
#if 0
    int tail = pq->front ;

    while(tail != pq->rear) {

        log_print( DEBUG, (" %d ", pq->item[tail]));

        tail = (tail + 1) % queue->maxsize;

    }
#endif

}
uint8_t is_queue_full(Queue * queue)
{
    if(queue->front == (queue->rear + 1) % queue->maxsize) { //判断循环链表是否满，留一个预留空间不用
        return 1;
    } else {
        return 0;
    }
}
uint8_t is_queue_empty(Queue * queue)
{
    if(queue->front == queue->rear) {
        return 1;
    } else {
        return 0;
    }
}
uint8_t en_queue(Queue * queue, Item item)
{
    if(!queue->item || is_queue_full(queue)) {
        return 0;
    } else {
        queue->item[queue->rear] = item;
        queue->rear = (queue->rear + 1) % queue->maxsize;
        return 1;
    }
}

uint8_t de_queue(Queue * queue, Item *item)
{
    if(!queue->item || is_queue_empty(queue)) {
        return 0;
    } else {
        (*item) = queue->item[queue->front];
        queue->front = (queue->front + 1) % queue->maxsize;
        return 1;
    }
}

int get_queue_length(Queue * queue)
{
    if(!queue->item) {
        return -1;
    }

    return ((queue->rear - queue->front + queue->maxsize) % queue->maxsize);
}

void destroy_queue(Queue * queue)
{
    if(queue->item) {
//		free(queue->item);
        myfree(SRAMIN, queue->item);
    }
}




/**
 * @:*********************************************************************************************************: 
 * @函 数 名: Msg_Queue_Send(uint8_t msg_num)
 * @功能说明: 异常消息发送
 * @形    参: uint8_t msg_num：异常消息
 * @返 回 值: 无
 * @**********************************************************************************************************: 
 */
Queue Msg_For_Exception_queue;
void Msg_Queue_Send(uint8_t *msg_num)
{
  //en_queue(&Msg_For_Exception_queue, (Item)msg_num); // put it into the queue
  uint32_t data_to_write = Msg_Queue_Length;

  uint8_t *ptr = (uint8_t *)msg_num;

  while(data_to_write--) {
      //USART_SendData(USART1, *(uint8_t *)ptr);
      HAL_UART_Transmit(&huart1, (uint8_t *)ptr, 1, 0xffff);
      en_queue(&Msg_For_Exception_queue, (Item)(*ptr));
      /* Loop until the end of transmission */
      if(data_to_write) {
          ptr++;
      }
  }
  ptr = NULL;

}

/**
 * @:*********************************************************************************************************: 
 * @函 数 名: Msg_Queue_Rev(uint8_t msg_num)
 * @功能说明: 异常消息接收
 * @形    参: uint8_t msg_num：异常消息
 * @返 回 值: 无
 * @**********************************************************************************************************: 
 */
uint32_t Msg_Queue_Rev(uint8_t* data)
{
	uint32_t ret = 0;
	//de_queue(&Msg_For_Exception_queue, data);
	#if 1
	uint32_t data_to_read = Msg_Queue_Length;

	while(data_to_read)
	{
		if(!de_queue(&Msg_For_Exception_queue, data)) {
				return ret;
		}
		data_to_read--;
		data++;
		ret++;
	}
	#endif
	return ret;
}

