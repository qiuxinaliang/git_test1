/*
 * @:*********************************************************************************************************: 
 * @Date: 2020-11-04 17:11:36
 * @LastEditTime: 2020-11-13 14:56:55
 * @**********************************************************************************************************: 
 */
#include "rtc.h"

RTC_HandleTypeDef RTC_Handler;  //RTC句柄

/*
*********************************************************************************************************
*	函 数 名: RTC_Set_Time
*	功能说明: RTC时间设置
*	形    参: hour,min,sec:小时,分钟,秒钟
*	返 回 值: SUCEE(1),成功，ERROR(0),进入初始化模式失败 
*********************************************************************************************************
*/
HAL_StatusTypeDef RTC_Set_Time(uint8_t hour,uint8_t min,uint8_t sec,uint8_t ampm)
{
	RTC_TimeTypeDef RTC_TimeStructure;
	
	RTC_TimeStructure.Hours=hour;
	RTC_TimeStructure.Minutes=min;
	RTC_TimeStructure.Seconds=sec;
	RTC_TimeStructure.TimeFormat=ampm;
	RTC_TimeStructure.DayLightSaving=RTC_DAYLIGHTSAVING_NONE;
    RTC_TimeStructure.StoreOperation=RTC_STOREOPERATION_RESET;
	return HAL_RTC_SetTime(&RTC_Handler,&RTC_TimeStructure,RTC_FORMAT_BIN);	
}

/*
*********************************************************************************************************
*	函 数 名: RTC_Set_Date
*	功能说明: RTC日期设置
*	形    参: year,month,date:年(0~99),月(1~12),日(0~31)，星期(1~7,0,非法!)
*	返 回 值: SUCEE(1),成功，ERROR(0),进入初始化模式失败 
*********************************************************************************************************
*/
HAL_StatusTypeDef RTC_Set_Date(uint8_t year,uint8_t month,uint8_t date,uint8_t week)
{
	RTC_DateTypeDef RTC_DateStructure;
    
	RTC_DateStructure.Date=date;
	RTC_DateStructure.Month=month;
	RTC_DateStructure.WeekDay=week;
	RTC_DateStructure.Year=year;
	return HAL_RTC_SetDate(&RTC_Handler,&RTC_DateStructure,RTC_FORMAT_BIN);
}

/*
*********************************************************************************************************
*	函 数 名:RTC_Init
*	功能说明: RTC初始化
*	形    参: 无
*	返 回 值: 返回值:0,初始化成功;2,进入初始化模式失败;
*********************************************************************************************************
*/
uint8_t RTC_Init(void)
{      
  RTC_Handler.Instance = RTC;
  RTC_Handler.Init.HourFormat = RTC_HOURFORMAT_24;    //RTC设置为24小时格式 
  RTC_Handler.Init.AsynchPrediv = 0X7F;                 //RTC异步分频系数(1~0X7F)
  RTC_Handler.Init.SynchPrediv = 0XFF;                  //RTC同步分频系数(0~7FFF)   
  RTC_Handler.Init.OutPut = RTC_OUTPUT_DISABLE;     
  RTC_Handler.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  RTC_Handler.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if(HAL_RTC_Init(&RTC_Handler) != HAL_OK) return 2;
  if(HAL_RTCEx_BKUPRead(&RTC_Handler,RTC_BKP_DR0) != 0X5050)//是否第一次配置
  { 
		RTC_Set_Time(17,57,56,RTC_HOURFORMAT_24);	        //设置时间 ,根据实际时间修改
    //RTC_Set_Time(17,57,56,RTC_HOURFORMAT12_PM);	        //设置时间 ,根据实际时间修改
    RTC_Set_Date(20,11,4,3);		                    //设置日期
    HAL_RTCEx_BKUPWrite(&RTC_Handler,RTC_BKP_DR0,0X5050);//标记已经初始化过了
  }
  return 0;
}

/*
*********************************************************************************************************
*	函 数 名: HAL_RTC_MspInit
*	功能说明: RTC底层驱动，时钟配置，此函数会被HAL_RTC_Init()调用
*	形    参: hrtc:RTC句柄
*	返 回 值: 无
*********************************************************************************************************
*/
void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc)
{
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

  __HAL_RCC_PWR_CLK_ENABLE();//使能电源时钟PWR
  HAL_PWR_EnableBkUpAccess();//取消备份区域写保护

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI;    //LSE配置
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  //RCC_OscInitStruct.LSEState = RCC_LSE_OFF;                     //RTC使用LSE
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC; //外设为RTC
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI; //RTC时钟源为LSE
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
      
  __HAL_RCC_RTC_ENABLE();                                        //RTC时钟使能
}

/*
*********************************************************************************************************
*	函 数 名: time2Stamp
*	功能说明: 时间 + 日期 转换为时间戳
*	形    参: RTC_DateTypeDef date：日期句柄 ，RTC_TimeTypeDef time：时间句柄
*	返 回 值: 时间戳
*********************************************************************************************************
*/
const uint8_t Days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
const uint16_t monDays[12] = {0,31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
uint32_t time2Stamp(RTC_DateTypeDef date, RTC_TimeTypeDef time)    //北京时间转时间戳
{
	u32 result;
	uint16_t Year=date.Year+2000;
	result = (Year - 1970) * 365 * 24 * 3600 + (monDays[date.Month-1] + date.Date - 1) * 24 * 3600 + (time.Hours-8) * 3600 + time.Minutes * 60 + time.Seconds;
	//printf("[%u]",result);
	result += (date.Month>2 && (Year % 4 == 0) && (Year % 100 != 0 || Year % 400 == 0))*24*3600;	//闰月
	//printf("[%u]",result);
	Year -= 1970;
	result += (Year/4 - Year/100 + Year/400 + 1)*24 * 3600;		//闰年
	//printf("[%u]",result);
	return result;
}
 
/*
*********************************************************************************************************
*	函 数 名: stamp2Time
*	功能说明: 时间戳转换为时间 + 日期 ，定进行时间和日期冲设置
*	形    参: uint32_t stamp：时间戳
*	返 回 值: 时间戳
*********************************************************************************************************
*/
void stamp2Time(uint32_t stamp)        //时间戳转北京时间
{
	RTC_DateTypeDef date;
	RTC_TimeTypeDef time;
  uint32_t days; 
  uint16_t leap_num;
	
    time.Seconds = stamp % 60;
    stamp /= 60;	//获取分
    time.Minutes = stamp % 60;
    stamp += 8 * 60 ;
    stamp /= 60;	//获取小时
    time.Hours = stamp % 24;
    days = stamp / 24;
    leap_num = (days + 365) / 1461;
    if( ((days + 366) % 1461) == 0) 
    {
        date.Year = (days / 366)+ 1970 -2000;
        date.Month = 12;
		date.Date = 31;  
    } else
	{
		days -= leap_num; 
		date.Year = (days / 365) + 1970 -2000;
		days %= 365;
		days += 1;
		if(((date.Year%4) == 0 ) && (days==60))
		{
			date.Month = 2; 
			date.Date = 29; 
		}else
		{ 
			if(((date.Year%4) == 0 ) && (days > 60)) --days;
			for(date.Month = 0;Days[date.Month] < days;date.Month++) 
			{ 
				days -= Days[date.Month]; 
			} 
			++date.Month;
			date.Date = days;
		}
	}
	//RTC_Set_Time(time.Hours,time.Minutes,time.Seconds,RTC_HOURFORMAT12_PM);	
	//RTC_Set_Date(date.Year,date.Month,date.Date,1);	//year,month,date,week:(1~7)
}


/*
*********************************************************************************************************
*	函 数 名: stamp2Time_SetTimeData
*	功能说明: 时间戳转换为时间 + 日期 ，定进行时间和日期冲设置
*	形    参: uint32_t stamp：时间戳
*	返 回 值: 无
*********************************************************************************************************
*/
void stamp2Time_SetTimeData(uint32_t stamp)        //时间戳转北京时间
{
	RTC_DateTypeDef date;
	RTC_TimeTypeDef time;
    uint32_t days; 
    uint16_t leap_num;
	
    time.Seconds = stamp % 60;
    stamp /= 60;	//获取分
    time.Minutes = stamp % 60;
    stamp += 8 * 60 ;
    stamp /= 60;	//获取小时
    time.Hours = stamp % 24;
    days = stamp / 24;
    leap_num = (days + 365) / 1461;
    if( ((days + 366) % 1461) == 0) 
    {
        date.Year = (days / 366)+ 1970 -2000;
        date.Month = 12;
		date.Date = 31;  
    } else
		{
		days -= leap_num; 
		date.Year = (days / 365) + 1970 -2000;
		days %= 365;
		days += 1;
		if(((date.Year%4) == 0 ) && (days==60))
		{
			date.Month = 2; 
			date.Date = 29; 
		}else
		{ 
			if(((date.Year%4) == 0 ) && (days > 60)) --days;
			for(date.Month = 0;Days[date.Month] < days;date.Month++) 
			{ 
				days -= Days[date.Month]; 
			} 
			++date.Month;
			date.Date = days;
		}
	}
	
	RTC_Set_Time(time.Hours,time.Minutes,time.Seconds,RTC_HOURFORMAT_24);	
	RTC_Set_Date(date.Year,date.Month,date.Date,1);	//year,month,date,week:(1~7)
}





