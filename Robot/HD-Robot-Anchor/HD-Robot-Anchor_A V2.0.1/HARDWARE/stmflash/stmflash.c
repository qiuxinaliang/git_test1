#include "stmflash.h"

uint32_t UpgradeFlag;
/*
*******************************************************************************
*	函 数 名:  STMFLASH_ReadWord
*	功能说明: 读取指定地址的字(32位数据)
*	形	  参: faddr:读地址
*	返 回 值: 对应数据
*******************************************************************************
*/
u32 STMFLASH_ReadWord(u32 faddr)
{
	return *(__IO uint32_t *)faddr; 
}

/*
*******************************************************************************
*	函 数 名:  STMFLASH_GetFlashSector
*	功能说明: 获取某个地址所在的flash扇区
*	形	  参: addr:flash地址
*	返 回 值: 0~11,即addr所在的扇区
*******************************************************************************
*/
u8 STMFLASH_GetFlashSector(u32 addr)
{
	if(addr<ADDR_FLASH_SECTOR_1)return FLASH_SECTOR_0;
	else if(addr<ADDR_FLASH_SECTOR_2)return FLASH_SECTOR_1;
	else if(addr<ADDR_FLASH_SECTOR_3)return FLASH_SECTOR_2;
	else if(addr<ADDR_FLASH_SECTOR_4)return FLASH_SECTOR_3;
	else if(addr<ADDR_FLASH_SECTOR_5)return FLASH_SECTOR_4;
	else if(addr<ADDR_FLASH_SECTOR_6)return FLASH_SECTOR_5;
	else if(addr<ADDR_FLASH_SECTOR_7)return FLASH_SECTOR_6;
	else if(addr<ADDR_FLASH_SECTOR_8)return FLASH_SECTOR_7;
	else if(addr<ADDR_FLASH_SECTOR_9)return FLASH_SECTOR_8;
	else if(addr<ADDR_FLASH_SECTOR_10)return FLASH_SECTOR_9;
	else if(addr<ADDR_FLASH_SECTOR_11)return FLASH_SECTOR_10; 
	else if(addr<ADDR_FLASH_SECTOR_12)return FLASH_SECTOR_11; 
	else if(addr<ADDR_FLASH_SECTOR_13)return FLASH_SECTOR_12; 
	else if(addr<ADDR_FLASH_SECTOR_14)return FLASH_SECTOR_13; 
	else if(addr<ADDR_FLASH_SECTOR_15)return FLASH_SECTOR_14; 
	else if(addr<ADDR_FLASH_SECTOR_16)return FLASH_SECTOR_15; 
	else if(addr<ADDR_FLASH_SECTOR_17)return FLASH_SECTOR_16; 
	else if(addr<ADDR_FLASH_SECTOR_18)return FLASH_SECTOR_17; 
	else if(addr<ADDR_FLASH_SECTOR_19)return FLASH_SECTOR_18; 
	else if(addr<ADDR_FLASH_SECTOR_20)return FLASH_SECTOR_19; 
	else if(addr<ADDR_FLASH_SECTOR_21)return FLASH_SECTOR_20; 
	else if(addr<ADDR_FLASH_SECTOR_22)return FLASH_SECTOR_21; 
	else if(addr<ADDR_FLASH_SECTOR_23)return FLASH_SECTOR_22;  
	return FLASH_SECTOR_23;	
}

/*
*******************************************************************************
*	函 数 名:  STMFLASH_Write
*	功能说明: 从指定地址开始写入指定长度的数据
* 特别注意:因为STM32F4的扇区实在太大,没办法本地保存扇区数据,所以本函数
*         写地址如果非0XFF,那么会先擦除整个扇区且不保存扇区数据.所以
*         写非0XFF的地址,将导致整个扇区数据丢失.建议写之前确保扇区里
*         没有重要数据,最好是整个扇区先擦除了,然后慢慢往后写. 
*         该函数对OTP区域也有效!可以用来写OTP区!
*         OTP区域地址范围:0X1FFF7800~0X1FFF7A0F(注意：最后16字节，用于OTP数据块锁定，别乱写！！)
*	形	  参: WriteAddr:起始地址(此地址必须为4的倍数!!),pBuffer:数据指针,NumToWrite:字(32位)
*           数(就是要写入的32位数据的个数.) 
*	返 回 值: 无
*******************************************************************************
*/
void STMFLASH_Write(u32 WriteAddr,u32 *pBuffer,u32 NumToWrite)	
{ 
	FLASH_EraseInitTypeDef FlashEraseInit;
	HAL_StatusTypeDef FlashStatus=HAL_OK;
	u32 SectorError=0;
	u32 addrx=0;
	u32 endaddr=0;	
  if(WriteAddr<STM32_FLASH_BASE||WriteAddr%4)return;	//非法地址
    
 	HAL_FLASH_Unlock();             //解锁	
	addrx=WriteAddr;				//写入的起始地址
	endaddr=WriteAddr+NumToWrite*4;	//写入的结束地址
    
	if(addrx<0X1FFF0000)
	{
		while(addrx<endaddr)		//扫清一切障碍.(对非FFFFFFFF的地方,先擦除)
		{
			if(STMFLASH_ReadWord(addrx)!=0XFFFFFFFF)//有非0XFFFFFFFF的地方,要擦除这个扇区
			{   
				FlashEraseInit.TypeErase=FLASH_TYPEERASE_SECTORS;       //擦除类型，扇区擦除 
				FlashEraseInit.Sector=STMFLASH_GetFlashSector(addrx);   //要擦除的扇区
				FlashEraseInit.NbSectors=1;                             //一次只擦除一个扇区
				FlashEraseInit.VoltageRange=FLASH_VOLTAGE_RANGE_3;      //电压范围，VCC=2.7~3.6V之间!!
				if(HAL_FLASHEx_Erase(&FlashEraseInit,&SectorError)!=HAL_OK) 
				{
					break;//发生错误了	
				}
			}
			else addrx+=4;
			FLASH_WaitForLastOperation(FLASH_WAITETIME);                //等待上次操作完成
		}
	}
	FlashStatus=FLASH_WaitForLastOperation(FLASH_WAITETIME);            //等待上次操作完成
	if(FlashStatus==HAL_OK)
	{
		while(WriteAddr<endaddr)//写数据
		{
			if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,WriteAddr,*pBuffer)!=HAL_OK)//写入数据
			{ 
				break;	//写入异常
			}
			WriteAddr+=4;
			pBuffer++;
		} 
	}
	HAL_FLASH_Lock();           //上锁
} 

/*
*******************************************************************************
*	函 数 名:  STMFLASH_Read
*	功能说明: 从指定地址开始读出指定长度的数据
*	形	  参: ReadAddr:起始地址,pBuffer:数据指针,NumToRead:字(32位)数
*	返 回 值: 无
*******************************************************************************
*/
void STMFLASH_Read(u32 ReadAddr,u32 *pBuffer,u32 NumToRead)   	
{
	u32 i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadWord(ReadAddr);//读取4个字节.
		ReadAddr+=4;//偏移4个字节.	
	}
}
// 测试函数
void Test_Write(void)   	
{
  uint8_t Anchor_UpgradeFlag_t;
  uint8_t Anchor_address_t;
  uint8_t Anchor_Workmode_t;
  Network_Manage Network_Manage_tt;
  STMFlashReadWrite_Stru STMFlashReadWrite_Stru_temp;
  Anchor_UpgradeFlag_t = 0x01;
  Anchor_address_t = 0x05;
  Anchor_Workmode_t = 0x04;
  Network_Manage_tt.ip[0] = 192;
  Network_Manage_tt.ip[1] = 168;
  Network_Manage_tt.ip[2] = 1;
  Network_Manage_tt.ip[3] = 105;
  Network_Manage_tt.local_port = 6008;
  Network_Manage_tt.remoteip[0] = 193;
  Network_Manage_tt.remoteip[1] = 167;
  Network_Manage_tt.remoteip[2] = 2;
  Network_Manage_tt.remoteip[3] = 106;
  Network_Manage_tt.remote_port = 6007;
  Network_Manage_tt.net_mark = 0x03;

  Dev_Upgrade_Write(Anchor_UpgradeFlag_t);
  UWB_AnchorID_Write(Anchor_address_t);
  Dev_Workmode_Write(Anchor_Workmode_t);
  STMFLASH_NetworkPara_Config_Write((Network_Manage *)&Network_Manage_tt);
  Anchor_UpgradeFlag_t = 0x0;
  Anchor_address_t = 0x0;
  Anchor_Workmode_t = 0x0;
  Network_Manage_tt.ip[0] = 0;
  Network_Manage_tt.ip[1] = 0;
  Network_Manage_tt.ip[2] = 0;
  Network_Manage_tt.ip[3] = 0;
  Network_Manage_tt.local_port = 0;
  Network_Manage_tt.remoteip[0] = 0;
  Network_Manage_tt.remoteip[0] = 0;
  Network_Manage_tt.remoteip[0] = 0;
  Network_Manage_tt.remoteip[0] = 0;
  Network_Manage_tt.remote_port = 0;
  Network_Manage_tt.net_mark = 0x0;
  Anchor_UpgradeFlag_t = Dev_Upgrade_Read();
  Anchor_address_t = UWB_AnchorID_Read();
  Anchor_Workmode_t = Dev_Workmode_Read();
  
	memset((uint32_t *)&STMFlashReadWrite_Stru_temp, 0, sizeof(STMFlashReadWrite_Stru));
	STMFLASH_Read(ADDR_FLASH_SECTOR_7,(uint32_t *)&STMFlashReadWrite_Stru_temp, sizeof(STMFlashReadWrite_Stru)/4); 
	memcpy((uint8_t *)&Network_Manage_tt, (uint8_t *)&STMFlashReadWrite_Stru_temp.Network_Manage_temp, sizeof(Network_Manage));
  log_print(DEBUG,("UpgradeFlag = %d, address = %d, Workmode = %d", Anchor_UpgradeFlag_t, Anchor_address_t,Anchor_Workmode_t)); 
  log_print(DEBUG, ("Static IP........................%d.%d.%d.%d\r\n",Network_Manage_tt.ip[0],Network_Manage_tt.ip[1],Network_Manage_tt.ip[2],Network_Manage_tt.ip[3]));
  log_print(DEBUG, ("Static IP........................%d.%d.%d.%d\r\n",Network_Manage_tt.remoteip[0],Network_Manage_tt.remoteip[1],
                                                                       Network_Manage_tt.remoteip[2],Network_Manage_tt.remoteip[3]));
  log_print(DEBUG,("LocalPort = %d, RemotePort = %d", Network_Manage_tt.local_port, Network_Manage_tt.remote_port)); 
	log_print(DEBUG,("sizeof = %d\r\n", sizeof(STMFlashReadWrite_Stru_temp)));
	
}

uint8_t Anchor_UpgradeFlag;
uint8_t Anchor_address;
uint8_t Anchor_Workmode;

void Dev_Upgrade_Write(uint8_t UpgradeFlag)   	
{
	STMFlashReadWrite_Stru STMFlashReadWrite_Stru_temp;
	memset((uint32_t *)&STMFlashReadWrite_Stru_temp, 0, sizeof(STMFlashReadWrite_Stru));
	STMFLASH_Read(ADDR_FLASH_SECTOR_7,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru_temp)/4); 
	STMFlashReadWrite_Stru_temp.UpgradeFlag = UpgradeFlag;
	STMFLASH_Write(ADDR_FLASH_SECTOR_7 ,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru));//写入一个字 
	log_print( DEBUG, ("UpgradeFlag write to flash \r\n"));
}

uint8_t Dev_Upgrade_Read(void)   	
{
	STMFlashReadWrite_Stru STMFlashReadWrite_Stru_temp;
	memset((uint32_t *)&STMFlashReadWrite_Stru_temp, 0, sizeof(STMFlashReadWrite_Stru));
	STMFLASH_Read(ADDR_FLASH_SECTOR_7,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru_temp)/4); 
	log_print(DEBUG, ("Upgrade = %d\r\n", STMFlashReadWrite_Stru_temp.UpgradeFlag));
	return 	STMFlashReadWrite_Stru_temp.UpgradeFlag;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void UWB_AnchorID_Write(uint8_t Address)   	
{
	STMFlashReadWrite_Stru STMFlashReadWrite_Stru_temp;
	memset((uint32_t *)&STMFlashReadWrite_Stru_temp, 0, sizeof(STMFlashReadWrite_Stru));
	STMFLASH_Read(ADDR_FLASH_SECTOR_7,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru_temp)/4); 
	STMFlashReadWrite_Stru_temp.UWB_Anchor_address = Address;
	STMFLASH_Write(ADDR_FLASH_SECTOR_7 ,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru));//写入一个字 
	log_print( DEBUG, ("UWBAnchorID write to flash \r\n"));
}

uint8_t UWB_AnchorID_Read(void)   	
{
	STMFlashReadWrite_Stru STMFlashReadWrite_Stru_temp;
	memset((uint32_t *)&STMFlashReadWrite_Stru_temp, 0, sizeof(STMFlashReadWrite_Stru));
	STMFLASH_Read(ADDR_FLASH_SECTOR_7,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru_temp)/4); 
	log_print(DEBUG, ("UWBAnchorID = %d\r\n", STMFlashReadWrite_Stru_temp.UWB_Anchor_address));
	return STMFlashReadWrite_Stru_temp.UWB_Anchor_address;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Dev_Workmode_Write(uint8_t WorkingMode)   	
{
	STMFlashReadWrite_Stru STMFlashReadWrite_Stru_temp;
	memset((uint32_t *)&STMFlashReadWrite_Stru_temp, 0, sizeof(STMFlashReadWrite_Stru));
	STMFLASH_Read(ADDR_FLASH_SECTOR_7,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru_temp)/4); 
	STMFlashReadWrite_Stru_temp.WorkingMode = WorkingMode;
	STMFLASH_Write(ADDR_FLASH_SECTOR_7 ,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru));//写入一个字 
	log_print( DEBUG, ("WorkMode write to flash \r\n"));
}

uint8_t Dev_Workmode_Read(void)   	
{
	STMFlashReadWrite_Stru STMFlashReadWrite_Stru_temp;
	memset((uint32_t *)&STMFlashReadWrite_Stru_temp, 0, sizeof(STMFlashReadWrite_Stru));
	STMFLASH_Read(ADDR_FLASH_SECTOR_7,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru_temp)/4); 
	log_print(DEBUG, ("Workmode = %d\r\n", STMFlashReadWrite_Stru_temp.WorkingMode));
	return STMFlashReadWrite_Stru_temp.WorkingMode;
}

// 网络参数的写入
extern Network_Manage *Network_Manage_pbuf;
void STMFLASH_NetworkPara_Config_Write(Network_Manage *Network_Manage_pbuf)
{
	STMFlashReadWrite_Stru STMFlashReadWrite_Stru_temp;
	memset((uint32_t *)&STMFlashReadWrite_Stru_temp, 0, sizeof(STMFlashReadWrite_Stru));
	STMFLASH_Read(ADDR_FLASH_SECTOR_7,(uint32_t *)&STMFlashReadWrite_Stru_temp, sizeof(STMFlashReadWrite_Stru)/4); 
	memcpy((uint8_t *)&STMFlashReadWrite_Stru_temp.Network_Manage_temp,(uint8_t *)Network_Manage_pbuf, sizeof(Network_Manage));
	STMFLASH_Write(ADDR_FLASH_SECTOR_7 ,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru)/4);//写入一个字 
}

extern Network_Manage Network_Manage_t;
uint8_t STMFLASH_NetworkPara_Config_Read(void)   	
{
	STMFlashReadWrite_Stru STMFlashReadWrite_Stru_temp;
	memset((uint32_t *)&STMFlashReadWrite_Stru_temp, 0, sizeof(STMFlashReadWrite_Stru));
	STMFLASH_Read(ADDR_FLASH_SECTOR_7,(uint32_t *)&STMFlashReadWrite_Stru_temp, sizeof(STMFlashReadWrite_Stru)/4); 
	memcpy((uint8_t *)&Network_Manage_t, (uint8_t *)&STMFlashReadWrite_Stru_temp.Network_Manage_temp, sizeof(Network_Manage));
	return Network_Manage_t.net_mark;
}



