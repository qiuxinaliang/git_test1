#ifndef _COMMON_H_
#define _COMMON_H_

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
//#include "sys_Debug.h"
#include "sys.h"
#include "delay.h"

#include "usart.h"
#include "timer.h"
#include "malloc.h"
#include "lwip/netif.h"
#include "lwip_comm.h"
#include "lwipopts.h"
#include "udp_demo.h"
#include "httpd.h"

#include "stm32f4xx.h"
#include "spi.h"
#include "libdw1000Spi.h"
#include "gpio.h"
#include "stmflash.h"

#include "dwParameter.h"
#include "dwOps.h"
#include "dw1000Api.h"
#include "dw1000.h"
#include <libdw1000.h>
#include "mac.h"
#include "libdw1000Reg.h"

#include "myqueue.h"
#include "DistanceHandle.h"

extern char heart_data[8];
void _Error_Handler(char *, int);
#define Error_Handler() _Error_Handler(__FILE__, __LINE__)

#endif
