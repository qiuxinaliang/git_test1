/*
 * @:*********************************************************************************************************: 
 * @Date: 2020-11-04 15:41:09
 * @LastEditTime: 2020-12-04 14:24:55
 * @**********************************************************************************************************: 
 */
#include "ServerToAnchor.h"
#include "public.h"

NetworkRecvData_Stru NetworkRecvData_Stru_temp;
/*
*********************************************************************************************************
*	函 数 名: PowerOnMessage_Handle
*	功能说明: 开机信息上报
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
extern RTC_HandleTypeDef RTC_Handler;
extern uint8_t DeviceSelfCheckInfo;
void PowerOnMessage_Handle(void)
{
  DevicePowerOn_Stru DevicePowerOn_Stru_temp;
  memset((uint8_t *)&DevicePowerOn_Stru_temp, 0, sizeof(DevicePowerOn_Stru));
  DevicePowerOn_Stru_temp.ProtocolFrameCtrlUint_Temp.ProtocolStartID = UsrProtocolStartID;
  DevicePowerOn_Stru_temp.ProtocolFrameCtrlUint_Temp.ProtocolType = bswap_16(UsrProtocolType);
  DevicePowerOn_Stru_temp.ProtocolFrameCtrlUint_Temp.DeviceAddress = *(uint32_t*)(UsrDeviceAddress);
  DevicePowerOn_Stru_temp.ProtocolFrameCtrlUint_Temp.DeviceType = UsrDeviceType;
  DevicePowerOn_Stru_temp.ProtocolFrameCtrlUint_Temp.DataLen = sizeof(DevicePowerOn_Stru) - sizeof(ProtocolFrameCtrlUint) - sizeof(ProtocolFrameEnderUint);
  DevicePowerOn_Stru_temp.ProtocolFrameCtrlUint_Temp.cmd_Unit = Cmd_SendDeviceArrti;
  DevicePowerOn_Stru_temp.AttributePairsNum = 0x08;
  DevicePowerOn_Stru_temp.SoftwareVersionID = SoftwareVersionID;
  memcpy((uint8_t *)&DevicePowerOn_Stru_temp.SoftwareVersion, (uint8_t *)&E_Version_stru_temp.SoftwareVersion, 3);
  DevicePowerOn_Stru_temp.HardwareVersionID =  HardwareVersionID;
  memcpy((uint8_t *)&DevicePowerOn_Stru_temp.HardwareVersion, (uint8_t *)&E_Version_stru_temp.HardwareVersion, 3);
  DevicePowerOn_Stru_temp.Reserved1ID = BatteryLevelID;
  DevicePowerOn_Stru_temp.Reserved1 = 0x00;
  DevicePowerOn_Stru_temp.Reserved2ID = BatteryVoltageID;
  DevicePowerOn_Stru_temp.Reserved2 = 0x00;
  DevicePowerOn_Stru_temp.PowerOnTypeID = PowerOnTypeID;    // 
  DevicePowerOn_Stru_temp.PowerOnType = E_PowerOnType;
  DevicePowerOn_Stru_temp.TimeStampID =  ReportTimeStampID;
  DevicePowerOn_Stru_temp.TimeStamp = rtc_timestamp;
  DevicePowerOn_Stru_temp.SelfCheckID = SelfCheckID;
  DevicePowerOn_Stru_temp.SelfCheck= DeviceSelfCheckInfo;
  DevicePowerOn_Stru_temp.AnchorWorkModeID = AnchorWorkModeID;
  DevicePowerOn_Stru_temp.AnchorWorkMode = E_AnchorWorkMode;
  DevicePowerOn_Stru_temp.ProtocolFrameEnderUint_Temp.CalibrationValue = UDP_public_Sum_check(((uint8_t *)&DevicePowerOn_Stru_temp) + 3, sizeof(DevicePowerOn_Stru_temp) - 6); 
  DevicePowerOn_Stru_temp.ProtocolFrameEnderUint_Temp.ProtocolEnder = UsrProtocolEnder;
  UDP_SendDataPacket((uint8_t *)&DevicePowerOn_Stru_temp,sizeof(DevicePowerOn_Stru));
}

/*
*********************************************************************************************************
*	函 数 名: DeviceReportHeart
*	功能说明: 设备上传心跳包
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void DeviceReportHeart(void)
{
  AnchorSendHeart_Stru AnchorSendHeart_Stru_temp;
  memset((uint8_t *)&AnchorSendHeart_Stru_temp, 0, sizeof(AnchorSendHeart_Stru));
  AnchorSendHeart_Stru_temp.ProtocolFrameCtrlUint_Temp.ProtocolStartID = UsrProtocolStartID;
  AnchorSendHeart_Stru_temp.ProtocolFrameCtrlUint_Temp.ProtocolType = bswap_16(UsrProtocolType);
  AnchorSendHeart_Stru_temp.ProtocolFrameCtrlUint_Temp.DeviceAddress = *(uint32_t*)(UsrDeviceAddress);
  AnchorSendHeart_Stru_temp.ProtocolFrameCtrlUint_Temp.DeviceType = UsrDeviceType;
  AnchorSendHeart_Stru_temp.ProtocolFrameCtrlUint_Temp.DataLen = sizeof(AnchorSendHeart_Stru) - sizeof(ProtocolFrameCtrlUint) - sizeof(ProtocolFrameEnderUint);
  AnchorSendHeart_Stru_temp.ProtocolFrameCtrlUint_Temp.cmd_Unit = Cmd_HeartPacket;
  AnchorSendHeart_Stru_temp.AttributePairsNum = 0x04;
  AnchorSendHeart_Stru_temp.PowerOnTypeID = PowerOnTypeID;  
  AnchorSendHeart_Stru_temp.PowerOnType = E_PowerOnType;
  AnchorSendHeart_Stru_temp.TimeStampID =  ReportTimeStampID;
  AnchorSendHeart_Stru_temp.TimeStamp = rtc_timestamp;
  AnchorSendHeart_Stru_temp.SelfCheckID = SelfCheckID;
  AnchorSendHeart_Stru_temp.SelfCheck=  DeviceSelfCheckInfo;
  AnchorSendHeart_Stru_temp.AnchorWorkModeID = AnchorWorkModeID;
  AnchorSendHeart_Stru_temp.AnchorWorkMode = E_AnchorWorkMode;
  AnchorSendHeart_Stru_temp.ProtocolFrameEnderUint_Temp.CalibrationValue = UDP_public_Sum_check(((uint8_t *)&AnchorSendHeart_Stru_temp) + 3, sizeof(AnchorSendHeart_Stru_temp) - 6); 
  AnchorSendHeart_Stru_temp.ProtocolFrameEnderUint_Temp.ProtocolEnder = UsrProtocolEnder; 
  UDP_SendDataPacket((uint8_t *)&AnchorSendHeart_Stru_temp,sizeof(AnchorSendHeart_Stru));
}

/*
*********************************************************************************************************
*	函 数 名: DeviceUpdateTimeRequest
*	功能说明: 设备上传时间更新请求
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void DeviceUpdateTimeRequest(void)
{
  UpdateTimeRequest_Stru UpdateTimeRequest_Stru_temp;
  memset((uint8_t *)&UpdateTimeRequest_Stru_temp, 0, sizeof(UpdateTimeRequest_Stru));
  UpdateTimeRequest_Stru_temp.ProtocolFrameCtrlUint_Temp.ProtocolStartID = UsrProtocolStartID;
  UpdateTimeRequest_Stru_temp.ProtocolFrameCtrlUint_Temp.ProtocolType = bswap_16(UsrProtocolType);
  UpdateTimeRequest_Stru_temp.ProtocolFrameCtrlUint_Temp.DeviceAddress = *(uint32_t*)(UsrDeviceAddress);
  UpdateTimeRequest_Stru_temp.ProtocolFrameCtrlUint_Temp.DeviceType = UsrDeviceType;
  UpdateTimeRequest_Stru_temp.ProtocolFrameCtrlUint_Temp.DataLen = sizeof(UpdateTimeRequest_Stru) - sizeof(ProtocolFrameCtrlUint) - sizeof(ProtocolFrameEnderUint);
  UpdateTimeRequest_Stru_temp.ProtocolFrameCtrlUint_Temp.cmd_Unit = Cmd_UpdateTimeRequest;
  UpdateTimeRequest_Stru_temp.AttributePairsNum = 0x01;
  UpdateTimeRequest_Stru_temp.UpdateTimeRequestID =  SyncTimeStampID;
  UpdateTimeRequest_Stru_temp.UpdateTimeRequest = rtc_timestamp;
  UpdateTimeRequest_Stru_temp.ProtocolFrameEnderUint_Temp.CalibrationValue = UDP_public_Sum_check(((uint8_t *)&UpdateTimeRequest_Stru_temp) + 3, sizeof(UpdateTimeRequest_Stru_temp) - 6); 
  UpdateTimeRequest_Stru_temp.ProtocolFrameEnderUint_Temp.ProtocolEnder = UsrProtocolEnder; 
  UDP_SendDataPacket((uint8_t *)&UpdateTimeRequest_Stru_temp,sizeof(UpdateTimeRequest_Stru));
}

/*
*********************************************************************************************************
*	函 数 名: ServiceAndDeviceResponse
*	功能说明: 服务器或设备响应结构体
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void ServiceAndDeviceResponse(uint8_t DataFrameState, uint8_t CmdAck, uint32_t TimeStamp)
{
  Response_Stru Response_Stru_temp;
  
  memset((uint8_t *)&Response_Stru_temp, 0, sizeof(Response_Stru));
  Response_Stru_temp.ProtocolFrameCtrlUint_Temp.ProtocolStartID = UsrProtocolStartID;
  Response_Stru_temp.ProtocolFrameCtrlUint_Temp.ProtocolType = bswap_16(UsrProtocolType);
  Response_Stru_temp.ProtocolFrameCtrlUint_Temp.DeviceAddress = *(uint32_t*)(UsrDeviceAddress);
  Response_Stru_temp.ProtocolFrameCtrlUint_Temp.DeviceType = UsrDeviceType;
  Response_Stru_temp.ProtocolFrameCtrlUint_Temp.DataLen = sizeof(Response_Stru) - sizeof(ProtocolFrameCtrlUint) - sizeof(ProtocolFrameEnderUint);
  Response_Stru_temp.ProtocolFrameCtrlUint_Temp.cmd_Unit = CmdAck;
  Response_Stru_temp.AttributePairsNum = 0x03;
  Response_Stru_temp.DataFrameStateID = DataFrameStateID;
  Response_Stru_temp.DataFrameState = DataFrameState;
  Response_Stru_temp.CmdAckID = CmdAckID;
  Response_Stru_temp.CmdAck = CmdAck;
  Response_Stru_temp.TimeStampID = Msg_ID;
  Response_Stru_temp.TimeStamp = TimeStamp;
  Response_Stru_temp.ProtocolFrameEnderUint_Temp.CalibrationValue = UDP_public_Sum_check(((uint8_t *)&Response_Stru_temp) + 3, sizeof(Response_Stru_temp) - 6); 
  Response_Stru_temp.ProtocolFrameEnderUint_Temp.ProtocolEnder = UsrProtocolEnder; 
  UDP_SendDataPacket((uint8_t *)&Response_Stru_temp,sizeof(Response_Stru));
}

/*
*********************************************************************************************************
*	函 数 名: ServiceAndDeviceResponse
*	功能说明: 服务器或设备响应结构体
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void DevResponConfigInfoRequst(uint8_t CmdAck, uint32_t TimeStamp)
{
  DevResponConfigInfo_Stru DevResponConfigInfo_Stru_temp;
  
  memset((uint8_t *)&DevResponConfigInfo_Stru_temp, 0, sizeof(DevResponConfigInfo_Stru));
  DevResponConfigInfo_Stru_temp.ProtocolFrameCtrlUint_Temp.ProtocolStartID = UsrProtocolStartID;
  DevResponConfigInfo_Stru_temp.ProtocolFrameCtrlUint_Temp.ProtocolType = bswap_16(UsrProtocolType);
  DevResponConfigInfo_Stru_temp.ProtocolFrameCtrlUint_Temp.DeviceAddress = *(uint32_t*)(UsrDeviceAddress);
  DevResponConfigInfo_Stru_temp.ProtocolFrameCtrlUint_Temp.DeviceType = UsrDeviceType;
  DevResponConfigInfo_Stru_temp.ProtocolFrameCtrlUint_Temp.DataLen = sizeof(DevResponConfigInfo_Stru) - sizeof(ProtocolFrameCtrlUint) - sizeof(ProtocolFrameEnderUint);
  DevResponConfigInfo_Stru_temp.ProtocolFrameCtrlUint_Temp.cmd_Unit = CmdAck;
  DevResponConfigInfo_Stru_temp.AttributePairsNum = 0x0E;
  DevResponConfigInfo_Stru_temp.SoftwareVersionID = SoftwareVersionID;
  memcpy((uint8_t *)&DevResponConfigInfo_Stru_temp.SoftwareVersion, (uint8_t *)&E_Version_stru_temp.SoftwareVersion, 3);
  DevResponConfigInfo_Stru_temp.HardwareVersionID =  HardwareVersionID;
  memcpy((uint8_t *)&DevResponConfigInfo_Stru_temp.HardwareVersion, (uint8_t *)&E_Version_stru_temp.HardwareVersion, 3);
  DevResponConfigInfo_Stru_temp.SelfCheckID = SelfCheckID;
  DevResponConfigInfo_Stru_temp.SelfCheck = DeviceSelfCheckInfo;
  DevResponConfigInfo_Stru_temp.AnchorWorkModeID = AnchorWorkModeID;
  DevResponConfigInfo_Stru_temp.AnchorWorkMode = E_AnchorWorkMode;
  DevResponConfigInfo_Stru_temp.PowerOnTypeID = PowerOnTypeID;
  DevResponConfigInfo_Stru_temp.PowerOnType = E_PowerOnType;

  DevResponConfigInfo_Stru_temp.AnchoripaddressID = AnchoripaddressID;
  memcpy((uint8_t *)&DevResponConfigInfo_Stru_temp.Anchoripaddress, (uint8_t *)&E_Network_Manage_stru_temp.Anchoripaddress, 4);
  DevResponConfigInfo_Stru_temp.AnchorPortID = AnchorPortID;
  DevResponConfigInfo_Stru_temp.AnchorPort = E_Network_Manage_stru_temp.AnchorPort;
  DevResponConfigInfo_Stru_temp.SubnetmaskID = SubnetmaskID;
  memcpy((uint8_t *)&DevResponConfigInfo_Stru_temp.Subnetmask, (uint8_t *)&E_Network_Manage_stru_temp.Subnetmask, 4);
  DevResponConfigInfo_Stru_temp.GatewayID = GatewayID;
  memcpy((uint8_t *)&DevResponConfigInfo_Stru_temp.Gateway, (uint8_t *)&E_Network_Manage_stru_temp.Gateway, 4);
  DevResponConfigInfo_Stru_temp.CenteripaddressID = CenteripaddressID;
  memcpy((uint8_t *)&DevResponConfigInfo_Stru_temp.Centeripaddress, (uint8_t *)&E_Network_Manage_stru_temp.Centeripaddress, 4);
  DevResponConfigInfo_Stru_temp.CenterPortID = CenterPortID;
  DevResponConfigInfo_Stru_temp.CenterPort = E_Network_Manage_stru_temp.CenterPort;
  
  DevResponConfigInfo_Stru_temp.EngineipaddressID = EngineipaddressID;
  memcpy((uint8_t *)&DevResponConfigInfo_Stru_temp.Engineipaddress, (uint8_t *)&E_Network_Manage_stru_temp.Engineipaddress, 4);
  DevResponConfigInfo_Stru_temp.EnginePortID = EnginePortID;
  DevResponConfigInfo_Stru_temp.EnginePort = E_Network_Manage_stru_temp.EnginePort;

  DevResponConfigInfo_Stru_temp.TimeStampID = ReportTimeStampID;
  DevResponConfigInfo_Stru_temp.TimeStamp = TimeStamp;
  DevResponConfigInfo_Stru_temp.ProtocolFrameEnderUint_Temp.CalibrationValue = UDP_public_Sum_check(((uint8_t *)&DevResponConfigInfo_Stru_temp) + 3, sizeof(DevResponConfigInfo_Stru_temp) - 6); 
  DevResponConfigInfo_Stru_temp.ProtocolFrameEnderUint_Temp.ProtocolEnder = UsrProtocolEnder; 
  UDP_SendDataPacket((uint8_t *)&DevResponConfigInfo_Stru_temp,sizeof(DevResponConfigInfo_Stru));
}

/*
*********************************************************************************************************
*	函 数 名: ServiceSendConfig_Handle
*	功能说明: 服务器下发的配置信息处理
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
extern ServerSendConfig_Stru *ServerSendConfig_Stru_temp;
void ServiceSendConfig_Handle(void)
{
  uint8_t DataFrameState;
  uint8_t CmdAck; 
  uint32_t TimeStamp; 
  uint8_t CalibrationValue_temp = 0;
  ServerSendConfig_Stru_temp = (ServerSendConfig_Stru *)mymalloc(SRAMIN, sizeof(ServerSendConfig_Stru));
  if(ServerSendConfig_Stru_temp != NULL)
  {
    mymemset((uint8_t *)ServerSendConfig_Stru_temp, 0, sizeof(ServerSendConfig_Stru_temp));
    memcpy((uint8_t *)ServerSendConfig_Stru_temp, (uint8_t *)NetworkRecvData_Stru_temp.NetworkRecvData_Buffer, sizeof(ServerSendConfig_Stru));
    CalibrationValue_temp = UDP_public_Sum_check(((uint8_t *)ServerSendConfig_Stru_temp) + 3, sizeof(ServerSendConfig_Stru) - 6); 
    if(CalibrationValue_temp == ServerSendConfig_Stru_temp->ProtocolFrameEnderUint_Temp.CalibrationValue)
    {
			log_print(DEBUG,("ServerSendConfig_Stru Calibration Success\r\n"));
			DataFrameState = 0x01;
      TimeStamp = ServerSendConfig_Stru_temp->TimeStamp;
      CmdAck = ServerSendConfig_Stru_temp->ProtocolFrameCtrlUint_Temp.cmd_Unit;
      ServiceAndDeviceResponse(DataFrameState, CmdAck, TimeStamp);
      E_AnchorWorkMode = ServerSendConfig_Stru_temp->AnchorWorkMode;
      memcpy((uint8_t *)&E_Network_Manage_stru_temp.Anchoripaddress, (uint8_t *)ServerSendConfig_Stru_temp->Anchoripaddress, 4);
			for(int i = 0; i < 4; i++)
		  log_print(DEBUG,("ipaddress[%d] = %d\r\n",i, E_Network_Manage_stru_temp.Anchoripaddress[i]));
			E_Network_Manage_stru_temp.AnchorPort = ServerSendConfig_Stru_temp->CenterPort;
      //E_Network_Manage_stru_temp.AnchorPort = ServerSendConfig_Stru_temp->AnchorPort;
			log_print(DEBUG,("EnginePort = %d\r\n", E_Network_Manage_stru_temp.AnchorPort));
      memcpy((uint8_t *)&E_Network_Manage_stru_temp.Subnetmask, (uint8_t *)ServerSendConfig_Stru_temp->Subnetmask, 4);
			for(int i = 0; i < 4; i++)
				log_print(DEBUG,("Subnetmask[%d] = %d\r\n",i, E_Network_Manage_stru_temp.Subnetmask[i]));
      memcpy((uint8_t *)&E_Network_Manage_stru_temp.Gateway, (uint8_t *)ServerSendConfig_Stru_temp->Gateway, 4);
			for(int i = 0; i < 4; i++)
				log_print(DEBUG,("Gateway[%d] = %d\r\n",i, E_Network_Manage_stru_temp.Gateway[i]));
      memcpy((uint8_t *)&E_Network_Manage_stru_temp.Centeripaddress, (uint8_t *)ServerSendConfig_Stru_temp->Centeripaddress, 4);
			for(int i = 0; i < 4; i++)
				log_print(DEBUG,("Centeripaddress[%d] = %d\r\n",i, E_Network_Manage_stru_temp.Centeripaddress[i]));
      E_Network_Manage_stru_temp.CenterPort = ServerSendConfig_Stru_temp->CenterPort;
      memcpy((uint8_t *)&E_Network_Manage_stru_temp.Engineipaddress, (uint8_t *)ServerSendConfig_Stru_temp->Engineipaddress, 4);
      E_Network_Manage_stru_temp.EnginePort = ServerSendConfig_Stru_temp->EnginePort;
			log_print(DEBUG,("EnginePort = %d\r\n", E_Network_Manage_stru_temp.EnginePort));
      E_Network_Manage_stru_temp.net_mark = 0x01;
      //Network_Manage_Write();
      EE_Network_Write();
      EE_AnchorWorkMode_Write();
      //AnchorWorkMode_Write();
			SYS_REBOOT();
    }
    else
    {
      log_print(DEBUG,("ServerSendConfig_Stru Calibration Fail\r\n"));
    }
    myfree(SRAMIN, ServerSendConfig_Stru_temp);
  }
  else
  {
    myfree(SRAMIN, ServerSendConfig_Stru_temp);
  }
}

/*
*********************************************************************************************************
*	函 数 名: DeviceResponseHeart_Handle
*	功能说明: 心跳包响应处理
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
uint8_t Network_Communciton_Flag = 0;
extern uint8_t tim6_irq_flag;
extern SemaphoreHandle_t CountSemaphore;//计数型信号量
extern xTaskHandle GeneralSignal_Handle;
void DeviceResponseHeart_Handle(void)
{
  BaseType_t ret = 0;
  static int Count_temp = 0;
  static int SemaphoreTime_cnt = 0;
  uint8_t  semavalue;
  while(1)
  {
    if(tim6_irq_flag == 1)
    {
      tim6_irq_flag = 0;
      Count_temp++;
      SemaphoreTime_cnt++;
    }
    if(Count_temp >= 30)     // 如何60s中服务器和基站都未进行通行，则发送心跳包
    {
      Count_temp = 0;
			//Network_Communciton_Flag = 0;
      if(Network_Communciton_Flag == 0)
      {
        DeviceReportHeart();		// 发送心跳包
        ret = xSemaphoreGive(CountSemaphore);//释放计数型信号量
        if(ret == pdFALSE)
        {
          log_print(ERR,("SemaphoreGive Fail\r\n"));
        }
      }
      semavalue = uxSemaphoreGetCount(CountSemaphore); 	//获取数值信号量值
      log_print(DEBUG,("semavalue = %d\r\n", semavalue));
      Network_Communciton_Flag = 0;
    }
    if(SemaphoreTime_cnt >= (60*30))
    {
      SemaphoreTime_cnt = 0;
      semavalue = uxSemaphoreGetCount(CountSemaphore); 	//获取数值信号量值
      if(semavalue > 25)
      {
        SYS_REBOOT();
      }
      else
      {
        for(int i = 0; i < semavalue; i++)
        {
          xSemaphoreTake(CountSemaphore,0);
        }
      }
    }
    if(Exception_Massage_Global == ExceptionTaskSuspend) 
		{
			Exception_Massage_Global = 0;
			
			vTaskSuspend(GeneralSignal_Handle);
		}
    HAL_RTC_GetTime(&RTC_Handler,&RTC_TimeStruct,RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&RTC_Handler,&RTC_DateStruct,RTC_FORMAT_BIN);
    rtc_timestamp = time2Stamp(RTC_DateStruct, RTC_TimeStruct);
		//log_print(DEBUG,("rtc_timestamp = %d\r\n", rtc_timestamp));
    HAL_GPIO_TogglePin(LED_GPIO_PORT, LED_GPIO_PIN);
    vTaskDelay(1000);
  }
}

/*
*********************************************************************************************************
*	函 数 名: ServerCtrlAnchor_Handle
*	功能说明: 服务器控制基站
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
extern ServerCtrlAnchor_Stru *ServerCtrlAnchor_Stru_temp;
void ServerCtrlAnchor_Handle(void)
{
  uint8_t DataFrameState;
  uint8_t CmdAck; 
  uint32_t TimeStamp; 
  uint8_t CalibrationValue_temp = 0;
  ServerCtrlAnchor_Stru_temp = (ServerCtrlAnchor_Stru *)mymalloc(SRAMIN, sizeof(ServerCtrlAnchor_Stru));
  if(ServerCtrlAnchor_Stru_temp != NULL)
  {
    mymemset((uint8_t *)ServerCtrlAnchor_Stru_temp, 0, sizeof(ServerCtrlAnchor_Stru_temp));
    memcpy((uint8_t *)ServerCtrlAnchor_Stru_temp, (uint8_t *)NetworkRecvData_Stru_temp.NetworkRecvData_Buffer, sizeof(ServerCtrlAnchor_Stru));
    CalibrationValue_temp = UDP_public_Sum_check(((uint8_t *)ServerCtrlAnchor_Stru_temp) + 3, sizeof(ServerCtrlAnchor_Stru) - 6); 
		log_print(DEBUG,("temp = %x, %x\r\n",CalibrationValue_temp,ServerCtrlAnchor_Stru_temp->ProtocolFrameEnderUint_Temp.CalibrationValue));
    if(CalibrationValue_temp == ServerCtrlAnchor_Stru_temp->ProtocolFrameEnderUint_Temp.CalibrationValue)
    {
			DataFrameState = 0x01;
      TimeStamp = ServerCtrlAnchor_Stru_temp->TimeStamp;
      CmdAck = ServerCtrlAnchor_Stru_temp->ProtocolFrameCtrlUint_Temp.cmd_Unit;
      ServiceAndDeviceResponse(DataFrameState, CmdAck, TimeStamp);
      if( ServerCtrlAnchor_Stru_temp->ServerCtrlAnchor == PowerOnOpen)
      {
        E_PowerOnType = ServerCtrlAnchor_Stru_temp->ServerCtrlAnchor;
        EE_PowerOnType_Write();
        SYS_REBOOT();
      }
      else if(ServerCtrlAnchor_Stru_temp->ServerCtrlAnchor == SysReboot)
      {
        upgrateFlag = 1;
        EE_upgrateFlag_Write();
        E_PowerOnType = ServerCtrlAnchor_Stru_temp->ServerCtrlAnchor;
        EE_PowerOnType_Write();
        SYS_REBOOT();
      }
      else ;
      log_print(DEBUG,("ServerCtrlAnchor_Stru Calibration Success\r\n"));
    }
    else
    {
      log_print(DEBUG,("ServerCtrlAnchor_Stru Calibration Fail\r\n"));
    }
    
    myfree(SRAMIN, ServerCtrlAnchor_Stru_temp);
  }
  else
  {
    myfree(SRAMIN, ServerCtrlAnchor_Stru_temp);
  }
}

/*
*********************************************************************************************************
*	函 数 名: UpdateTimeReqReponse_Handle
*	功能说明: 基站时间更新请求响应处理
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void UpdateTimeReqReponse_Handle(void)
{
  uint8_t DataFrameState;
  uint8_t CmdAck; 
  uint32_t TimeStamp; 
  uint8_t CalibrationValue_temp = 0;
  Response_Stru *Response_Stru_temp = NULL;
  Response_Stru_temp = (Response_Stru *)mymalloc(SRAMIN, sizeof(Response_Stru));
  if(Response_Stru_temp != NULL)
  {
    mymemset((uint8_t *)Response_Stru_temp, 0, sizeof(Response_Stru_temp));
    memcpy((uint8_t *)Response_Stru_temp, (uint8_t *)NetworkRecvData_Stru_temp.NetworkRecvData_Buffer, sizeof(Response_Stru));
    CalibrationValue_temp = UDP_public_Sum_check(((uint8_t *)Response_Stru_temp) + 3, sizeof(Response_Stru) - 6); 
		log_print(DEBUG,("temp = %x, %x\r\n",CalibrationValue_temp,Response_Stru_temp->ProtocolFrameEnderUint_Temp.CalibrationValue));
    if(CalibrationValue_temp == Response_Stru_temp->ProtocolFrameEnderUint_Temp.CalibrationValue)
    {
      TimeStamp = Response_Stru_temp->TimeStamp;
			//Staticgloabl_stru_temp.timestamp = TimeStamp;
      E_Staticgloabl_stru_temp.timestamp = TimeStamp;
			//Static_Gloabl_Write();
      EE_TimeStamp_Write();
      stamp2Time_SetTimeData(TimeStamp);  // 
			Exception_Massage_Global = UpdateTimeReqReponse;
      log_print(DEBUG,("UpdateTimeReqReponse Success\r\n"));
    }
    else
    {
      log_print(DEBUG,("UpdateTimeReqReponse Fail\r\n"));
    }
    myfree(SRAMIN, Response_Stru_temp);
  }
  else
  {
    myfree(SRAMIN, Response_Stru_temp);
  }
}

/*
*********************************************************************************************************
*	函 数 名: ServerFindAnchorInfo_Handle
*	功能说明: 服务器查询基站信息
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void ServerFindAnchorInfo_Handle(void)
{
  uint8_t DataFrameState;
  uint8_t CmdAck; 
  uint32_t TimeStamp; 
  uint8_t CalibrationValue_temp = 0;
  ServerFindAnchorInfo_Stru *ServerFindAnchorInfo_Stru_temp = NULL;
  ServerFindAnchorInfo_Stru_temp = (ServerFindAnchorInfo_Stru *)mymalloc(SRAMIN, sizeof(ServerFindAnchorInfo_Stru));
  if(ServerFindAnchorInfo_Stru_temp != NULL)
  {
    mymemset((uint8_t *)ServerFindAnchorInfo_Stru_temp, 0, sizeof(ServerFindAnchorInfo_Stru_temp));
    memcpy((uint8_t *)ServerFindAnchorInfo_Stru_temp, (uint8_t *)NetworkRecvData_Stru_temp.NetworkRecvData_Buffer, sizeof(ServerFindAnchorInfo_Stru));
    CalibrationValue_temp = UDP_public_Sum_check(((uint8_t *)ServerFindAnchorInfo_Stru_temp) + 3, sizeof(ServerFindAnchorInfo_Stru) - 6);		
    if(CalibrationValue_temp == ServerFindAnchorInfo_Stru_temp->ProtocolFrameEnderUint_Temp.CalibrationValue)
    {
      TimeStamp = ServerFindAnchorInfo_Stru_temp->TimeStamp;
      CmdAck = ServerFindAnchorInfo_Stru_temp->ProtocolFrameCtrlUint_Temp.cmd_Unit;
      DevResponConfigInfoRequst(CmdAck, TimeStamp);
      log_print(DEBUG,("ServerFindAnchorInfo Calibration Success\r\n"));
    }
    else
    {
      log_print(DEBUG,("ServerFindAnchorInfo Calibration Fail\r\n"));
    }
    myfree(SRAMIN, ServerFindAnchorInfo_Stru_temp);
  }
  else
  {
    myfree(SRAMIN, ServerFindAnchorInfo_Stru_temp);
  }
}












