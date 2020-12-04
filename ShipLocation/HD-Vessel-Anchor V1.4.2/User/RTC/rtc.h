/*
 * @:*********************************************************************************************************: 
 * @Date: 2020-11-04 17:11:18
 * @LastEditTime: 2020-11-13 14:57:08
 * @**********************************************************************************************************: 
 */
#ifndef __RTC__H__
#define __RTC__H__

#include "common.h"

HAL_StatusTypeDef RTC_Set_Time(uint8_t hour,uint8_t min,uint8_t sec,uint8_t ampm);
HAL_StatusTypeDef RTC_Set_Date(uint8_t year,uint8_t month,uint8_t date,uint8_t week);
uint8_t RTC_Init(void);
void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc);
uint32_t time2Stamp(RTC_DateTypeDef date, RTC_TimeTypeDef time);    //北京时间转时间戳
void stamp2Time(uint32_t stamp);        //时间戳转北京时间
void stamp2Time_SetTimeData(uint32_t stamp);
#endif  //!__RTC__H__
