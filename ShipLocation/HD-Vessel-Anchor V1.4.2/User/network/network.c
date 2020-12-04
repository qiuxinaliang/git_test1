/*
 * @:*********************************************************************************************************: 
 * @Date: 2020-12-04 13:51:51
 * @LastEditTime: 2020-12-04 14:06:39
 * @**********************************************************************************************************: 
 */
#include "network.h"

extern NetworkRecvData_Stru NetworkRecvData_Stru_temp;
extern SemaphoreHandle_t MutexSemaphore;	//互斥信号量
uint8_t Exception_Massage_Global = 0;

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
	ProtocolFrameCtrlUint ProtocolFrameCtrlUint_temp;
	while(1)
	{
    if(NetworkRecvData_Stru_temp.NetworkRecvData_Len > 0)
    {
      log_print(DEBUG,("net\r\n"));
			mymemset((uint8_t *)NetWorkData_temp, 0x00, NetworkRecvData_Stru_temp.NetworkRecvData_Len);
			memcpy((uint8_t *)NetWorkData_temp, (uint8_t *)NetworkRecvData_Stru_temp.NetworkRecvData_Buffer, NetworkRecvData_Stru_temp.NetworkRecvData_Len);
			mymemset((uint8_t *)&ProtocolFrameCtrlUint_temp, 0x00, sizeof(ProtocolFrameCtrlUint));
			memcpy((uint8_t *)&ProtocolFrameCtrlUint_temp, (uint8_t *)NetWorkData_temp, sizeof(ProtocolFrameCtrlUint));
			if(ProtocolFrameCtrlUint_temp.DeviceType == SeaPosJianZhang_Down)
			{
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