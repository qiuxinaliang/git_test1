/*
 * @:*********************************************************************************************************: 
 * @Date: 2020-11-04 09:58:17
 * @LastEditTime: 2020-11-04 17:12:31
 * @**********************************************************************************************************: 
 */
#ifndef _COMMON_
#define _COMMON_

#include <stdio.h>
#include <stdint.h>

#include "main.h"
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "debug.h"
#include "./usart/bsp_debug_usart.h"
#include "./led/bsp_led.h" 
#include "./eth/bsp_eth.h" 
#include "udpecho.h"
#include "board.h"
#include "stmflash.h"
#include "time.h"
#include "spi.h"

#include <cm_backtrace.h>

///* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "APPTask.h"
#include "ServerToAnchor.h"
#include "public.h"
#include "rtc.h"
#include "malloc.h"	
#include "iacs_queue.h"

#include "sx127x_driver.h"
#include "sx1276-LoRa.h"
#include "sx127x_hal.h"
#include "Lora_demo.h"
#include "network.h"

#include "bsp_i2c_ee.h"

#endif // !1






