#ifndef _TIMER_H
#define _TIMER_H
//#include "sys.h"
#include "common.h"

extern uint8_t tim6_irq_flag;
void TIM3_Init(u16 arr,u16 psc);
void MX_TIM6_Init(void);
#endif

