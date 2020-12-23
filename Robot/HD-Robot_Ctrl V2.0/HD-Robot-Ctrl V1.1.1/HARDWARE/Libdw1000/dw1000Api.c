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
	
	log_print(DEBUG,("DW1000_Parameter_Config...\n "));
	dwInit(dev, &dwOps);
	dwOpsInit(dev);
	result = dwConfigure(dev); 
	if (result == 0) 
	{
		log_print(DEBUG,("[OK]\r\n"));
		/* 通信收发状态指示 + PA使能配置 */
		//dwEnableAllLeds(dev);
	} 
	else
	{
		log_print(ERR,("[ERROR]: %s\r\n", dwStrError(result)));
		selftestPasses = false;
	}
	if (!selftestPasses) 
	{
		log_print(ERR,("TEST\t: One or more self-tests failed, blocking startup!\r\n"));
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
	dwTime_t delay = {.full = ANTENNA_OFFSET/2};//delay =16475.679
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
			log_print(DEBUG,("Idle_mode\n"));
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
				if(udp_demo_flag&1<<6)
				{
					UDP_RecvDataHandle();
					udp_demo_flag&=~(1<<6);
					if(ModeTemp == Calibration_mode) break;
				}
				lwip_periodic_handle();
				if(ModeTemp_Flag == 1) 
				{
					ModeTemp = Calibration_mode;
					break;
				}
			}
			if(ModeTemp_Flag == 1) 
			{
				ModeTemp_Flag = 0;
				ModeTemp = Calibration_mode;
			}
			udp_UWB_senddata(udppcb, (uint8_t*)heart_data, sizeof(heart_data));
			break;
		}
		case Receiving_mode:
		{
			/* 使能标签接收 */
			log_print(DEBUG,("Receiving_mode\n"));
			DW_TagEnableReceive_Distance(dev);
			TimeTemp = HAL_GetTick();
			while(HAL_GetTick() < (TimeTemp + DWTime_WaitingForRangingFlag * 5)) /* 判定是否有测距标志*/
			{
				if(rangingComplete == 1)
				{
					rangingComplete = 0;
					Distance_Count++;
				}
				if(udp_demo_flag&1<<6)
				{
					UDP_RecvDataHandle();
					udp_demo_flag&=~(1<<6);
					if(ModeTemp == Calibration_mode) break;
				}
				lwip_periodic_handle();
				//UWB_DistanceToAngle();
				//delay_ms(100);
				if((ModeTemp_Flag == 1) ||((ModeTemp_Flag == 2))) break;
			}
			if(ModeTemp_Flag == 1) 
			{
				dwIdle(dwm);
				dwStopReceive(dwm); 
				ModeTemp_Flag = 0;
				ModeTemp = Calibration_mode; 
				break;
			}
			else if(ModeTemp_Flag == 2)
			{
				dwIdle(dwm);
				dwStopReceive(dwm); 
				ModeTemp_Flag = 0;
				ModeTemp = AntDelayCalibration_mode; 
				break;
			}
			udp_UWB_senddata(udppcb, (uint8_t*)heart_data, sizeof(heart_data));
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
			log_print(DEBUG,("Monitoring_mode\n"));
			dwIdle(dwm);
			dwStopReceive(dwm); 
			TimeTemp = HAL_GetTick();
			while(HAL_GetTick() < (TimeTemp + DWTime_WaitingForRangingFlag * 5))
			{
				if(udp_demo_flag&1<<6)
				{
					UDP_RecvDataHandle();
					udp_demo_flag&=~(1<<6);
					if(ModeTemp == Calibration_mode) break;
				}
				lwip_periodic_handle();
			}
			udp_UWB_senddata(udppcb, (uint8_t*)heart_data, sizeof(heart_data));
			TimeTemp = HAL_GetTick();
			while(HAL_GetTick() < (TimeTemp + DWTime_WaitingForRangingFlag * 5))
			{
				if(udp_demo_flag&1<<6)
				{
					UDP_RecvDataHandle();
					udp_demo_flag&=~(1<<6);
					if(ModeTemp == Calibration_mode) break;
				}
				lwip_periodic_handle();
			}
			
			udp_UWB_senddata(udppcb, (uint8_t*)heart_data, sizeof(heart_data));
			DW_TagEnableReceive_Distance(dwm);
			TimeTemp = HAL_GetTick();
			while(HAL_GetTick() < (TimeTemp + DWTime_WaitingForRangingFlag*5))
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
				if(udp_demo_flag&1<<6)
				{
					UDP_RecvDataHandle();
					udp_demo_flag&=~(1<<6);
					if(ModeTemp == Calibration_mode) break;
				}
				lwip_periodic_handle();
			}
			udp_UWB_senddata(udppcb, (uint8_t*)heart_data, sizeof(heart_data));
			break;
		}
		case Calibration_mode:
		{
			log_print(DEBUG,("Calibration_mode\n"));
			ModeTemp = Calibration_mode;
			DW_TagEnableReceive_Distance(dwm);
			TimeTemp = HAL_GetTick();
			while(HAL_GetTick() < (TimeTemp + DWTime_WaitingForRangingFlag*2))
			{
				//UWB_DistanceToAngle();
				if(udp_demo_flag&1<<6)
				{
					UDP_RecvDataHandle();
					udp_demo_flag&=~(1<<6);
				}
				lwip_periodic_handle();
			}
			udp_UWB_senddata(udppcb, (uint8_t*)heart_data, sizeof(heart_data));
			break;
		}
		case AntDelayCalibration_mode:
		{
			log_print(DEBUG,("AntDelayCalibration_mode\n"));
			ModeTemp = AntDelayCalibration_mode;
			delay_ms(2000);
			dwIdle(dwm);
			DW_TagEnableReceive_Distance(dwm);
			TimeTemp = HAL_GetTick();
			while(HAL_GetTick() < (TimeTemp + DWTime_WaitingForRangingFlag*5))
			{
				DW_AntDelayCali_Handle();
				if(udp_demo_flag&1<<6)
				{
					UDP_RecvDataHandle();
					udp_demo_flag&=~(1<<6);
				}
				lwip_periodic_handle();
			}
			udp_UWB_senddata(udppcb, (uint8_t*)heart_data, sizeof(heart_data));
			
			if(ant_delay_calibration_completed_flag == 1)
			{
				dwIdle(dwm);
				dwStopReceive(dwm); 
				ModeTemp = Receiving_mode;
			}
			break;
		}
		default: break;
			
	}
}


/*
*******************************************************************************
*	函 数 名: DW_AntDelayCali_Handle
*	功能说明: 天线的延时校准
*	形	  参: dwDevice_t * dev ：通信参数配置结构体
*	返 回 值: 无
*******************************************************************************
*/
int32_t Distance_AntDelayValue = 0;
uint8_t ant_delay_calibration_completed_flag = 0;
void DW_AntDelayCali_Handle(void)
{
	static int32_t  TX_RX_delay_offset = 0;
	uint64_t TX_RX_delay_time = 0;
	uint32_t DW_STD_Distance_Value = 0, local_Distance_Temp = 0;
	static uint32_t Distance_Value_Temp = 0;
	static int UWB_distance_count = 0;
	static int UWB_distance_diff_count = 0;
	static int UWB_distance_diff_cnt = 0;
	DW_STD_Distance_Value = Distance_AntDelay;
	while(ant_delay_calibration_completed_flag == 0)
	{
		if(rangingComplete == 1)
		{
			rangingComplete = 0;
			//log_print(DEBUG,("AntDelayCali\n"));
			log_print(DEBUG,("local_Anchor_Distance = %d\n", local_Anchor_Distance));
			UWB_distance_count++;
			local_Distance_Temp = local_Anchor_Distance + TX_RX_delay_offset;
			if(abs((local_Distance_Temp - DW_STD_Distance_Value)) >= 50)
			{
				Distance_Value_Temp = Distance_Value_Temp + local_Distance_Temp;
				local_Distance_Temp = 0;
				UWB_distance_diff_count++;
			}
			else
			{
				UWB_distance_diff_cnt++;	/* 误差小于5cm次数统计 */
			}
			/* 如果10次测距中有5次误差大于5cm,则调整延时值 */
			if((UWB_distance_count == 10) && (UWB_distance_diff_count >= 5))
			{
				/* 进入校准模式 */
				UWB_distance_diff_cnt = 0;
				UWB_distance_count = 0;
				UWB_distance_diff_count = 0;
				Distance_Value_Temp = Distance_Value_Temp/10;
				if((Distance_Value_Temp - DW_STD_Distance_Value) > 10)
				{
					TX_RX_delay_offset = TX_RX_delay_offset + 10;
				}
				else
				{
					TX_RX_delay_offset = TX_RX_delay_offset - 10;
				}
				log_print(DEBUG,("Distance_Value_Temp  = %d\n", Distance_Value_Temp ));
				Distance_Value_Temp  = 0;
				ModeTemp = AntDelayCalibration_mode;
			}
			/* 如果10测测量以内，有超过5测测量误差小于5cm */
			else if((UWB_distance_count < 10) && (UWB_distance_diff_cnt >= 5))
			{
				ModeTemp = Receiving_mode;
				ant_delay_calibration_completed_flag = 1;
				STMFLASH_DistanceAntDelay_Write(TX_RX_delay_offset);
				log_print(DEBUG,("TX_RX_delay_offset = %d\n", TX_RX_delay_offset));
				TX_RX_delay_offset = 0;
				//break;
			}
		}
	}
}



