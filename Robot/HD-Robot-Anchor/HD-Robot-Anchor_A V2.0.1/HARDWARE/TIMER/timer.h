#ifndef _TIMER_H
#define _TIMER_H
//#include "sys.h"
#include "common.h"

void TIM3_Init(u16 arr,u16 psc);
void MX_TIM6_Init(void);
void IWDG_Init(u8 prer,u16 rlr);
void IWDG_Feed(void);
#endif

