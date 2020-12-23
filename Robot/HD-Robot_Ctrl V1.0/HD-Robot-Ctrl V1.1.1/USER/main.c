/*
 * @:*********************************************************************************************************: 
 * @Date: 2020-09-29 16:11:00
 * @LastEditTime: 2020-10-10 16:22:48
 * @**********************************************************************************************************: 
 */
#include "common.h"
char data[30] = {0};
char heart_data[9] = {0x48,0x44,0x00,0x00,0x00,0x00,0x4F,0x4B};
char heart_anchor_data[5] = {0x48,0x44,0xFF,0x4F,0x4B};
extern distance_SendPackets distance_SendPackets_temp;
extern DistanceOffset_Stru DistanceOffset_Stru_temp;
extern AnchorFollowCmd_Stru AnchorFollowCmd_Stru_temp;
extern Network_Manage Network_Manage_t;
DistanceOffset_Stru DistanceOffset_Formflash;
uint8_t ModeTemp;
int main(void)
{ 
	Distance_ToAngle_Stru Distance_ToAngle_Stru_temp;
	uint8_t ret = 0;
	char *temp = NULL;
	uint32_t TimeTemp;
	SCB->VTOR = FLASH_BASE | 0x10000;   // 中断向量基地址跳转
	__set_PRIMASK(0);	/* 开启全部中断 */
	
	HAL_Init();                     //初始化HAL库   
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);//中断优先级分组2	//初始化HAL库   
	Stm32_Clock_Init(360,25,2,8);   //设置时钟,180Mhz
	delay_init(180);                //初始化延时函数
	Debug_USART1_Init();           //初始化USART
	GPIO_Init_Config();
	log_print(DEBUG,("CTRL APP Initing...\n"));
	my_mem_init(SRAMIN);		    //初始化内部内存池
	my_mem_init(SRAMCCM);		    //初始化CCM内存池
	//DistanceData_QUEUE_INIT(DistanceData_Queue_Size);
	//STMFLASH_Write_test();
	IWDG_Init(IWDG_PRESCALER_64,2000);
	Upgrade_CleanWrite();
	ret = STMFLASH_NetworkPara_Config_Read();  
	if(ret != 0x01)
	{
		if((Network_Manage_t.remoteip[0] != Network_Manage_t.ip[0]) || (Network_Manage_t.remoteip[1] != Network_Manage_t.ip[1]) 
			||(Network_Manage_t.remoteip[0] == 0xFF) || (Network_Manage_t.ip[0] == 0xFF)
		  ||(Network_Manage_t.remoteip[3] == Network_Manage_t.ip[3]))
		{
			log_print(DEBUG,("network default config..."));
			udp_Network_Init();
		}
	}
	#if 1
	MX_TIM6_Init();
	TIM3_Init(1000-1,900-1);        //定时器3初始化，定时器时钟为90M，分频系数为900-1，														
	log_print(DEBUG,("lwIP Initing...\n"));
	while(lwip_comm_init())         //lwip初始化
	{
		log_print(DEBUG,("lwIP Initing fail\n Retrying...\r\n"));
		delay_ms(500);
		IWDG_Feed();
	}
	log_print(DEBUG,("lwIP Initing succes\r\n"));
	udp_create();
	ret = 0;
	ret = STMFLASH_Anchors_Distance_Read();
	if(ret != 0x01)
	{
		if((AnchorToAnchor_Distance[0] <= 0) ||(AnchorToAnchor_Distance[0] > 10000) 
			||(AnchorToAnchor_Distance[1] > 10000) || (AnchorToAnchor_Distance[2] > 10000))
		{
			log_print(DEBUG,("Anchors distance default config..."));
			AnchorToAnchor_Distance[0] = 1247;
			AnchorToAnchor_Distance[1] = 1247;
			AnchorToAnchor_Distance[2] = 1247;
		}
	}	
	log_print(DEBUG,("Anchors = %f,%f,%f\r\n",AnchorToAnchor_Distance[0],AnchorToAnchor_Distance[1],AnchorToAnchor_Distance[2]));
	#endif
	AnchorFollowCmd_Stru_temp.Follwmode = AheadFollow;
	STMFLASH_DistanceOffset_Read();
	if((DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[0].FollowOffset_Mark != 0x01)&&\
		 (DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[1].FollowOffset_Mark != 0x01)&&\
	   (DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[2].FollowOffset_Mark != 0x01))
	{
		Distance_DefaultPara_Config();
	}
	IWDG_Feed();
	while(1)
	{
		log_print( DEBUG, ("CTRL APP run...\r\n"));
		for(int i = 0; i < 5; i++)
		{
			heart_data[i + 2] = Array_ip_D[i];
		}
		udp_UWB_senddata(udppcb, (uint8_t *)heart_data, sizeof(heart_data));
		lwip_periodic_handle();
		TimeTemp = HAL_GetTick();
		while(HAL_GetTick() < (TimeTemp + 10000)) /* 判定是否有测距标志*/
		{
			if(udp_demo_flag&1<<6)//是否收到数据?
			{
				HAL_GPIO_TogglePin(LED_GPIO_PORT, LED_GPIO_PIN);
				UDP_RecvDataHandle();
				udp_demo_flag&=~(1<<6);//标记数据已经被处理了.
			} 
			lwip_periodic_handle();
			IWDG_Feed();
		}
	} 
}

/* 硬件错误处理函数 */
void _Error_Handler(char * file, int line)
{
	SYS_REBOOT();
}
