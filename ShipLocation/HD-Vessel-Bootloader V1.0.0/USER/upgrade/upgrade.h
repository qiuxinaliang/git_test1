/*
 * @:*********************************************************************************************************: 
 * @Date: 2020-10-12 13:40:54
 * @LastEditTime: 2020-10-12 15:29:03
 * @**********************************************************************************************************: 
 */
#ifndef __UPGRADE_H
#define __UPGRADE_H

#include "common.h"

#define Cmd_UpgradeStart	0xC1
#define Cmd_UpgradePacket	0x88
#define FLASH_APP_ADDR		0x08010000		//保留0X08000000~0X0800FFFF的空间为Bootloader使用(共64KB)
extern uint32_t UpgradeFlag;
#pragma pack(1)
typedef struct
{
	uint8_t ProtocolStartID;
	uint16_t ProtocolType;
	uint32_t DeviceAddress;
	uint8_t DeviceType;		//软件升级写入地址
	uint16_t DataLen;
	uint8_t cmd_Unit;
	uint8_t HardwareVersion[3];	//软件版本
	uint8_t SoftVersion[3];	//软件版本
	uint8_t CalibrationValue;
	uint16_t ProtocolEnder;
}UpgradeReq_Stru;
#pragma pack()

// 请求响应数据包
#pragma pack(1)
typedef struct
{
	uint8_t ProtocolStartID;
	uint16_t ProtocolType;
	uint32_t DeviceAddress;
	uint8_t DeviceType;		//软件升级写入地址
	uint16_t DataLen;
	uint8_t cmd_Unit;
	uint32_t UpgradePacket_Total;	
	uint32_t UpgradeFile_Bytes;
	uint8_t FileTotal_CalibrationValue;
	uint8_t HardwareVersion[3];	//软件版本
	uint8_t SoftVersion[3];			//软件版本
	uint8_t CalibrationValue;
	uint16_t ProtocolEnder;
}UpgradeResponse_Stru;
#pragma pack()

// 获取数据包请求
#pragma pack(1)
typedef struct
{
	uint8_t ProtocolStartID;
	uint16_t ProtocolType;
	uint32_t DeviceAddress;
	uint8_t DeviceType;		//软件升级写入地址
	uint16_t DataLen;
	uint8_t cmd_Unit;
	uint16_t GetPacket_N;
	uint8_t CalibrationValue;
	uint16_t ProtocolEnder;
}UpgradePacket_Stru;
#pragma pack()

#if 1
// 升级数据包下发
#pragma pack(1)
typedef struct
{
	uint8_t ProtocolStartID;
	uint16_t ProtocolType;
	
	uint32_t DeviceAddress;
	uint8_t DeviceType;		//软件升级写入地址
	uint16_t DataLen;
	uint8_t cmd_Unit;
	//T_upgradePacketData upgradePacketData;
	uint8_t upgradePacketData[1024];
	uint8_t CalibrationValue;
	uint16_t ProtocolEnder;
}UpgradePcketData_Stru;
#pragma pack()
#endif
// 升级信息结构体
#pragma pack(1)
typedef struct
{
	uint8_t HardwareVersion[3];	//软件版本
	uint8_t SoftVersion[3];	//软件版本
	uint32_t UpgradePacket_Total;	
	uint32_t UpgradeFile_Bytes;
	uint8_t CalibrationValue; 
}UpgradeFile_Info;
#pragma pack()

extern  uint8_t *UpgradeFileBuf;
extern uint8_t UpgradePacket_n;
extern UpgradeFile_Info UpgradeFile_Info_t;
void BootLoad_to_App(uint32_t AppAddr);
void iap_write_appbin(u32 appxaddr,u8 *appbuf,u32 appsize);
void UDP_SendUpgradeReq(void);
void UDP_SendGetUpgradePacketReq(uint8_t n);
uint8_t UDP_RecvData_Handle(void);
uint8_t UDP_public_Sum_check(uint8_t *str, uint32_t str_length);
void OTA_StringCopy(uint8_t *From_buff, uint8_t *To_buff, uint16_t Str_Len, uint8_t UpgradePacket_n);
int Upgrade_Query(void);
void Upgrade_CleanFlag(void);

#endif