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
	__set_PRIMASK(1);	
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
	__set_PRIMASK(0);	
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
	__set_PRIMASK(1);	
	u32 i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadWord(ReadAddr);//读取4个字节.
		ReadAddr+=4;//偏移4个字节.	
	}
	__set_PRIMASK(0);	
}

// 测试函数
void Test_Write(u32 WriteAddr,u32 WriteData)   	
{
	char test[50] = "stmflash test read and write\n";
	char test1[50] = { 0 };
	//char *testdata = NULL;
	//testdata = test;
	//*testdata = "stmflash test read and write\n";
	STMFLASH_Write(ADDR_FLASH_SECTOR_7 ,(uint32_t *)test,sizeof(test)/4);//写入一个字 
	delay_ms(2000);
	memset((char *)test1, 0, 50);
	STMFLASH_Read(ADDR_FLASH_SECTOR_7,(uint32_t *)test1,sizeof(test)/4); 
	log_print( DEBUG, ("test  = %s\r\n", (char *)test1));
	//STMFLASH_Write(WriteAddr,&WriteData,1);//写入一个字 
}

extern DistanceOffset_Stru DistanceOffset_Stru_temp;
void Upgrade_StatusWrite(void)   	
{
	STMFlashReadWrite_Stru STMFlashReadWrite_Stru_temp;
	memset((uint32_t *)&STMFlashReadWrite_Stru_temp, 0, sizeof(STMFlashReadWrite_Stru));
	STMFLASH_Read(ADDR_FLASH_SECTOR_7,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru_temp)/4); 
	STMFlashReadWrite_Stru_temp.UpgradeFlag = 0x24;
	STMFLASH_Write(ADDR_FLASH_SECTOR_7 ,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru)/4);//写入一个字 
	log_print( DEBUG, ("UpgradeFlag write to flash \r\n"));
}

void Upgrade_CleanWrite(void)   	
{
	STMFlashReadWrite_Stru STMFlashReadWrite_Stru_temp;
	memset((uint32_t *)&STMFlashReadWrite_Stru_temp, 0, sizeof(STMFlashReadWrite_Stru));
	STMFLASH_Read(ADDR_FLASH_SECTOR_7,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru_temp)/4); 
	log_print( DEBUG, ("UpgradeFlag = %x \r\n", STMFlashReadWrite_Stru_temp.UpgradeFlag));
	STMFlashReadWrite_Stru_temp.UpgradeFlag = 0x00;
	STMFLASH_Write(ADDR_FLASH_SECTOR_7 ,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru)/4);//写入一个字 
	
}

int Upgrade_Query(void)   	
{
	uint32_t read_temp = 0;
	STMFlashReadWrite_Stru STMFlashReadWrite_Stru_temp;
	memset((uint32_t *)&STMFlashReadWrite_Stru_temp, 0, sizeof(STMFlashReadWrite_Stru));
	STMFLASH_Read(ADDR_FLASH_SECTOR_7,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru)/4); 
	STMFlashReadWrite_Stru_temp.UpgradeFlag = 0;
	STMFLASH_Write(ADDR_FLASH_SECTOR_7 ,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru)/4);//写入一个字 
	log_print( DEBUG, ("read UpgradeFlag\r\n"));
	return read_temp;
}


/*
*******************************************************************************
*	函 数 名:  STMFLASH_HeaderDistanceOffset_Write
*	功能说明: 前跟随模式补偿参数保存
*	形	  参: 无
*	返 回 值: 无
*******************************************************************************
*/
void STMFLASH_HeaderDistanceOffset_Write(void)   	
{
	STMFlashReadWrite_Stru STMFlashReadWrite_Stru_temp;
	memset((uint32_t *)&STMFlashReadWrite_Stru_temp, 0, sizeof(STMFlashReadWrite_Stru));
	STMFLASH_Read(ADDR_FLASH_SECTOR_7,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru)/4); 
	memcpy((uint8_t *)&STMFlashReadWrite_Stru_temp.DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[0],(uint8_t *)&DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[0], 
	                                                                        sizeof(FollowModeOffset_Stru));
	STMFLASH_Write(ADDR_FLASH_SECTOR_7 ,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru)/4);//写入一个字 
}

/* 左跟随距离补偿参数保存 */
void STMFLASH_LeftDistanceOffset_Write(void)   	
{
	STMFlashReadWrite_Stru STMFlashReadWrite_Stru_temp;
	memset((uint32_t *)&STMFlashReadWrite_Stru_temp, 0, sizeof(STMFlashReadWrite_Stru));
	STMFLASH_Read(ADDR_FLASH_SECTOR_7,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru)/4); 
	memcpy((uint8_t *)&STMFlashReadWrite_Stru_temp.DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[1],(uint8_t *)&DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[1], 
	                                                                        sizeof(FollowModeOffset_Stru));
	STMFLASH_Write(ADDR_FLASH_SECTOR_7 ,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru)/4);//写入一个字 
}
/* 右跟随距离补偿参数保存 */
void STMFLASH_RightDistanceOffset_Write(void)   	
{
	STMFlashReadWrite_Stru STMFlashReadWrite_Stru_temp;
	memset((uint32_t *)&STMFlashReadWrite_Stru_temp, 0, sizeof(STMFlashReadWrite_Stru));
	STMFLASH_Read(ADDR_FLASH_SECTOR_7,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru)/4); 
	memcpy((uint8_t *)&STMFlashReadWrite_Stru_temp.DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[2],(uint8_t *)&DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[2], 
	                                                                        sizeof(FollowModeOffset_Stru));
	STMFLASH_Write(ADDR_FLASH_SECTOR_7 ,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru)/4);//写入一个字 
}

#if 0
void STMFLASH_DistanceOffset_Write(void)   	
{
	STMFlashReadWrite_Stru STMFlashReadWrite_Stru_temp;
	memset((uint32_t *)&STMFlashReadWrite_Stru_temp, 0, sizeof(STMFlashReadWrite_Stru));
	STMFLASH_Read(ADDR_FLASH_SECTOR_7,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru)/4); 
	memcpy((DistanceOffset_Stru *)&STMFlashReadWrite_Stru_temp.DistanceOffset_Stru_temp,(DistanceOffset_Stru *)&DistanceOffset_Stru_temp, sizeof(DistanceOffset_Stru));
	STMFLASH_Write(ADDR_FLASH_SECTOR_7 ,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru)/4);//写入一个字 
}
#endif

void STMFLASH_DistanceOffset_Read(void)   	
{
	STMFlashReadWrite_Stru STMFlashReadWrite_Stru_temp;
	memset((uint32_t *)&STMFlashReadWrite_Stru_temp, 0, sizeof(STMFlashReadWrite_Stru));
	STMFLASH_Read(ADDR_FLASH_SECTOR_7,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru)/4); 
	memcpy((uint8_t *)&DistanceOffset_Stru_temp, (uint8_t *)&STMFlashReadWrite_Stru_temp.DistanceOffset_Stru_temp, sizeof(DistanceOffset_Stru));
	log_print( DEBUG, ("A_HOff = %f, B = %f\r\n",DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[0].A_distance_offset,
	                                                     DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[0].B_distance_offset));
	log_print( DEBUG, ("A_LOff = %f, B = %f\r\n",DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[1].A_distance_offset,
	                                                     DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[1].B_distance_offset));
	log_print( DEBUG, ("A_ROff = %f, B = %f\r\n",DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[2].A_distance_offset,
	                                                     DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[2].B_distance_offset));
}

/* 一次性写入三组数据 */
void STMFLASH_Anchors_Distance_Write(uint8_t Anchors_Distance_Mark)   	
{
	STMFlashReadWrite_Stru STMFlashReadWrite_Stru_temp;
	memset((uint32_t *)&STMFlashReadWrite_Stru_temp, 0, sizeof(STMFlashReadWrite_Stru));
	STMFLASH_Read(ADDR_FLASH_SECTOR_7,(uint32_t *)&STMFlashReadWrite_Stru_temp, sizeof(STMFlashReadWrite_Stru)/4); 
	//DistanceOffset_Stru_temp.AnchorToAnchor_Distance[0] = AnchorToAnchor_Distance[0];
	//DistanceOffset_Stru_temp.AnchorToAnchor_Distance[1] = AnchorToAnchor_Distance[1];
	//DistanceOffset_Stru_temp.AnchorToAnchor_Distance[2] = AnchorToAnchor_Distance[2];
	DistanceOffset_Stru_temp.Anchors_Distance_Mark = Anchors_Distance_Mark;
	STMFlashReadWrite_Stru_temp.DistanceOffset_Stru_temp.AnchorToAnchor_Distance[0] = AnchorToAnchor_Distance[0];
	STMFlashReadWrite_Stru_temp.DistanceOffset_Stru_temp.AnchorToAnchor_Distance[1] = AnchorToAnchor_Distance[1];
	STMFlashReadWrite_Stru_temp.DistanceOffset_Stru_temp.AnchorToAnchor_Distance[2] = AnchorToAnchor_Distance[2];
	//memcpy((DistanceOffset_Stru *)&STMFlashReadWrite_Stru_temp.DistanceOffset_Stru_temp, (DistanceOffset_Stru *)&DistanceOffset_Stru_temp, sizeof(DistanceOffset_Stru));
	STMFLASH_Write(ADDR_FLASH_SECTOR_7 ,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru)/4);//写入一个字 
}

uint8_t STMFLASH_Anchors_Distance_Read(void)   	
{
	uint8_t Anchors_Distance_Mark;
	STMFlashReadWrite_Stru STMFlashReadWrite_Stru_temp;
	memset((uint32_t *)&STMFlashReadWrite_Stru_temp, 0, sizeof(STMFlashReadWrite_Stru));
	STMFLASH_Read(ADDR_FLASH_SECTOR_7,(uint32_t *)&STMFlashReadWrite_Stru_temp, sizeof(STMFlashReadWrite_Stru)/4); 
	AnchorToAnchor_Distance[0] = STMFlashReadWrite_Stru_temp.DistanceOffset_Stru_temp.AnchorToAnchor_Distance[0];
	AnchorToAnchor_Distance[1] = STMFlashReadWrite_Stru_temp.DistanceOffset_Stru_temp.AnchorToAnchor_Distance[1];
	AnchorToAnchor_Distance[2] = STMFlashReadWrite_Stru_temp.DistanceOffset_Stru_temp.AnchorToAnchor_Distance[2];
	Anchors_Distance_Mark = STMFlashReadWrite_Stru_temp.DistanceOffset_Stru_temp.Anchors_Distance_Mark;
	return Anchors_Distance_Mark;
}

// 网络参数的写入
extern Network_Manage *Network_Manage_pbuf;
void STMFLASH_NetworkPara_Config_Write(Network_Manage Network_Manage_pbuf)
{
	STMFlashReadWrite_Stru STMFlashReadWrite_Stru_temp;
	memset((uint32_t *)&STMFlashReadWrite_Stru_temp, 0, sizeof(STMFlashReadWrite_Stru));
	STMFLASH_Read(ADDR_FLASH_SECTOR_7,(uint32_t *)&STMFlashReadWrite_Stru_temp, sizeof(STMFlashReadWrite_Stru)/4); 
	memcpy((uint8_t *)&STMFlashReadWrite_Stru_temp.Network_Manage_temp,(uint8_t *)&Network_Manage_pbuf, sizeof(Network_Manage));
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

void STMFLASH_Write_test(void)
{
	Upgrade_StatusWrite();
	Upgrade_CleanWrite();
	AnchorToAnchor_Distance[0] = 1200;
	AnchorToAnchor_Distance[1] = 1300;
	AnchorToAnchor_Distance[2] = 1400;
	STMFLASH_Anchors_Distance_Write(0x01);
	STMFLASH_Anchors_Distance_Read();
  log_print( DEBUG, ("H_dis = %f,L_dis = %f,R_dis = %f\r\n", AnchorToAnchor_Distance[0],AnchorToAnchor_Distance[1],AnchorToAnchor_Distance[2]));
	DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[0].FollowOffset_Mark = 0x01;
	DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[0].FollowMode = 0x01;  	
	DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[0].A_distance_offset =  567;
	DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[0].B_distance_offset =  789;
	STMFLASH_HeaderDistanceOffset_Write();
	DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[1].FollowOffset_Mark = 0x04;
	DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[1].FollowMode = 0x02;  	
	DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[1].A_distance_offset =  123;
	DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[1].B_distance_offset =  456;
	STMFLASH_LeftDistanceOffset_Write();
	DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[2].FollowOffset_Mark = 0x05;
	DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[2].FollowMode = 0x03;  	
	DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[2].A_distance_offset =  1576;
	DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[2].B_distance_offset =  489;
	STMFLASH_RightDistanceOffset_Write();
	STMFLASH_DistanceOffset_Read();
	
		//默认远端IP为:192.168.1.100
	Network_Manage_t.remoteip[0] = 110;	
	Network_Manage_t.remoteip[1] = 112;
	Network_Manage_t.remoteip[2] = 1;
	Network_Manage_t.remoteip[3] = 113;
	Network_Manage_t.remote_port = 6008;
	
	Network_Manage_t.ip[0] = 114;	
	Network_Manage_t.ip[1] = 115;
	Network_Manage_t.ip[2] = 7;
	Network_Manage_t.ip[3] = 102;
	Network_Manage_t.local_port = 6004;
	STMFLASH_NetworkPara_Config_Write(Network_Manage_t);
	Network_Manage_t.remoteip[0] = 0;	
	Network_Manage_t.remoteip[1] = 0;
	Network_Manage_t.remoteip[2] = 0;
	Network_Manage_t.remoteip[3] = 0;
	Network_Manage_t.remote_port = 0;
	
	Network_Manage_t.ip[0] = 0;	
	Network_Manage_t.ip[1] = 0;
	Network_Manage_t.ip[2] = 0;
	Network_Manage_t.ip[3] = 0;
	Network_Manage_t.local_port = 6009;
	STMFLASH_NetworkPara_Config_Read();
	printf("Static IP........................%d.%d.%d.%d\r\n",Network_Manage_t.ip[0],Network_Manage_t.ip[1],Network_Manage_t.ip[2],Network_Manage_t.ip[3]);
	printf("Remote IP..........................%d.%d.%d.%d\r\n",Network_Manage_t.remoteip[0],Network_Manage_t.remoteip[1],
	                                                            Network_Manage_t.remoteip[2],Network_Manage_t.remoteip[3]);
	printf("local_port = %d, remote_port = %d\r\n",Network_Manage_t.local_port,Network_Manage_t.remote_port);																												 
	while(1);
}


