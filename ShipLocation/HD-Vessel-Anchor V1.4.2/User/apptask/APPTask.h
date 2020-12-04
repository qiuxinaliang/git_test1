/*
 * @:*********************************************************************************************************: 
 * @Date: 2020-11-04 10:26:13
 * @LastEditTime: 2020-11-19 18:34:21
 * @**********************************************************************************************************: 
 */
#ifndef __APPTASK__H__
#define __APPTASK__H__

#include "common.h"
#include "iacs_queue.h"

#define Startup_Task_PRIO    				( tskIDLE_PRIORITY  + 3 )
#define Startup_Task_STACK   				( 256 )

#define NetworkMessage_Task_PRIO    		( tskIDLE_PRIORITY  + 8 )
#define NetworkMessage_Task_STACK   		( 256 )

#define GeneralSignal_Task_PRIO    	( tskIDLE_PRIORITY  + 8 )
#define GeneralSignal_Task_STACK   	( 256 )

#define UWB_UASRT_DMA_Task_PRIO    	( tskIDLE_PRIORITY  + 8 )
#define UWB_UASRT_DMA_Task_STACK   	( 256 )

#define LoraMessage_Task_PRIO    			( tskIDLE_PRIORITY  + 8 )
#define LoraMessage_Task_STACK   			( 256 )

#define HeartPacketMessage_Task_PRIO    		( tskIDLE_PRIORITY  + 8 )
#define HeartPacketMessage_Task_STACK   		( 256 )

#define Network_RecvData_QUEUE_SIZE 	64
#define USART3_For_Lora_QUEUE_SIZE 		64
#define MSG_FOR_EXCEPTION_QUEUE_SIZE 	4

#define Network_RecvData_QUEUE_DEFINE() 	Queue Network_RecvData_queue;
extern Queue USART3_For_Lora_queue;

#define Network_RecvData_QUEUE_INIT(queue_size)\
{\
    Network_RecvData_queue.item = NULL;\
    init_queue(&Network_RecvData_queue,queue_size);\
}

#define USART3_For_Lora_QUEUE_INIT(queue_size)\
{\
    USART3_For_Lora_queue.item = NULL;\
    init_queue(&USART3_For_Lora_queue,queue_size);\
}

extern Queue Msg_For_Exception_queue; 

#define MSG_FOR_EXCEPTION_QUEUE_INIT(queue_size)\
{\
  Msg_For_Exception_queue.item = NULL;\
  init_queue(&Msg_For_Exception_queue,queue_size);\
}

extern RTC_TimeTypeDef RTC_TimeStruct;
extern RTC_DateTypeDef RTC_DateStruct;
extern uint32_t rtc_timestamp;
extern SemaphoreHandle_t CountSemaphore;//计数型信号量
extern SemaphoreHandle_t MutexSemaphore;	//互斥信号量

void startup_App_Task(void);
#endif  //!__APPTASK__H__
