/*
 * @:*********************************************************************************************************: 
 * @Date: 2020-11-04 10:26:13
 * @LastEditTime: 2020-12-04 14:20:07
 * @**********************************************************************************************************: 
 */
#include "APPTask.h"

ServerSendConfig_Stru *ServerSendConfig_Stru_temp = NULL;
ServerCtrlAnchor_Stru *ServerCtrlAnchor_Stru_temp = NULL;
Queue USART3_For_Lora_queue;
Queue Msg_For_Exception_queue;
SemaphoreHandle_t CountSemaphore;//计数型信号量
SemaphoreHandle_t MutexSemaphore;	//互斥信号量
extern UART_HandleTypeDef husart3;
extern NetworkRecvData_Stru NetworkRecvData_Stru_temp;
uint32_t rtc_timestamp = 0;

xTaskHandle	NetworkMessage_Handle;
xTaskHandle GeneralSignal_Handle;
xTaskHandle UWB_UASRT_DMA_Handle;
xTaskHandle LoraMessage_Handle;
xTaskHandle HeartPacketMessage_Handle;
//USART3_For_Lora_QUEUE_DEFINE(); 


static void NetworkMessage_Task(void * param);
static void GeneralSignal_Task(void * param);
static void UWB_UASRT_DMA_Task(void * param);
static void LoraMessage_Task(void * param);
static void HeartPacketMessage_Task(void * param);

/*
*********************************************************************************************************
*	函 数 名: startup_App_Task
*	功能说明: 任务创建函数
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void startup_App_Task(void)
{
  taskENTER_CRITICAL();
  CountSemaphore = xSemaphoreCreateCounting(255,0);	
	MutexSemaphore=xSemaphoreCreateMutex();
	xTaskCreate(NetworkMessage_Task,
				"NetworkMessage",
				NetworkMessage_Task_STACK,
				NULL,
				NetworkMessage_Task_PRIO,
				&NetworkMessage_Handle);
	
	xTaskCreate(GeneralSignal_Task,
				"GeneralSignal",
				GeneralSignal_Task_STACK,
				NULL,
				GeneralSignal_Task_PRIO,
				&GeneralSignal_Handle);
	
	/*xTaskCreate(UWB_UASRT_DMA_Task,
				"UWB_UASRT_DMA",
				UWB_UASRT_DMA_Task_STACK,
				NULL,
				UWB_UASRT_DMA_Task_PRIO,
				&UWB_UASRT_DMA_Handle);*/
	
	xTaskCreate(LoraMessage_Task,
				"LoraMessage",
				LoraMessage_Task_STACK,
				NULL,
				LoraMessage_Task_PRIO,
				&LoraMessage_Handle);

	xTaskCreate(HeartPacketMessage_Task,
				"HeartPacketMessage",
				HeartPacketMessage_Task_STACK,
				NULL,
				HeartPacketMessage_Task_PRIO,
				&HeartPacketMessage_Handle);
  taskEXIT_CRITICAL();
}
/*
*********************************************************************************************************
*	函 数 名: USART1_For_WIFI_Task
*	功能说明: USART1消息处理任务函数
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
extern RTC_HandleTypeDef RTC_Handler;
RTC_TimeTypeDef RTC_TimeStruct;
RTC_DateTypeDef RTC_DateStruct;
static void NetworkMessage_Task(void * param)
{	
	while(1)
	{ 
    Usr_NetworkRecvData_Handle();
    vTaskDelay(100);
	}
}

/*
*********************************************************************************************************
*	函 数 名: GeneralSignal_Task
*	功能说明: 异常消息出来
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
extern uint32_t uwTick;
static void GeneralSignal_Task(void * param)
{
  int rand_temp = 0;
  uint32_t UpdateTime_temp = 1604489403;

  MSG_FOR_EXCEPTION_QUEUE_INIT(MSG_FOR_EXCEPTION_QUEUE_SIZE);
	
	if((E_Staticgloabl_stru_temp.timestamp > 1604489403) && (E_Staticgloabl_stru_temp.timestamp < 4004489403))
	{
		UpdateTime_temp = E_Staticgloabl_stru_temp.timestamp;
	}
	stamp2Time_SetTimeData(UpdateTime_temp);
	srand(UpdateTime_temp);
	rand_temp = rand()%5000 + 5000;      //产生一个30到50的随机数
	vTaskDelay(rand_temp);   //PowerOnMessageReponse
	while(1)
	{
		PowerOnMessage_Handle();
		vTaskDelay(10000);
		if(Exception_Massage_Global == PowerOnMessageReponse) 
		{
			Exception_Massage_Global = 0;
			break;
		}
	}
  while(1)
  {
    DeviceUpdateTimeRequest();
		vTaskDelay(10000);
    if(Exception_Massage_Global == UpdateTimeReqReponse) 
		{
			Exception_Massage_Global = 0;
			break;
		}
  }
	while(1)
	{
		Exception_Massage_Global = ExceptionTaskSuspend;
		//log_print(DEBUG,("G_Task\r\n"));
		vTaskDelay(1000);
	}
}

/*
*********************************************************************************************************
*	函 数 名: UWB_UASRT_DMA_Task
*	功能说明: UWB消息处理
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/

/*static void UWB_UASRT_DMA_Task(void * param)
{
  char Lora_test[50] = "this is a loar communction test\r\n";
	while(1)
	{
    //xSemaphoreTake(MutexSemaphore, portMAX_DELAY);	//获取互斥信号量
    //sx127xSend((uint8_t *)Lora_test, sizeof(Lora_test), 100);//
    //xSemaphoreGive(MutexSemaphore);	 //释放信号量
		vTaskDelay(1000);
	}
}*/

/*
*********************************************************************************************************
*	函 数 名: LoraMessage_Task
*	功能说明: Lora消息处理
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/

static void LoraMessage_Task(void * param)
{	
	char Lora_test[6] = "test\r\n";
	while(1)
	{
		//sx127xSend((uint8_t *)Lora_test, sizeof(Lora_test), 100);//
		log_print(DEBUG,("L_T\r\n"));
		IWDG_Feed();
		vTaskDelay(1000);
	}
}

/*
*********************************************************************************************************
*	函 数 名: HeartPacketMessage_Task
*	功能说明: 心跳包消息异常处理
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
//extern uint32_t uwTick;
static void HeartPacketMessage_Task(void * param)
{	
	int rand_temp = 0;
	srand(E_Staticgloabl_stru_temp.timestamp);
  rand_temp = rand()%1000 + 1000;      //产生一个30到50的随机数
  vTaskDelay(rand_temp);
	// 通过上传心跳包进行开机注册
  vTaskDelay(rand_temp);
  DeviceReportHeart();
  vTaskDelay(1000);
  DeviceReportHeart();
  vTaskDelay(1000);
	DeviceReportHeart();
	
	rtc_timestamp = 1604489403;
	
	//HAL_RTC_GetTime(&RTC_Handler,&RTC_TimeStruct,RTC_FORMAT_BIN);
	//HAL_RTC_GetDate(&RTC_Handler,&RTC_DateStruct,RTC_FORMAT_BIN);
  //rtc_timestamp = time2Stamp(RTC_DateStruct, RTC_TimeStruct);
  while(1)
	{
    DeviceResponseHeart_Handle();
		vTaskDelay(1000);
	}
}

