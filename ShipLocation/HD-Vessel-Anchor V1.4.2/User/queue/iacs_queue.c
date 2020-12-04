#include "common.h"

/***********************************************
Function: Create a queue;
************************************************/
int init_queue(Queue * queue, int maxsize)
{
//    queue->item = (Item *)malloc(sizeof(Item) * maxsize);
    queue->item = (Item *)mymalloc(SRAMIN, sizeof(Item) * maxsize);
    if(!queue->item) {
        log_print( ERR, ("Memory allocation failure\r\n"));
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


