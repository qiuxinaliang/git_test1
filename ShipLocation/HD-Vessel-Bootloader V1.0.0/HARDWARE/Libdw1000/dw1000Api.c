#include "dw1000Api.h"

/* UWB通信参数配置结构体变量定义 */
dwDevice_t dwm_device;
/* 定义结构体变量指针 */
dwDevice_t *dwm = &dwm_device;
distance_SendPackets distance_SendPackets_temp;

/*
*******************************************************************************
*	函 数 名: DW_Parameter_Config
*	功能说明: DW的基础参数配置，以及MCU与DW1000的通信测试
*	形	  参: dwDevice_t * dev ：通信参数配置结构体
*	返 回 值: 无
*******************************************************************************
*/
void DW_Parameter_Config(dwDevice_t * dev)
{
	int result=0;
	bool selftestPasses = true;
	
	printf("DW1000_Parameter_Config...\n ");
	dwInit(dev, &dwOps);
	dwOpsInit(dev);
	result = dwConfigure(dev); 
	if (result == 0) 
	{
		printf("[OK]\r\n");
		/* 通信收发状态指示 + PA使能配置 */
		//dwEnableAllLeds(dev);
	} 
	else
	{
		printf("[ERROR]: %s\r\n", dwStrError(result));
		selftestPasses = false;
	}
	if (!selftestPasses) 
	{
		printf("TEST\t: One or more self-tests failed, blocking startup!\r\n");
		//while(1);
	}
}

/*
*******************************************************************************
*	函 数 名: DW_User_Justable_Config
*	功能说明: DW通信过程中，用户可自定义的配置，
*	形	  参: dwDevice_t * dev ：通信参数配置结构体
*	返 回 值: 无
*******************************************************************************
*/
void DW_User_Justable_Config(dwDevice_t * dev)
{
	dwTime_t delay = {.full = ANTENNA_DELAY/2};//delay =16475.679
	dwSetAntenaDelay(dev, delay);

	dwAttachSentHandler(dev, txcallback);
	dwAttachReceivedHandler(dev, rxcallback);

	dwNewConfiguration(dev);
	dwSetDefaults(dev);

	dwEnableMode(dwm,MODE_SHORTDATA_FAST_ACCURACY);	

	//dwEnableMode(dev,MODE_LONGDATA_RANGE_LOWPOWER);	

	dwSetChannel(dev, CHANNEL_2);
	dwSetPreambleCode(dev, PREAMBLE_CODE_64MHZ_9);

	dwCommitConfiguration(dev);
	// Initialize the packet in the TX buffer
	MAC80215_PACKET_INIT(txPacket, MAC802154_TYPE_DATA);
	txPacket.pan = 0xbccf;
	
	dwSetLNA_PA_Mode(dev, DWT_LNA_ENABLE | DWT_PA_ENABLE);
	
}

/*
*******************************************************************************
*	函 数 名: DW_AnchorToTag_Distance
*	功能说明: UWB测距，用户入口函数
*	形	  参: dwDevice_t * dev ：通信参数配置结构体
*	返 回 值: 无
*******************************************************************************
*/
extern uint8_t address[8];
extern uint8_t base_address[8];
extern volatile uint8_t curr_seq;
extern uint32_t timeRangingComplete;
extern uint8_t rangingComplete;
extern uint8_t Distance_Msg_buff[9];
void DW_AnchorToTag_Distance(dwDevice_t * dev)
{
	distance_SendPackets distance_SendPackets_temp = {0};
	int k = 0;
	unsigned int DistanceBuff = 0;
	for (k = 2; k < 4; k++) 
	{
		timeRangingComplete = HAL_GetTick();
		base_address[0] = k;
		dwIdle(dev);
		txPacket.payload[0] = POLL;
		txPacket.payload[1] = ++curr_seq;

		memcpy(txPacket.sourceAddress, address, 8);
		memcpy(txPacket.destAddress, base_address, 8);

		dwNewTransmit(dev);
		dwSetData(dev, (uint8_t*)&txPacket, MAC802154_HEADER_LENGTH + 2);

		dwWaitForResponse(dev, true);
		dwStartTransmit(dev); 
		
		while((HAL_GetTick()<(timeRangingComplete+rangingOverTime)));
		
		if(k == 2)
		{
			distance_SendPackets_temp.A_id_tag = rxPacket.sourceAddress[0];
	    distance_SendPackets_temp.A_distance = (unsigned int)(distance*1000);
		}
		else if(k == 3)
		{
			distance_SendPackets_temp.B_id_tag = rxPacket.sourceAddress[0];
	    distance_SendPackets_temp.B_distance = (unsigned int)(distance*1000);
		}
	}
	
	distance_SendPackets_temp.id_anchor = address[0];
	distance_SendPackets_temp.header = 0x4448;
	distance_SendPackets_temp.ender = 0x4225;
	distance_SendPackets_temp.reserved = 0;
	if(rangingComplete == 1)
	{
		rangingComplete = 0;
		USART1_For_Bytes_tx(sizeof(distance_SendPackets), (uint8_t *)&distance_SendPackets_temp);
	
		dwIdle(dev);
		memcpy((uint8_t *)&txPacket.payload,(uint8_t *)&distance_SendPackets_temp,sizeof(distance_SendPackets));
		memcpy(txPacket.sourceAddress, address, 8);
		base_address[0] = 254;
		memcpy(txPacket.destAddress, base_address, 8);

		dwNewTransmit(dev);
		dwSetData(dev, (uint8_t*)&txPacket, MAC802154_HEADER_LENGTH + 19);

		dwWaitForResponse(dev, true);
		dwStartTransmit(dev); 
		memset((uint8_t *)&txPacket.payload, 0, sizeof(txPacket.payload));
		memset((uint8_t *)&distance_SendPackets_temp, 0, sizeof(distance_SendPackets));
	}
}

/*
*******************************************************************************
*	函 数 名: DW_TagEnableReceive_Distance
*	功能说明: 在标签模式下，使能接收
*	形	  参: dwDevice_t * dev ：通信参数配置结构体
*	返 回 值: 无
*******************************************************************************
*/
void DW_TagEnableReceive_Distance(dwDevice_t* dev)
{
	dwNewReceive(dwm);
  dwSetDefaults(dwm);
  dwStartReceive(dwm);
}

/*
*******************************************************************************
*	函 数 名: DW_TagStateExchang_Distance
*	功能说明: 标签的不同工作模式的切换，主要为：工作模式和监测模式
*	形	  参: dwDevice_t * dev ：通信参数配置结构体
*	返 回 值: 无
*******************************************************************************
*/
TagWorkingMode DW_TagWorkingMode;
int ModeTemp = 0;
#define DWTime_WaitingForRangingFlag 2000 
void DW_TagStateModify_Distance(dwDevice_t* dev)
{
	uint32_t TimeTemp = 0;
	int Distance_Count = 0;
	/* 对标签模式的判定 */
	switch(ModeTemp)
	{
		case Idle_mode:
		{
			printf("Idle_mode\n");
			DW_TagEnableReceive_Distance(dev);
			TimeTemp = HAL_GetTick();
			while(HAL_GetTick() < (TimeTemp + DWTime_WaitingForRangingFlag * 5)) /* 判定是否有测距标志*/
			{
				if(rangingComplete == 1)
				{
					rangingComplete = 0;
					ModeTemp = Receiving_mode;
					break;
				}
				else
				{
					ModeTemp = Monitoring_mode;
				}
			}
			break;
		}
		case Receiving_mode:
		{
			/* 使能标签接收 */
			printf("Receiving_mode\n");
			DW_TagEnableReceive_Distance(dev);
			TimeTemp = HAL_GetTick();
			while(HAL_GetTick() < (TimeTemp + DWTime_WaitingForRangingFlag * 5)) /* 判定是否有测距标志*/
			{
				if(rangingComplete == 1)
				{
					rangingComplete = 0;
					//HAL_GPIO_TogglePin(DW_LED_GPIO_PORT, DW_LED_PIN);
					//dwStopReceive(dwm); 
					//HAL_Delay(500);
					//DW_TagEnableReceive_Distance(dwm);
					Distance_Count++;
				}
				UWB_DistanceToAngle();
			}
			if(Distance_Count >= 2)
			{
				Distance_Count = 0;
				ModeTemp = Receiving_mode;
			}
			else
			{
				ModeTemp = Monitoring_mode;
			}
			break;
			
		}
		case Monitoring_mode:
		{
			printf("Monitoring_mode\n");
			dwIdle(dwm);
			dwStopReceive(dwm); 
			TimeTemp = HAL_GetTick();
			while(HAL_GetTick() < (TimeTemp + DWTime_WaitingForRangingFlag * 10)); 
			DW_TagEnableReceive_Distance(dwm);
			TimeTemp = HAL_GetTick();
			while(HAL_GetTick() < (TimeTemp + DWTime_WaitingForRangingFlag*4))
			{
				if(rangingComplete == 1)
				{
					rangingComplete = 0;
					ModeTemp = Receiving_mode;
					break;
				}
				else
				{
					ModeTemp = Monitoring_mode;
				}
			}
			break;
		}
	}
}
