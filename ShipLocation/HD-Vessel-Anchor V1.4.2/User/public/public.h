/*
 * @:*********************************************************************************************************: 
 * @Date: 2020-11-04 17:02:54
 * @LastEditTime: 2020-12-04 14:05:08
 * @**********************************************************************************************************: 
 */
#ifndef __PUBLIC__H__
#define __PUBLIC__H__

#include "common.h"

uint8_t UDP_public_Sum_check(uint8_t *str, uint32_t str_length);
uint16_t bswap_16(uint16_t x);
uint32_t bswap_32(uint32_t x);
void Msg_Queue_Send(Queue Msg_For_Exception_queue, uint8_t *msg_num);
uint32_t Msg_Queue_Rev(Queue Msg_For_Exception_queue, uint8_t* data);
#endif  //!__PUBLIC__H__
