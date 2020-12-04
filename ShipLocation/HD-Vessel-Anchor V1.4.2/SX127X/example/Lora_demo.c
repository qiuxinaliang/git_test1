/*
 * @:*********************************************************************************************************: 
 * @Date: 2020-12-04 13:51:50
 * @LastEditTime: 2020-12-04 14:27:20
 * @**********************************************************************************************************: 
 */
#include "Lora_demo.h"

/*
*********************************************************************************************************
*	函 数 名:JianZhang_LoraData_Handle
*	功能说明: 肩章信号处理
*	形	  参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
extern uint32_t uwTick;
void JianZhang_LoraData_Handle(void)
{
  uint8_t Lora_RxBuf[100] = { 0 };
  uint8_t JianZhang_RxBuf[50] = { 0 };
  uint8_t Calibration_temp = 0;
  int DataPacket_len = 100;
	int SendData_len = 0;
  uint8_t ret = -1;
	memset((uint8_t *)Lora_RxBuf, 0, sizeof(Lora_RxBuf));
	memset((uint8_t *)JianZhang_RxBuf, 0, sizeof(JianZhang_RxBuf));
  ProtocolFrameCtrlUint ProtocolFrameCtrlUint_temp;
  ProtocolFrameEnderUint ProtocolFrameEnderUint_temp;
  ProtocolFrameCtrlUint_temp.ProtocolStartID = UsrProtocolStartID;
  ProtocolFrameCtrlUint_temp.ProtocolType = bswap_16(UsrProtocolType);  //41 00 23 00
  ProtocolFrameCtrlUint_temp.DeviceAddress = *(uint32_t*)(UsrDeviceAddress);
	//ProtocolFrameCtrlUint_temp.DeviceAddress = 0x00230041;
  ProtocolFrameCtrlUint_temp.DeviceType = SeaPosJianZhang_Up;
  ProtocolFrameCtrlUint_temp.DataLen = 0x00;
  ProtocolFrameCtrlUint_temp.cmd_Unit = 0xFF;
  ProtocolFrameEnderUint_temp.CalibrationValue = 0; 
  ProtocolFrameEnderUint_temp.ProtocolEnder = UsrProtocolEnder;

	Calibration_temp = 0;
	DataPacket_len = 50;
	ret = sx127xRx(JianZhang_RxBuf, (uint8_t *)&DataPacket_len, 100);
	switch (ret)
	{
		case 0:
			#if 1
			Calibration_temp = UDP_public_Sum_check((uint8_t *)JianZhang_RxBuf, DataPacket_len -1);
			if( Calibration_temp == JianZhang_RxBuf[DataPacket_len  -1])
			{
				ProtocolFrameCtrlUint_temp.DataLen = DataPacket_len;
				memcpy((uint8_t *)Lora_RxBuf, (uint8_t *)&ProtocolFrameCtrlUint_temp, sizeof(ProtocolFrameCtrlUint));
				memcpy((uint8_t *)Lora_RxBuf + sizeof(ProtocolFrameCtrlUint), (uint8_t *)JianZhang_RxBuf, DataPacket_len);
				ProtocolFrameEnderUint_temp.CalibrationValue = UDP_public_Sum_check((uint8_t *)Lora_RxBuf + 3, sizeof(ProtocolFrameCtrlUint) + DataPacket_len - 3);
				memcpy((uint8_t *)Lora_RxBuf + sizeof(ProtocolFrameCtrlUint) + DataPacket_len, (uint8_t *)&ProtocolFrameEnderUint_temp, sizeof(ProtocolFrameEnderUint));
				SendData_len = sizeof(ProtocolFrameCtrlUint) + DataPacket_len + sizeof(ProtocolFrameEnderUint);
				UDP_SendDataPacket((uint8_t *)&Lora_RxBuf, SendData_len);
				USART1_For_Debug_tx(SendData_len, (uint8_t *)&Lora_RxBuf);
				//printf("\r\n");
				SendData_len = 0;
				vTaskDelay(5);
				//sx127xSend((uint8_t *)Lora_RxBuf, 5, 50);//
				HAL_GPIO_TogglePin(LED_GPIO_PORT, LED_GPIO_PIN);
				vTaskDelay(10);
			}
			#endif
			//log_print(DEBUG,("len = %d, %s\r\n", DataPacket_len, (uint8_t *)JianZhang_RxBuf));
			break;
		case 1:
		case 2:
		 //log_print(DEBUG,("rx timeout ret:%d\r\n", ret));
			break;
		default:
			log_print(DEBUG,("unknow ret:%d\r\n", ret));
			break;
	}
		//vTaskDelay(50);
}






