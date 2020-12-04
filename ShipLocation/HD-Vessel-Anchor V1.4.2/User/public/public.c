/*
 * @:*********************************************************************************************************: 
 * @Date: 2020-11-04 17:03:11
 * @LastEditTime: 2020-11-19 18:09:38
 * @**********************************************************************************************************: 
 */
#include "public.h"

/**
 * @:*********************************************************************************************************: 
 * @函 数 名: UDP_public_Sum_chec
 * @功能说明: 公共求和校验函数
 * @形    参: uint8_t *str:待校验字符串，str_length:待校验长度
 * @返 回 值: uint8_t XorCheck_Value
 * @**********************************************************************************************************: 
 */
uint8_t UDP_public_Sum_check(uint8_t *str, uint32_t str_length)
{
  uint8_t SumCheck_Value = 0;
  int i = 0;
  
  SumCheck_Value = str[0];
	//printf("%02x",str[0]);
  for(i = 1; i < str_length; i++)
  {
		 //printf("%02x",str[i]);
     SumCheck_Value ^= str[i];
  }
  //log_print(DEBUG, ("\r\nXorCheck_Value = %x\r\n",  SumCheck_Value));
  return SumCheck_Value;
}

// 大小端转换
uint16_t bswap_16(uint16_t x)
{
	return (x >> 8) | (x << 8); 
}

// 大小端转换
uint32_t bswap_32(uint32_t x)
{
	x = ((x << 8) &0xFF00FF00) | ((x >> 8) &0x00FF00FF);
	return (x >> 16) | (x << 16);
}

/**
 * @:*********************************************************************************************************: 
 * @函 数 名: Msg_Queue_Send(uint8_t msg_num)
 * @功能说明: 异常消息发送
 * @形    参: uint8_t msg_num：异常消息
 * @返 回 值: 无
 * @**********************************************************************************************************: 
 */
extern Queue Msg_For_Exception_queue;
void Msg_Queue_Send(Queue Msg_For_Exception_queue, uint8_t *msg_num)
{
  //en_queue(&Msg_For_Exception_queue, (Item)msg_num); // put it into the queue
  uint32_t data_to_write = MSG_FOR_EXCEPTION_QUEUE_SIZE;

  uint8_t *ptr = (uint8_t *)msg_num;

  while(data_to_write--) {
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
uint32_t Msg_Queue_Rev(Queue Msg_For_Exception_queue, uint8_t* data)
{
	uint32_t ret = 0;
	uint32_t data_to_read = MSG_FOR_EXCEPTION_QUEUE_SIZE;

	while(data_to_read)
	{
		if(!de_queue(&Msg_For_Exception_queue, data)) {
				return ret;
		}
		data_to_read--;
		data++;
		ret++;
	}
	return ret;
}