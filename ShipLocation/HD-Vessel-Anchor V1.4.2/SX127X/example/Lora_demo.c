#include "Lora_demo.h"

#if 0
/*
*********************************************************************************************************
*	函 数 名: Lora_SendData_Handle
*	功能说明: Lora 发送信号处理
*	形	  参: uint8_t *buf:发送缓存， len:接收信号长度统计， timeoutMs:延时设置
*	返 回 值:  0: 发送正常， 其他：发送异常
*********************************************************************************************************
*/
uint8_t Lora_SendData_Handle(uint8_t  *data, int len, uint32_t timeoutMs)
{
  uint32_t systickBak = GET_TICK_COUNT(), currTick;

	Write127xReg(REG_LR_PAYLOADLENGTH, len); //设置负载长度
	Write127xReg(REG_LR_FIFOTXBASEADDR, 0); //将发送buf的基地址指向0x00，此时整个fifo都可以用来发送数据
	Write127xReg(REG_LR_FIFOADDRPTR, 0);	//将fifi读写指针执行0x00，此时向寄存器0x00读写数据指针会自增的将数据写入fifo
	SX127xSetOpMode(LORA_OPMODE_STANDBY);	//LORA_OPMODE_SLEEP 模式不能读写fifo

	SX127xWriteFifo(data, len); //将要发送的数据写入fifo
	//开启中断屏蔽位(只留下了 RFLR_IRQFLAGS_TXDONE 中断没有屏蔽掉)，设置为发送模式后任意读写一个寄存器就会触发发送，这里写入这个开中断正好触发
	Write127xReg(REG_LR_IRQFLAGSMASK, RFLR_IRQFLAGS_RXTIMEOUT |
              RFLR_IRQFLAGS_RXDONE |
              RFLR_IRQFLAGS_PAYLOADCRCERROR |
              RFLR_IRQFLAGS_VALIDHEADER |
              //RFLR_IRQFLAGS_TXDONE |
              RFLR_IRQFLAGS_CADDONE |
              RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
              RFLR_IRQFLAGS_CADDETECTED);
	Write127xReg(REG_LR_DIOMAPPING1, (Read127xReg(REG_LR_DIOMAPPING1) & RFLR_DIOMAPPING1_DIO0_MASK) | RFLR_DIOMAPPING1_DIO0_01); //DIO0设置为TXdone中断
	//DEBUG("DIO0=%d\r\n0x06:%02x\r\n",SX1276ReadDio0(),Read127xReg(0x06));
	SX127xSetOpMode(LORA_OPMODE_TRANSMITTER); //设置为发送模式
	Read127xReg(REG_LR_IRQFLAGS);			  //设置发送后读写任意寄存器可以开始发送

	while (1)
	{
		if (1 == SX1276ReadDio0())
		{
			Write127xReg(REG_LR_IRQFLAGS, RFLR_IRQFLAGS_TXDONE); //清除中断标志位
			return 0;
		}
		currTick = GET_TICK_COUNT();
		if (currTick >= systickBak)
		{
			if (currTick - systickBak > timeoutMs)
			{
				return 1;
			}
		}
		else
		{
			//currTick溢出了
			if (currTick + (~systickBak) > timeoutMs)
			{
				return 1;
			}
		}
	}
}

/*
*********************************************************************************************************
*	函 数 名:Lora_RecvData_Handle
*	功能说明: 接收信号处理
*	形	  参: uint8_t *buf:接收缓存， len:接收信号长度统计， timeoutMs:延时设置
*	返 回 值: 0: 接收到有效数据， 其他：接收异常
*********************************************************************************************************
*/
uint8_t Lora_RecvData_Handle(uint8_t *buf, uint8_t *len, uint32_t timeoutMs)
{
	uint32_t systickBak = GET_TICK_COUNT(), currTick;
	uint8_t u8_reciveLength;

	SX127xSetOpMode(LORA_OPMODE_STANDBY);
	Write127xReg(REG_LR_FIFORXBASEADDR, 0);	   //将接收buf的基地址指向0x00，此时整个fifo都可以用来接收数据
	Write127xReg(REG_LR_FIFOADDRPTR, 0);	   //将fifi读写指针执行0x00，此时向寄存器0x00读写数据指针会自增的从fifo中读取数据
	Write127xReg(REG_LR_SYMBTIMEOUTLSB, 0xFF); //配置 0x1f rx超时

	//配置中断
	Write127xReg(REG_LR_IRQFLAGSMASK, RFLR_IRQFLAGS_RXTIMEOUT |
              //RFLR_IRQFLAGS_RXDONE |
              RFLR_IRQFLAGS_PAYLOADCRCERROR |
              RFLR_IRQFLAGS_VALIDHEADER |
              RFLR_IRQFLAGS_TXDONE |
              RFLR_IRQFLAGS_CADDONE |
              RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
              RFLR_IRQFLAGS_CADDETECTED);
	// DIO0=RxDone 0x00, DIO2=RxTimeout 0x00
	Write127xReg(REG_LR_DIOMAPPING1, (Read127xReg(REG_LR_DIOMAPPING1) & RFLR_DIOMAPPING1_DIO0_MASK) | RFLR_DIOMAPPING1_DIO0_00 | RFLR_DIOMAPPING1_DIO1_00);
	SX127xSetOpMode(LORA_OPMODE_RECEIVER); //连续接收
	//SX127xSetOpMode( LORA_OPMODE_RECIVER_SINGLE );//单次接收
	Read127xReg(REG_LR_IRQFLAGS); //设置接收后读写任意寄存器可以开始接收

	while (1)
	{
		if (1 == SX1276ReadDio0())
		{
			Write127xReg(REG_LR_IRQFLAGS, RFLR_IRQFLAGS_RXDONE); //清除中断标志位
			u8_reciveLength = Read127xReg(REG_LR_NBRXBYTES);
			if (u8_reciveLength > *len)
			{
				u8_reciveLength = *len;
			}
			else
			{
				*len = u8_reciveLength;
			}
			SX127xReadFifo(buf, u8_reciveLength);
			return 0;
		}
		if (1 == SX1276ReadDio1())
		{
			Write127xReg(REG_LR_IRQFLAGS, RFLR_IRQFLAGS_RXTIMEOUT); //清除中断标志位
			return 1;
		}
		currTick = GET_TICK_COUNT();
		if (currTick >= systickBak)
		{
			if (currTick - systickBak > timeoutMs)
			{
				return 1;
			}
		}
		else
		{
			//currTick溢出了
			if (currTick + (~systickBak) > timeoutMs)
			{
				return 1;
			}
		}
	}
}
#endif

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
	//}
}






