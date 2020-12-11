/*
 * @:*********************************************************************************************************: 
 * @Date: 2020-09-29 16:11:00
 * @LastEditTime: 2020-10-10 16:22:48
 * @**********************************************************************************************************: 
 */
#include "common.h"
char data[30] = {0};
char heart_data[8] = {0x48,0x44,0x55,0x54,0x00,0x00,0x4F,0x4B};
extern distance_SendPackets distance_SendPackets_temp;
int main(void)
{   
	uint8_t ret = 0, Upgraderesponse_Flag = 1;
	uint32_t timetemp = 0;
	HAL_Init();                     //初始化HAL库   
	Stm32_Clock_Init(360,25,2,8);   //设置时钟,180Mhz
	delay_init(180);                //初始化延时函数
	Debug_USART1_Init();           //初始化USART  
	my_mem_init(SRAMIN);		    //初始化内部内存池
	my_mem_init(SRAMCCM);		    //初始化CCM内存池
	ret = STMFLASH_NetworkPara_Config_Read(); 	
	if(ret != 0x01)
	{
		log_print(DEBUG,("network default config..."));
		udp_Network_Init();
	}
	//udp_Network_Init();
	TIM3_Init(1000-1,900-1);        //定时器3初始化，定时器时钟为90M，分频系数为900-1，
	if(Dev_Upgrade_Read() == 0x01)		
	{
		log_print(DEBUG,("lwIP Initing...\n"));
		while(lwip_comm_init())         //lwip初始化
		{
			log_print(ERR,("lwIP Initing fail\n Retrying...\r\n"));
			delay_ms(500);
		}
		printf("lwIP Initing succes\r\n");
		log_print(DEBUG,("lwIP Initing succes"));
		udp_create();
		UDP_SendUpgradeReq();
		while(Upgraderesponse_Flag)
		{
			timetemp = HAL_GetTick();
			while(HAL_GetTick() < (timetemp + 5000)) /* 判定是否有测距标志*/
			{
				if(udp_demo_flag&1<<6)//是否收到数据?
				{		
					Upgraderesponse_Flag = 0;
					log_print(DEBUG,("response success\n jump to updataHandle...\r\n"));
					ret = UDP_RecvData_Handle();
					udp_demo_flag&=~(1<<6);//标记数据已经被处理了.
					break;
				} 
			}
			if(Upgraderesponse_Flag == 1)
			{
				log_print(DEBUG,("Upgrade requst no response\n Retrying...\r\n"));
				UDP_SendUpgradeReq();
			}
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
		} 
		if(ret == 1)
		{
			ret = 0;
			//Upgrade_CleanFlag();
			udp_demo_connection_close(udppcb); 
			BootLoad_to_App(FLASH_APP_ADDR);	
		}
	} 
}

/* 硬件错误处理函数 */
void _Error_Handler(char * file, int line)
{
	//udp_demo_connection_close(udppcb); 
	SYS_REBOOT();
}
