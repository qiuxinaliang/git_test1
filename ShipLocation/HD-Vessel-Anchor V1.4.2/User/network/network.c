#include "network.h"

extern NetworkRecvData_Stru NetworkRecvData_Stru_temp;
extern SemaphoreHandle_t MutexSemaphore;	//互斥信号量
uint8_t Exception_Massage_Global = 0;

/*
*********************************************************************************************************
*	函 数 名: Usr_PowerOnMessage_Handle
*	功能说明: 开机信息上传至服务器
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
#if 0
void Usr_PowerOnMessage_Handle(void)
{
	int cnt_temp = 0;
	ProtocolFrameCtrlUint ProtocolFrameCtrlUint_temp;
	
	PowerOnMessage_Handle();   // 上传开机信息
  cnt_temp = 1000;
  while(cnt_temp--)
  {
    if((cnt_temp > 0) &&(cnt_temp%250 == 0)) //间隔2.5s发送一次开机信息
      PowerOnMessage_Handle();
    if(NetworkRecvData_Len > 0)
    {
      mymemset((uint8_t *)&ProtocolFrameCtrlUint_temp, 0x00, sizeof(ProtocolFrameCtrlUint_temp));
      memcpy((uint8_t *)&ProtocolFrameCtrlUint_temp, (uint8_t *)NetworkRecvData_Stru_temp.NetworkRecvData_Buffer, sizeof(ProtocolFrameCtrlUint_temp));
      if(ProtocolFrameCtrlUint_temp.cmd_Unit == Cmd_SendDeviceArrti)
      {
        log_print(DEBUG, ("PowerOnMessageRecvSuccess\r\n"));
        NetworkRecvData_Len = 0;
        cnt_temp = 0;
        break;
      }
      NetworkRecvData_Len = 0;
    }
    vTaskDelay(10);
  }
	cnt_temp = 0;
}
#endif

/*
*********************************************************************************************************
*	函 数 名: Usr_NetworkRecvData_Handle
*	功能说明: 网络接收数据处理
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
uint8_t NetworkRecvData_Len;
void Usr_NetworkRecvData_Handle(void)
{
	int ret = 0;
	uint32_t temp = 0;
	uint8_t NetWorkData_temp[100] = { 0 };
	//uint8_t *NetWorkData_temp = NULL;
	ProtocolFrameCtrlUint ProtocolFrameCtrlUint_temp;
	//NetworkRecvData_Len = 0;
	while(1)
	{
		//sx127xSend((uint8_t *)temp_t, sizeof(temp_t), 100);
		//vTaskDelay(150);
		//log_print(DEBUG,("NetworkRecvData_Len  = %d\r\n",NetworkRecvData_Len));
    if(NetworkRecvData_Stru_temp.NetworkRecvData_Len > 0)
    {
			//NetworkRecvData_Stru_temp.NetworkRecvData_Len = 0;
      log_print(DEBUG,("net\r\n"));
		
		//NetWorkData_temp = (uint8_t *)mymalloc(SRAMIN, NetworkRecvData_Stru_temp.NetworkRecvData_Len);
		//if(NetWorkData_temp != NULL)
		//{
			mymemset((uint8_t *)NetWorkData_temp, 0x00, NetworkRecvData_Stru_temp.NetworkRecvData_Len);
			memcpy((uint8_t *)NetWorkData_temp, (uint8_t *)NetworkRecvData_Stru_temp.NetworkRecvData_Buffer, NetworkRecvData_Stru_temp.NetworkRecvData_Len);
			mymemset((uint8_t *)&ProtocolFrameCtrlUint_temp, 0x00, sizeof(ProtocolFrameCtrlUint));
			memcpy((uint8_t *)&ProtocolFrameCtrlUint_temp, (uint8_t *)NetWorkData_temp, sizeof(ProtocolFrameCtrlUint));
			if(ProtocolFrameCtrlUint_temp.DeviceType == SeaPosJianZhang_Down)
			{
				//srand(uwTick);
				//rand_temp = rand()%30 + 20;      //产生一个30到50的随机数
				//vTaskDelay(rand_temp);
				ret = sx127xSend((uint8_t *)NetWorkData_temp + sizeof(ProtocolFrameCtrlUint), bswap_16(ProtocolFrameCtrlUint_temp.DataLen), 100);
				NetworkRecvData_Stru_temp.NetworkRecvData_Len = 0;
				log_print(DEBUG,("Jz_ret = %d\r\n", ret));
			}
			else if(ProtocolFrameCtrlUint_temp.DeviceType == SeaLoraAnchor_Down)
			{
				switch(ProtocolFrameCtrlUint_temp.cmd_Unit)
				{
					case Cmd_ServerSendNetworkConfig:
					{
						log_print(DEBUG,("ServerSendNetworkConfig\r\n"));
						ServiceSendConfig_Handle();
						break; 
					}
					case Cmd_HeartPacket:
					{
						log_print(DEBUG,("HeartPacket\r\n"));
						xSemaphoreTake(CountSemaphore,0);
						break;
					}
					case Cmd_ServerSendRegular:
					{
						log_print(DEBUG,("ServerCtrlAnchor\r\n"));
						ServerCtrlAnchor_Handle();
						break;
					}
					case Cmd_UpdateTimeRequest:
					{
						//temp = UpdateTimeReqReponse;
						//Msg_Queue_Send(Msg_For_Exception_queue, (uint8_t *)&temp);
						UpdateTimeReqReponse_Handle();
						break;
					}
					case Cmd_ServerFindAnchorInfo:
					{
						ServerFindAnchorInfo_Handle();
						break;
					}
					case Cmd_SendDeviceArrti:
					{
						Exception_Massage_Global = PowerOnMessageReponse;
					}
					default: break;
				}
				NetworkRecvData_Stru_temp.NetworkRecvData_Len = 0;
			}
		  NetworkRecvData_Stru_temp.NetworkRecvData_Len = 0;
    }
		JianZhang_LoraData_Handle();
		vTaskDelay(10);
	}
}