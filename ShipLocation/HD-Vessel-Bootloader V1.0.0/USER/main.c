/*
 * @:*********************************************************************************************************: 
 * @Date: 2020-09-29 16:11:00
 * @LastEditTime: 2020-10-10 16:22:48
 * @**********************************************************************************************************: 
 */
#include "common.h"


int main(void)
{   
	uint8_t ret = 0, Upgraderesponse_Flag = 1;
  static int Upgradereq_cnt = 0;
	uint32_t timetemp = 0;
	HAL_Init();                     //初始化HAL库  
  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);//中断优先级分组2	
	Stm32_Clock_Init(360,25,2,8);   //设置时钟,180Mhz
	delay_init(180);                //初始化延时函数
	LED_Status_Init();
	Debug_USART1_Init();           //初始化USART  
	my_mem_init(SRAMIN);		    //初始化内部内存池
	my_mem_init(SRAMCCM);		    //初始化CCM内存池
	I2C_EE_Init();
	EE_Network_Read();
	if((E_Network_Manage_stru_temp.Anchoripaddress[0] != E_Network_Manage_stru_temp.Gateway[0]) 
		 || (E_Network_Manage_stru_temp.Anchoripaddress[1] != E_Network_Manage_stru_temp.Gateway[1]) 
	   || (E_Network_Manage_stru_temp.Anchoripaddress[0] == 0xFF) || (E_Network_Manage_stru_temp.Anchoripaddress[1] == 0xFF))
	{
		log_print(DEBUG,("network default config...\r\n"));
		udp_Network_Init();
	}
	//E_Network_Manage_stru_temp.Centeripaddress[3] = 104;
	E_Network_Manage_stru_temp.CenterPort = 6005;
	
	TIM3_Init(1000-1,900-1);        //定时器3初始化，定时器时钟为90M，分频系数为900-1，
	MX_TIM6_Init();
	I2C_EE_Init();
	EE_upgrateFlag_Read();
	upgrateFlag = 0x01;
	if(upgrateFlag == 0x01) 
	{
		log_print(DEBUG,("lwIP Initing...\n"));
		while(lwip_comm_init())         //lwip初始化
		{
			log_print(ERR,("lwIP Initing fail\n Retrying...\r\n"));
			delay_ms(500);
		}
		log_print(DEBUG,("lwIP Initing success"));
		udp_create();
		UDP_SendUpgradeReq();
		while(Upgraderesponse_Flag)
		{
			Upgradereq_cnt++;
			if(Upgradereq_cnt > 5)
			{
				udp_demo_connection_close(udppcb); 
				BootLoad_to_App(FLASH_APP_ADDR);
				Upgradereq_cnt = 0;
				break;
			}
			timetemp = HAL_GetTick();
			while(HAL_GetTick() < (timetemp + 5000)) 
			{
				if(udp_demo_flag&1<<6)//是否收到数据?
				{		
					Upgraderesponse_Flag = 0;
					log_print(DEBUG,("response success\n jump to updataHandle...\r\n"));
					ret = UDP_RecvData_Handle();
					udp_demo_flag&=~(1<<6);//标记数据已经被处理了.
					lwip_periodic_handle();
					break;
				} 
				if(tim6_irq_flag == true)
				{
					tim6_irq_flag = false;
					HAL_GPIO_TogglePin(LED_GPIO_PORT, LED_GPIO_PIN);
				}
			}
		
			if(Upgraderesponse_Flag == 1)
			{
				log_print(DEBUG,("Upgrade requst no response\n Retrying...\r\n"));
				UDP_SendUpgradeReq();
			}
			lwip_periodic_handle();
			
			
		}
	}
	else
	{
		udp_demo_connection_close(udppcb); 
		BootLoad_to_App(FLASH_APP_ADDR);
	}
	while(1)
	{
		if(udp_demo_flag&1<<6)//是否收到数据?
		{			
			ret = UDP_RecvData_Handle();
			udp_demo_flag&=~(1<<6);//标记数据已经被处理了.
			lwip_periodic_handle();
		} 
		if(ret == 1)
		{
			ret = 0;
			udp_demo_connection_close(udppcb); 
			BootLoad_to_App(FLASH_APP_ADDR);	
		}
	} 
}

/* 硬件错误处理函数 */
void _Error_Handler(char * file, int line)
{
	SYS_REBOOT();
}
