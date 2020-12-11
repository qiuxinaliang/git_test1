#ifndef _SYS_DEBUG_H_
#define _SYS_DEBUG_H_

#include "stdio.h"
//#include "cmsis_armcc.h"

#if 0
#define SYS_REBOOT()\
 do {\
     printf("\r\nReboot device at line %d in %s\r\n", __LINE__, __FILE__);\
     __disable_fault_irq();\
     NVIC_SystemReset();\
 } while(0)
#endif

#endif
