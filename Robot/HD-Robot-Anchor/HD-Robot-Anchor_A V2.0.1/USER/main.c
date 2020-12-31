/*
 * @:*********************************************************************************************************: 
 * @Date: 2020-09-29 16:11:00
 * @LastEditTime: 2020-11-30 11:09:19
 * @**********************************************************************************************************: 
 */
#include "common.h"
char heart_data[8] = {0x48,0x44,0x55,0x54,0x00,0x00,0x4F,0x4B};
extern distance_SendPackets distance_SendPackets_temp;
extern Network_Manage Network_Manage_t;
extern AnchorFollowCmd_Stru AnchorFollowCmd_Stru_temp;
extern uint8_t UWB_address_temp;
extern uint8_t address[8];
extern int AnchorMode_Flag;
int HeartTime_Cnt = 0;
int main(void)
{ 
	uint8_t ret = 0;
	SCB->VTOR = FLASH_BASE | 0x10000;   // 中断向量基地址跳转
	__set_PRIMASK(0);	/* 开启全部中断 */
	
	HAL_Init();    
  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);//中断优先级分组2	//初始化HAL库   	//初始化HAL库   
	Stm32_Clock_Init(360,25,2,8);   //设置时钟,180Mhz
	delay_init(180);                //初始化延时函数
	Debug_USART1_Init();           //初始化USART
	log_print(DEBUG,("anchor APP Initing...\n"));
  //Test_Write();
  /* 清除升级标志 */
  if(Dev_Upgrade_Read() == 0x01) 
  {
    Dev_Upgrade_Write(0);
  }
	DW_EXIT_GPIO_NVIC_Config();
	DW_SPI_Init();
  /* 默认参数配置 */
  Anchor_UpgradeFlag = 0;
  Anchor_address = 2;
  Anchor_Workmode = AheadAndLeftFollow;
  
	UWB_address_temp = UWB_AnchorID_Read();
	if((UWB_address_temp <= 0) || ((UWB_address_temp > 10)))
	{
    log_print(DEBUG,("UWB_address default config...\r\n"));
		UWB_address_temp = 2;
	}
	address[0]= UWB_address_temp;
  
	DW_Parameter_Config(dwm);
	DW_User_Justable_Config(dwm);
	my_mem_init(SRAMIN);		    //初始化内部内存池
	my_mem_init(SRAMCCM);		    //初始化CCM内存池
	//DistanceData_QUEUE_INIT(DistanceData_Queue_Size);
	ret = STMFLASH_NetworkPara_Config_Read();  
	if(ret != 0x01)
	{
    if((Network_Manage_t.ip[0] != Network_Manage_t.remoteip[0]) || (Network_Manage_t.ip[1] != Network_Manage_t.remoteip[1])
			||(Network_Manage_t.ip[0] == 0xFF) ||(Network_Manage_t.remoteip[0] == 0xFF) 
		  || (Network_Manage_t.ip[0] != Network_Manage_t.remoteip[0]) || (Network_Manage_t.ip[3] == Network_Manage_t.remoteip[3]))
    {
      log_print(DEBUG,("network default config...\r\n"));
		  udp_Network_Init();
    }
	}
	ret = 0;
	Anchor_Workmode = Dev_Workmode_Read();
	if((Anchor_Workmode < 1) || (Anchor_Workmode > 5))
	{
		log_print(DEBUG,("FollowAndLeftMode default set...\r\n"));
	  Anchor_Workmode = AheadAndLeftFollow;
	}
	AnchorFollowCmd_Stru_temp.Follwmode = AheadFollow;
  log_print(DEBUG,("Follwmode = %d\r\n",AnchorFollowCmd_Stru_temp.Follwmode));
	if((Anchor_Workmode == LeftFollow) ||(Anchor_Workmode == RightFollow))
	{
		AnchorMode_Flag = 1;
	}
	else
	{
		AnchorMode_Flag = 2;
	}
	#if 1
	MX_TIM6_Init();
	TIM3_Init(1000-1,900-1);        //定时器3初始化，定时器时钟为90M，分频系数为900-1，														//所以定时器3的频率为90M/900=100K，自动重装载为1000-1，那么定时器周期就是10ms
	log_print(DEBUG,("Anchor APP Initing...\n"));
	while(lwip_comm_init())         //lwip初始化
	{
		log_print(DEBUG,("lwIP Initing fail\n Retrying...\r\n"));
		delay_ms(500);
	}
	log_print(DEBUG,("lwIP Initing succes\r\n"));
	udp_create();
	
	heart_data[3] = Anchor_Workmode;
	heart_data[4] = address[0];
	heart_data[5] = Network_Manage_t.remoteip[3];
	
	#endif
	IWDG_Init(IWDG_PRESCALER_64,2000);  	//分频数为64,重载值为2000,溢出时间为16s	
	// 开机发送心跳包注册
	udp_UWB_senddata(udppcb, (uint8_t*)heart_data, sizeof(heart_data));
	delay_ms(50);
	udp_UWB_senddata(udppcb, (uint8_t*)heart_data, sizeof(heart_data));
  DW_TagEnableReceive_Distance(dwm);
	while(1)
	{
		//log_print( DEBUG, ("Anchor APP run...\r\n"));
		//DW_TagStateModify_Distance(dwm);
    DW_TagRecvState_Distance(dwm);
		if(HeartTime_Cnt >= 300)
		{
			HeartTime_Cnt = 0;
			heart_data[2] = AnchorFollowCmd_Stru_temp.Follwmode;
			udp_UWB_senddata(udppcb, (uint8_t*)heart_data, sizeof(heart_data));
		}
		IWDG_Feed();
	} 
}

/* 硬件错误处理函数 */
void _Error_Handler(char * file, int line)
{
	//udp_demo_connection_close(udppcb); 
	SYS_REBOOT();
}
