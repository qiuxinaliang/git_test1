/*
 * @:*********************************************************************************************************: 
 * @Date: 2020-05-25 16:58:51
 * @LastEditTime: 2020-10-12 09:50:04
 * @**********************************************************************************************************: 
 */
#ifndef __STMFLASH_H
#define __STMFLASH_H
#include "common.h"

//FLASH起始地址
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH的起始地址
#define FLASH_WAITETIME  50000          //FLASH等待超时时间

//FLASH 扇区的起始地址
#define ADDR_FLASH_SECTOR_0     ((u32)0x08000000) 	//扇区0起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_1     ((u32)0x08004000) 	//扇区1起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_2     ((u32)0x08008000) 	//扇区2起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_3     ((u32)0x0800C000) 	//扇区3起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_4     ((u32)0x08010000) 	//扇区4起始地址, 64 Kbytes  
#define ADDR_FLASH_SECTOR_5     ((u32)0x08020000) 	//扇区5起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_6     ((u32)0x08040000) 	//扇区6起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_7     ((u32)0x08060000) 	//扇区7起始地址, 128 Kbytes  

#define ADDR_FLASH_SECTOR_8     ((u32)0x08080000) 	//扇区8起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_9     ((u32)0x080A0000) 	//扇区9起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_10    ((u32)0x080C0000) 	//扇区10起始地址,128 Kbytes  
#define ADDR_FLASH_SECTOR_11    ((u32)0x080E0000) 	//扇区11起始地址,128 Kbytes 

#define ADDR_FLASH_SECTOR_12	((u32)0x08100000) 	//扇区12起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_13	((u32)0x08104000) 	//扇区13起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_14    ((u32)0x08108000) 	//扇区14起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_15	((u32)0x0810C000) 	//扇区15起始地址, 16 Kbytes  
#define ADDR_FLASH_SECTOR_16    ((u32)0x08110000) 	//扇区16起始地址, 64 Kbytes  
#define ADDR_FLASH_SECTOR_17	((u32)0x08120000) 	//扇区17起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_18	((u32)0x08140000) 	//扇区18起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_19	((u32)0x08160000) 	//扇区19起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_20    ((u32)0x08180000) 	//扇区20起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_21	((u32)0x081A0000) 	//扇区21起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_22    ((u32)0x081C0000) 	//扇区22起始地址, 128 Kbytes  
#define ADDR_FLASH_SECTOR_23    ((u32)0x081E0000) 	//扇区23起始地址, 128 Kbytes   

#if 1
#pragma pack(1)
typedef struct DistanceOffset_Stru
{
	uint8_t tag_id;
	uint8_t A_anchor_id;
	uint8_t B_anchor_id;
	uint8_t Anchors_Distance_Mark;
	uint8_t BatteryVoltage;
}DistanceOffset_Stru;
#pragma pack()
#endif

#pragma pack(1)  
typedef struct 
{
	uint8_t net_mark;
	uint8_t remoteip[4];	//远端主机IP地址 
	uint16_t remote_port;
	uint8_t ip[4];       //本机IP地址
	uint16_t local_port;
}Network_Manage;
#pragma pack()

#pragma pack(1)
typedef struct STMFlashReadWrite_Stru
{
	uint8_t UpgradeFlag; // 升级状态标志
	uint8_t UWB_Anchor_address;
  //DistanceOffset_Stru DistanceOffset_Stru_temp; //测距补偿值保存
	//int32_t TX_RX_delay_offset;	 // 天线延时值保存
  uint8_t WorkingMode;  // 基站跟随模式设置
	Network_Manage Network_Manage_temp; // 网络参数保存
	//AnchorFollowMode_Stru AnchorFollowMode_Stru_temp;
}STMFlashReadWrite_Stru;
#pragma pack()
 
u32 STMFLASH_ReadWord(u32 faddr);		  	//读出字  
void STMFLASH_Write(u32 WriteAddr,u32 *pBuffer,u32 NumToWrite);		//从指定地址开始写入指定长度的数据
void STMFLASH_Read(u32 ReadAddr,u32 *pBuffer,u32 NumToRead);   		//从指定地址开始读出指定长度的数据
void Test_Write(u32 WriteAddr,u32 WriteData);  
uint8_t STMFLASH_NetworkPara_Config_Read(void);
uint8_t Dev_Upgrade_Read(void);
#endif
