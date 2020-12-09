#include "upgrade.h"

UpgradeFile_Info UpgradeFile_Info_t; // 升级信息结构体
uint8_t UpgradePacket_n = 0;   // 第n个数据包
uint8_t *UpgradeFileBuf = NULL;
uint32_t UpgradeFlag;
typedef  void (*iapfun)(void);
iapfun jump2app;
struct upgradePacketData
{
	uint8_t upgradePacketData[1024];
};
static inline uint16_t bswap_16(uint16_t x);
static inline uint32_t bswap_32(uint32_t x);

//设置栈顶地址
//addr:栈顶地址
__asm void MSR_MSP(u32 addr) 
{
	MSR MSP, r0 			//set Main Stack value
	BX r14
}
/**
 * @:*********************************************************************************************************: 
 * @函 数 名: BootLoad_to_App
* @功能说明: 跳转到应用程序
 * @形    参: AppAddr：跳转地址
 * @返 回 值: 无
 * @**********************************************************************************************************: 
 */
void BootLoad_to_App(uint32_t AppAddr)
{
	if(((*(vu32*)(AppAddr+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
	{
		log_print(DEBUG, ("BootLoad jump to Application ...\r\n"));
		__HAL_RCC_USART1_CLK_DISABLE();
		HAL_GPIO_DeInit(Debug_USART_RX_GPIO_Port, Debug_USART_TX_Pin |Debug_USART_RX_Pin);
		HAL_NVIC_DisableIRQ(Debug_USART_IRQn);
		HAL_NVIC_DisableIRQ(ETH_IRQn);
		
		__HAL_RCC_TIM3_CLK_DISABLE();
		HAL_NVIC_DisableIRQ(TIM3_IRQn);
		__HAL_RCC_TIM6_CLK_DISABLE();
		HAL_NVIC_DisableIRQ(TIM6_DAC_IRQn);
		
		
		__set_PRIMASK(1);	
		jump2app = (iapfun) * (vu32 *)(AppAddr + 4);			
		MSR_MSP(*(vu32 *)AppAddr);							 
		jump2app();			
	}
	else
	{
		log_print(DEBUG, ("BootLoad jump to Application Err\r\n"));
	}
}

u32 iapbuf[512]; 	//2K字节缓存  
//appxaddr:应用程序的起始地址
//appbuf:应用程序CODE.
//appsize:应用程序大小(字节).
void iap_write_appbin(u32 appxaddr,u8 *appbuf,u32 appsize)
{
	u32 t;
	u16 i=0;
	u32 temp;
	u32 fwaddr = appxaddr;//当前写入的地址
	u8 *dfu=appbuf;
	for(t=0;t<appsize;t+=4)
	{						   
		temp=(u32)dfu[3]<<24;   
		temp|=(u32)dfu[2]<<16;    
		temp|=(u32)dfu[1]<<8;
		temp|=(u32)dfu[0];	  
		dfu+=4;//偏移4个字节
		iapbuf[i++]=temp;	    
		if(i==512)
		{
			i=0; 
			STMFLASH_Write(fwaddr,iapbuf,512);
			fwaddr+=2048;//偏移2048  512*4=2048
		}
	} 
	if(i)STMFLASH_Write(fwaddr,iapbuf,i);//将最后的一些内容字节写进去.  
}

/**
 * @:*********************************************************************************************************: 
 * @函 数 名: UDP_SendUpgradeReq
* @功能说明: 发送升级请求
 * @形    参: 无
 * @返 回 值: 无
 * @**********************************************************************************************************: 
 */
uint8_t temp;
void UDP_SendUpgradeReq(void)
{
	uint8_t *pData = NULL;
	uint32_t temp_address = 0;
	UpgradeReq_Stru *UpgradeReq_Stru_temp = NULL;
	UpgradeReq_Stru_temp = (UpgradeReq_Stru *)mymalloc(SRAMIN, sizeof(UpgradeReq_Stru));
	if(UpgradeReq_Stru_temp != NULL)
	{
		memset((uint8_t *)UpgradeReq_Stru_temp, 0, sizeof(UpgradeReq_Stru));
		UpgradeReq_Stru_temp->ProtocolStartID = 0x24;
		UpgradeReq_Stru_temp->ProtocolType = bswap_16(0x2401);
		temp_address = *(uint32_t *)(0x1FFF7A10);
		UpgradeReq_Stru_temp->DeviceAddress = bswap_32(*(uint32_t *)(0x1FFF7A10)); //获取STM32唯一ID
		UpgradeReq_Stru_temp->DeviceType = 0x10;
		UpgradeReq_Stru_temp->DataLen = bswap_16(0x08);
		UpgradeReq_Stru_temp->cmd_Unit = 0xC0;
		//UpgradeReq_Stru_temp->HardwareVersion = bswap_32(0x00010001);
		UpgradeReq_Stru_temp->HardwareVersion[0] = 0x01;
		UpgradeReq_Stru_temp->HardwareVersion[1] = 0x00;
		UpgradeReq_Stru_temp->HardwareVersion[2] = 0x01;
		//UpgradeReq_Stru_temp->SoftVersion = bswap_32(0x00010000);
		UpgradeReq_Stru_temp->SoftVersion[0] = 0x00;
		UpgradeReq_Stru_temp->SoftVersion[1] = 0x00;
		UpgradeReq_Stru_temp->SoftVersion[2] = 0x01;
		UpgradeReq_Stru_temp->CalibrationValue = UDP_public_Sum_check(((uint8_t *)UpgradeReq_Stru_temp) + 3, sizeof(UpgradeReq_Stru) - 6);
		UpgradeReq_Stru_temp->ProtocolEnder = bswap_16(0x3535);
		log_print(DEBUG, ("recv2\r\n"));
		pData = (uint8_t *)UpgradeReq_Stru_temp;
		for(int i = 0; i <= sizeof(UpgradeResponse_Stru); i++)
		{
			 printf("%02x", pData[i]);
		}
		log_print(DEBUG, ("\r\n"));
		
		udp_UWB_senddata(udppcb, (uint8_t *)UpgradeReq_Stru_temp, sizeof(UpgradeReq_Stru));
		lwip_periodic_handle();
	}
	else
  {
    log_print(ERR, ("UpgradeReq_Stru malloc fail\r\n"));
  }
	
	myfree(SRAMIN, UpgradeReq_Stru_temp); 
}

/**
 * @:*********************************************************************************************************: 
 * @函 数 名: UDP_SendGetUpgradePacketReq
* @功能说明: 发送获取数据包请求
 * @形    参: 无
 * @返 回 值: 无
 * @**********************************************************************************************************: 
 */
void UDP_SendGetUpgradePacketReq(uint8_t n)
{
	UpgradePacket_Stru *UpgradePacket_Stru_temp = NULL;
	UpgradePacket_Stru_temp = (UpgradePacket_Stru *)mymalloc(SRAMIN, sizeof(UpgradePacket_Stru));
	if(UpgradePacket_Stru_temp != NULL)
	{
		memset((uint8_t *)UpgradePacket_Stru_temp, 0, sizeof(UpgradeReq_Stru));
		UpgradePacket_Stru_temp->ProtocolStartID = 0x24;
		UpgradePacket_Stru_temp->ProtocolType = bswap_16(0x2401);
		UpgradePacket_Stru_temp->DeviceAddress = bswap_32(*(uint32_t *)(0x1FFF7A10)); //获取STM32唯一ID
		UpgradePacket_Stru_temp->DeviceType = 0x10;
		UpgradePacket_Stru_temp->DataLen = 0x0008;
		UpgradePacket_Stru_temp->cmd_Unit = 0x88;
		UpgradePacket_Stru_temp->GetPacket_N = n;
		UpgradePacket_Stru_temp->CalibrationValue = UDP_public_Sum_check(((uint8_t *)UpgradePacket_Stru_temp) + 3, sizeof(UpgradePacket_Stru) - 6);
		UpgradePacket_Stru_temp->ProtocolEnder = bswap_16(0x3535);
	
		udp_UWB_senddata(udppcb, (uint8_t *)UpgradePacket_Stru_temp, sizeof(UpgradePacket_Stru));
		if(udp_demo_flag&1<<6)
		{
			udp_demo_flag&=~(1<<6);
		} 
		lwip_periodic_handle();
	}
	else
  {
    log_print(ERR, ("UpgradePacket_Stru malloc fail\r\n"));
  }
	
	myfree(SRAMIN, UpgradePacket_Stru_temp); 
}

/**
 * @:*********************************************************************************************************: 
 * @函 数 名: UDP_RecvData_Handle
 * @功能说明: 服务器下发数据处理
 * @形    参: 无
 * @返 回 值: 无
 * @**********************************************************************************************************: 
 */
uint8_t UDP_RecvData_Handle(void)
{
	uint8_t cmd_uint = 0;
	uint8_t CalibrationValue_temp = 0;
	uint16_t PacketData_len = 0;
	UpgradeResponse_Stru *UpgradeResponse_Stru_temp = NULL;
	UpgradePcketData_Stru *UpgradePcketData_Stru_temp = NULL;
	
	UpgradeResponse_Stru_temp = (UpgradeResponse_Stru *)mymalloc(SRAMIN, sizeof(UpgradeResponse_Stru));
	
	if(UpgradeResponse_Stru_temp != NULL)
	{
		if((udp_demo_recvbuf[0] == 0x24) &&((udp_demo_recvbuf[1] == 0x24)))
		{
			memcpy((uint8_t *)UpgradeResponse_Stru_temp, udp_demo_recvbuf, sizeof(UpgradeResponse_Stru));
			log_print(DEBUG, ("recr_udp\r\n"));
			for(int i = 0; i <= sizeof(udp_demo_recvbuf); i++)
			{
				 //printf("%02x", udp_demo_recvbuf[i]);
			}
			//log_print(DEBUG, ("\r\n"));
		}
		else
			memset((uint8_t *)UpgradeResponse_Stru_temp, 0, sizeof(UpgradeResponse_Stru));
		cmd_uint = UpgradeResponse_Stru_temp->cmd_Unit;
		PacketData_len = UpgradeResponse_Stru_temp->DataLen;
		switch(cmd_uint)
		{
			case Cmd_UpgradeStart:
			{
				// 进行数据包校验
				CalibrationValue_temp = UDP_public_Sum_check(((uint8_t *)UpgradeResponse_Stru_temp) + 3, sizeof(UpgradeResponse_Stru) - 7);
				if(CalibrationValue_temp == UpgradeResponse_Stru_temp->CalibrationValue)
				{
					UpgradeFile_Info_t.UpgradePacket_Total = UpgradeResponse_Stru_temp->UpgradePacket_Total;
					UpgradeFile_Info_t.UpgradeFile_Bytes = UpgradeResponse_Stru_temp->UpgradeFile_Bytes;
					UpgradeFile_Info_t.CalibrationValue = UpgradeResponse_Stru_temp->FileTotal_CalibrationValue;
					memcpy((uint8_t *)&E_Version_stru_temp.HardwareVersion, (uint8_t *)UpgradeResponse_Stru_temp->HardwareVersion, 3);
					memcpy((uint8_t *)&E_Version_stru_temp.SoftwareVersion, (uint8_t *)UpgradeResponse_Stru_temp->SoftVersion, 3);
					log_print(DEBUG, ("UpgradeFile_Total = %d\r\n",E_Version_stru_temp.SoftwareVersion[0]));
					log_print(DEBUG, ("UpgradeFile_Total = %d\r\n",E_Version_stru_temp.SoftwareVersion[1]));
					log_print(DEBUG, ("UpgradeFile_Total = %d\r\n",E_Version_stru_temp.SoftwareVersion[2]));
					EE_Version_Write();
					//upgrateInfo_stru_temp.upgrate_mark = 0x01;
					//upgrateHardSoft_Write();
					//upgrateInfo_Write();
					//UpgradeFile_Info_t.CalibrationValue = 0xfc;
					log_print(DEBUG, ("UpgradeFile_Total = %d\r\n",UpgradeFile_Info_t.UpgradePacket_Total));
					log_print(DEBUG, ("UpgradeFile_Bytes = %d\r\n",UpgradeFile_Info_t.UpgradeFile_Bytes));
					log_print(DEBUG, ("CalibrationValue = %d\r\n",UpgradeFile_Info_t.CalibrationValue));
					#if 1
					UpgradeFileBuf = (uint8_t *)mymalloc(SRAMIN, UpgradeFile_Info_t.UpgradeFile_Bytes);
					if(UpgradeFileBuf != NULL)
					{
						log_print(DEBUG, ("UpgradeFileBuf malloc success\r\n"));
					}
					else
					{
						log_print(DEBUG, ("UpgradeFileBuf malloc fail\r\n"));
					}
					#endif
					delay_ms(100);
					UDP_SendGetUpgradePacketReq(UpgradePacket_n);  //发送第一个数据包请求
					log_print(DEBUG, ("Cmd_UpgradeStart\r\n"));
				}
				else
				{
					log_print(DEBUG, ("Calibration fail\r\n"));
				}
				break;
			}
			case Cmd_UpgradePacket:
			{
				log_print(DEBUG, ("Cmd_UpgradePacket\r\n"));
				UpgradePcketData_Stru_temp = (UpgradePcketData_Stru *)mymalloc(SRAMIN, sizeof(UpgradePcketData_Stru));
				if(UpgradePcketData_Stru_temp != NULL)
				{
					memset((uint8_t *)UpgradePcketData_Stru_temp, 0, sizeof(UpgradePcketData_Stru));
					memcpy((uint8_t *)UpgradePcketData_Stru_temp, udp_demo_recvbuf, sizeof(UpgradePcketData_Stru));
		
					CalibrationValue_temp = UDP_public_Sum_check(((uint8_t *)UpgradePcketData_Stru_temp) + 3, sizeof(UpgradePcketData_Stru) - 1024 + PacketData_len - 7);
				  log_print(DEBUG, ("CalibrationValue = %d\r\n",UpgradePcketData_Stru_temp->CalibrationValue));
					log_print(DEBUG, ("recvbufCalibrationValue = %d\r\n",udp_demo_recvbuf[sizeof(UpgradePcketData_Stru) - 1024 + PacketData_len - 3]));
					if((CalibrationValue_temp == UpgradePcketData_Stru_temp->CalibrationValue)
						 || (CalibrationValue_temp == udp_demo_recvbuf[sizeof(UpgradePcketData_Stru) - 1024 + PacketData_len - 3]))
					{
						log_print(DEBUG, ("Packet Num:[%d], len = %d\r\n", UpgradePacket_n, PacketData_len));
						OTA_StringCopy(UpgradeFileBuf, (uint8_t *)UpgradePcketData_Stru_temp->upgradePacketData, PacketData_len, UpgradePacket_n);
					}
					else
					{
						break;
					}
				}
				else
				{
					log_print(DEBUG, ("UpgradePcketData_Stru malloc fail\r\n"));
				
				}
				UpgradePacket_n++;
				if(UpgradePacket_n < UpgradeFile_Info_t.UpgradePacket_Total)
				{
				  UDP_SendGetUpgradePacketReq(UpgradePacket_n);  //发送第一个数据包请求
				}
				else if(UpgradePacket_n == UpgradeFile_Info_t.UpgradePacket_Total)
				{
					CalibrationValue_temp = UDP_public_Sum_check(UpgradeFileBuf, UpgradeFile_Info_t.UpgradeFile_Bytes);
					if(CalibrationValue_temp == UpgradeFile_Info_t.CalibrationValue)
					{
						UDP_SendGetUpgradePacketReq(UpgradePacket_n);
						iap_write_appbin(FLASH_APP_ADDR, UpgradeFileBuf, UpgradeFile_Info_t.UpgradeFile_Bytes);//更新FLASH代码  
						
            myfree(SRAMIN, UpgradeResponse_Stru_temp); 						
						myfree(SRAMIN, UpgradeFileBuf);
						myfree(SRAMIN, UpgradePcketData_Stru_temp); 
						UpgradePacket_n = 0;
						return 1;
					}
					else
					{
						UDP_SendGetUpgradePacketReq(UpgradePacket_n + 1);
					}
				}
				else if(UpgradePacket_n == (UpgradeFile_Info_t.UpgradePacket_Total + 1))
				{
					//UDP_SendGetUpgradePacketReq(UpgradePacket_n);  //发送第一个数据包请求
				}
				myfree(SRAMIN, UpgradePcketData_Stru_temp); 
				//delay_ms(100);
				break;
			}
			default:
			{
				log_print(DEBUG, ("Invalid data\r\n"));
				break;
			}
		}
	}
	else
  {
    log_print(ERR, ("UpgradeResponse_Stru malloc fail\r\n"));
  }
	myfree(SRAMIN, UpgradeResponse_Stru_temp); 
	return 0;
}

/**
 * @:*********************************************************************************************************: 
 * @函 数 名: UDP_public_Sum_chec
 * @功能说明: 公共求和校验函数
 * @形    参: uint8_t *str:待校验字符串，str_length:待校验长度
 * @返 回 值: uint8_t XorCheck_Value
 * @**********************************************************************************************************: 
 */
uint8_t UDP_public_Sum_check(uint8_t *str, uint32_t str_length)
{
  uint8_t SumCheck_Value = 0;
  int i = 0;
  
  SumCheck_Value = str[0];
	//printf("%02x",str[0]);
  for(i = 1; i <= str_length; i++)
  {
		 //printf("%02x",str[i]);
     SumCheck_Value += str[i];
  }
  log_print(DEBUG, ("\r\nXorCheck_Value = %x\r\n",  SumCheck_Value));
  return SumCheck_Value;
}

/**
 * @:*********************************************************************************************************: 
 * @函 数 名: OTA_StringCopy
 * @功能说明: 升级包数据拼接函数
 * @形    参: From_buff:目标buff	To_buff:写入buff	Str_Len:写入长度
 * @返 回 值: 无
 * @**********************************************************************************************************: 
 */
void OTA_StringCopy(uint8_t *From_buff, uint8_t *To_buff, uint16_t Str_Len, uint8_t UpgradePacket_n)
{
	uint16_t index = 0;
	
	for(index = 0; index < Str_Len; index++)
	{
		From_buff[(UpgradePacket_n) * 1024 + index] = *To_buff;
		To_buff++;
	}
}

static inline uint16_t bswap_16(uint16_t x)
{
	return (x >> 8) | (x << 8); 
}

static inline uint32_t bswap_32(uint32_t x)
{
	x = ((x << 8) &0xFF00FF00) | ((x >> 8) &0x00FF00FF);
	return (x >> 16) | (x << 16);
}

/**
 * @:*********************************************************************************************************: 
 * @函 数 名: Upgrade_StatusWrite
 * @功能说明: 升级状态写入
 * @形    参: 无
 * @返 回 值: 返回是否需要升级
 * @**********************************************************************************************************: 
 */
int Upgrade_StatusWrite()   	
{
	char test[50] = "UpgradeFlag write to flash\n";
	STMFLASH_Write(ADDR_FLASH_SECTOR_7 ,(uint32_t *)&UpgradeFlag,sizeof(uint32_t)/4);//Ð´ÈëÒ»¸ö×Ö 
	log_print( DEBUG, ("UpgradeFlag write to flash \r\n"));
	//STMFLASH_Write(WriteAddr,&WriteData,1);//Ð´ÈëÒ»¸ö×Ö 
}
/**
 * @:*********************************************************************************************************: 
 * @函 数 名: Upgrade_Query
 * @功能说明: 升级标志查询函数
 * @形    参: 无
 * @返 回 值: 返回是否需要升级
 * @**********************************************************************************************************: 
 */
int Upgrade_Query(void)   	
{
	uint32_t read_temp = 0,write_temp = 0; 
	STMFLASH_Read(ADDR_FLASH_SECTOR_7,(uint32_t *)&read_temp,sizeof(uint32_t)/4); 
	log_print( DEBUG, ("read UpgradeFlag\r\n"));
	return read_temp;
}

void Upgrade_CleanFlag(void)
{
	uint32_t write_temp = 0; 
	STMFLASH_Write(ADDR_FLASH_SECTOR_7 ,(uint32_t *)&write_temp,sizeof(uint32_t)/4);//Ð´ÈëÒ»¸ö×Ö 
}




