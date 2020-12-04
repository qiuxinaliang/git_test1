/*
 * @:*********************************************************************************************************: 
 * @Date: 2020-11-03 18:07:17
 * @LastEditTime: 2020-11-10 15:16:44
 * @**********************************************************************************************************: 
 */
#include "main.h"

/* FreeRTOS头文件 */
static TaskHandle_t AppTaskCreate_Handle = NULL;  /* 创建任务句柄 */
static void AppTaskCreate(void);                  /* 用于创建任务 */
extern void TCPIP_Init(void);

/*
*********************************************************************************************************
*	函 数 名: main
*	功能说明: 主函数,第一步：开发板硬件初始化, 第二步：创建APP应用任务, 第三步：启动FreeRTOS，开始多任务调度
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
int main(void)
{	
	SCB->VTOR = FLASH_BASE | 0x10000;   // 中断向量基地址跳转
	__set_PRIMASK(0);                 	/* 开启全部中断 */
  BaseType_t xReturn = pdPASS;        /* 定义一个创建信息返回值，默认为pdPASS */
  BSP_Init();                         /* 开发板硬件初始化 */
  
  /* 创建AppTaskCreate任务 */
  xReturn = xTaskCreate((TaskFunction_t)AppTaskCreate, (const char*)"AppTaskCreate", (uint16_t)512,
                       (void*)NULL, (UBaseType_t)1, (TaskHandle_t*)&AppTaskCreate_Handle);                                  
  if(pdPASS == xReturn)
    vTaskStartScheduler();   /* 启动任务，开启调度 */
	else
    return -1;   
  
  while(1);   /* 正常不会执行到这里 */    
}

/*
*********************************************************************************************************
*	函 数 名:  AppTaskCreate
*	功能说明: 为了方便管理，所有的任务创建函数都放在这个函数里面
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/ 
static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;        /* 定义一个创建信息返回值，默认为pdPASS */
  
  TCPIP_Init();
  udpecho_init();
  taskENTER_CRITICAL();           //进入临界区
  startup_App_Task();
  vTaskDelete(AppTaskCreate_Handle); //删除AppTaskCreate任务
  taskEXIT_CRITICAL();            //退出临界区
}

