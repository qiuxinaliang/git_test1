/*
 * @:*********************************************************************************************************: 
 * @Date: 2020-11-04 15:41:21
 * @LastEditTime: 2020-12-04 14:04:26
 * @**********************************************************************************************************: 
 */
#ifndef __SERVERTOANCHOR__H__
#define __SERVERTOANCHOR__H__

#include "common.h"
// 命令单元
typedef enum 
{
  Cmd_HeartPacket = 0x00,
  Cmd_ServerOrAnchorAck = 0x01,
  Cmd_SendDeviceArrti = 0x02,
  Cmd_DeviceSendAlarmInfo = 0x03,
  Cmd_DeviceSendInfo = 0x04,
  Cmd_AnchorSendUWBInfo = 0x05,
  Cmd_DataPacketFlag = 0x88,
  Cmd_RequestUpgrateFlag = 0xC0,
  Cmd_RequestUpgrateInfoFlag = 0xC1,
  Cmd_ServerSendNotice = 0xC2,
  Cmd_ServerSendDeviceConfig = 0xC3,
  Cmd_ServerSendNetworkConfig = 0xC4,
  Cmd_ServerTakeDeviceInfo = 0xC6,
  Cmd_UpdateTimeRequest = 0xC7,
  Cmd_ServerFindAnchorInfo = 0xC8,
  Cmd_ServerSendRegular = 0xFF
}ProtocolCmdCode;

// 属性ID
typedef enum
{
  DeviceID = 0x01,
  SoftwareVersionID = 0x02,
  HardwareVersionID = 0x03,
  BatteryLevelID = 0x04,
  BatteryVoltageID = 0x05,
  PowerOnTypeID = 0x06,
  HeartRateID = 0x11,
  StempNumID = 0x12,
  LocationTypeID = 0x13,
  GPS_LongitudeID = 0x15,
  GPS_LatitudeID = 0x17,
  GPS_SpeedID = 0x18,
  PostionTimeStampID = 0x20,
  ReportTimeStampID = 0x21,
  ReportFreqID = 0x22,
  DeviceTaskStateID = 0x23,
  PoliceTypeID = 0x24,
  ChargeStateID = 0x26,
  SubnetmaskID = 0x27,
  GatewayID = 0x28,
  CenteripaddressID = 0x29,
  CenterPortID = 0x30,
  Msg_ID = 0x31,
  DataFrameStateID = 0x32,
  CmdAckID = 0x33,
  EngineipaddressID = 0x34,
  EnginePortID = 0x35,
  AnchoripaddressID = 0x36,
  SelfCheckID = 0x37,
  AnchorCtrlResponseID = 0x38,
  AnchorPortID = 0x39,
  UWBInfoID = 0x42,
  SyncTimeStampID = 0x43,
  TargetDeviceTypeID = 0x44,
  AnchorWorkModeID = 0x45,
  NFCInfoID = 0x46,
  MessageID = 0x50,
  WatchFlagID = 0x51,
  AnchorIPID = 0x52,
  WatchLevelID = 0x53,
  WatchVoltageID = 0x54,
  ReleaseAlarmID = 0x55,
  DeviceCtrlID = 0x60
}ProtocolAttributeID;

// 设备开机类型
typedef enum 
{
  PowerOnOpen = 0x01,
  SysReboot = 0x02,
  IwdgeReboot = 0x03,
  NetCloseReboot = 0x04
}PowerOnType;

// 基站工作模式
typedef enum
{
  UWB_Postion_Mode = 0x01,
  Lora_Commun_Mode = 0x02,
  UWB_Lora_Mode = 0x03
}AnchorWorkMode;

// 设备类型
typedef enum 
{
  SeaPosJianZhang_Up = 0x01,
  SeaPosJianZhang_Down = 0x02,
  SeaLoraAnchor_UP = 0x03,
  SeaLoraAnchor_Down = 0x04,
  SeaUWBAnchor_UP = 0x05,
  SeaUWBAnchor_Down = 0x06,
  SeaUWBAnchor = 0x10,
  SeaLoraAnchor= 0x11
}DeviceType;

#define UsrProtocolStartID 0x24
#define UsrProtocolType    0x2401
#define UsrDeviceType      0x03
#define UsrDeviceAddress  0x1FFF7A10
#define UsrProtocolEnder  0x3535
#define UpdateTimeReqReponse 0x43
#define PowerOnMessageReponse 0x02
#define ExceptionTaskSuspend 0x44
/* 协议框架起始标志 + 控制单元 */
#pragma pack(1)
typedef struct
{
	uint8_t ProtocolStartID;    //0x24 
	uint16_t ProtocolType;      //0x2401
	uint32_t DeviceAddress;
	uint8_t DeviceType;
	uint16_t DataLen;
	uint8_t cmd_Unit;
}ProtocolFrameCtrlUint;
#pragma pack()

/* 协议框架中的校验和 + 结束标志 */
#pragma pack(1)
typedef struct
{
	uint8_t CalibrationValue;
	uint16_t ProtocolEnder;  // 0x3535
}ProtocolFrameEnderUint;
#pragma pack()

/* 协议框架中的开机信息结构体 */
#pragma pack(1)
typedef struct
{
  ProtocolFrameCtrlUint ProtocolFrameCtrlUint_Temp;
  uint16_t AttributePairsNum;
  uint8_t SoftwareVersionID;
  uint8_t SoftwareVersion[3];
  uint8_t HardwareVersionID;
  uint8_t HardwareVersion[3];
  uint8_t Reserved1ID;
  uint16_t Reserved1;
  uint8_t Reserved2ID;
  uint16_t Reserved2;
  uint8_t PowerOnTypeID;
  uint8_t PowerOnType;
  uint8_t TimeStampID;
  uint32_t TimeStamp;
  uint8_t SelfCheckID;
  uint8_t SelfCheck;
  uint8_t AnchorWorkModeID;
  uint8_t AnchorWorkMode;
	ProtocolFrameEnderUint ProtocolFrameEnderUint_Temp;
}DevicePowerOn_Stru;
#pragma pack()

/* 开机信息服务器响应结构体 */
#pragma pack(1)
typedef struct
{
  ProtocolFrameCtrlUint ProtocolFrameCtrlUint_Temp;
  uint16_t AttributePairsNum;
  uint8_t DataFrameStateID;
  uint8_t DataFrameState;
  uint8_t CmdAckID;
  uint8_t CmdAck;
  uint8_t TimeStampID;
  uint32_t TimeStamp;
	ProtocolFrameEnderUint ProtocolFrameEnderUint_Temp;
}Response_Stru;
#pragma pack()

/* 协议框架中的服务器下发配置 */
#pragma pack(1)
typedef struct
{
  ProtocolFrameCtrlUint ProtocolFrameCtrlUint_Temp;
  uint16_t AttributePairsNum;
  uint8_t AnchorWorkModeID;
  uint8_t AnchorWorkMode;
  uint8_t AnchoripaddressID;
  uint8_t Anchoripaddress[4];
  uint8_t AnchorPortID;
  uint16_t AnchorPort;
  uint8_t SubnetmaskID;
  uint8_t Subnetmask[4];
  uint8_t GatewayID;
  uint8_t Gateway[4];
  uint8_t CenteripaddressID;
  uint8_t Centeripaddress[4];
  uint8_t CenterPortID;
  uint16_t CenterPort;
  uint8_t EngineipaddressID;
  uint8_t Engineipaddress[4];
  uint8_t EnginePortID;
  uint16_t EnginePort;
  uint8_t TimeStampID;
  uint32_t TimeStamp;
	ProtocolFrameEnderUint ProtocolFrameEnderUint_Temp;
}ServerSendConfig_Stru;
#pragma pack()

/* 设备上报心跳包 */
#pragma pack(1)
typedef struct
{
  ProtocolFrameCtrlUint ProtocolFrameCtrlUint_Temp;
  uint16_t AttributePairsNum;
  uint8_t PowerOnTypeID;
  uint8_t PowerOnType;
  uint8_t TimeStampID;
  uint32_t TimeStamp;
  uint8_t SelfCheckID;
  uint8_t SelfCheck;
  uint8_t AnchorWorkModeID;
  uint8_t AnchorWorkMode;
	ProtocolFrameEnderUint ProtocolFrameEnderUint_Temp;
}AnchorSendHeart_Stru;
#pragma pack()

/* 服务器控制基站 */
#pragma pack(1)
typedef struct
{
  ProtocolFrameCtrlUint ProtocolFrameCtrlUint_Temp;
  uint16_t AttributePairsNum;
  uint8_t ServerCtrlAnchorID;
  uint8_t ServerCtrlAnchor;
  uint8_t TimeStampID;
  uint32_t TimeStamp;
	ProtocolFrameEnderUint ProtocolFrameEnderUint_Temp;
}ServerCtrlAnchor_Stru;
#pragma pack()

/* 基站时间更新请求 */
#pragma pack(1)
typedef struct
{
  ProtocolFrameCtrlUint ProtocolFrameCtrlUint_Temp;
  uint16_t AttributePairsNum;
  uint8_t UpdateTimeRequestID;
  uint32_t UpdateTimeRequest;
	ProtocolFrameEnderUint ProtocolFrameEnderUint_Temp;
}UpdateTimeRequest_Stru;       
#pragma pack()

/* 网络接收数据结构体 */
#pragma pack(1)
typedef struct
{
  uint8_t NetworkRecvData_Len;
  uint8_t NetworkRecvData_Buffer[100];
}NetworkRecvData_Stru;
#pragma pack()

/* 设备响应服务器获取配置信息请求 */
#pragma pack(1)
typedef struct
{
  ProtocolFrameCtrlUint ProtocolFrameCtrlUint_Temp;
  uint16_t AttributePairsNum;
  uint8_t SoftwareVersionID;
  uint8_t SoftwareVersion[3];
  uint8_t HardwareVersionID;
  uint8_t HardwareVersion[3];
  uint8_t SelfCheckID;
  uint8_t SelfCheck;
  uint8_t AnchorWorkModeID;
  uint8_t AnchorWorkMode;
  uint8_t PowerOnTypeID;
  uint8_t PowerOnType;

  uint8_t AnchoripaddressID;
  uint8_t Anchoripaddress[4];
  uint8_t AnchorPortID;
  uint16_t AnchorPort;
  uint8_t SubnetmaskID;
  uint8_t Subnetmask[4];
  uint8_t GatewayID;
  uint8_t Gateway[4];
  uint8_t CenteripaddressID;
  uint8_t Centeripaddress[4];
  uint8_t CenterPortID;
  uint16_t CenterPort;
  uint8_t EngineipaddressID;
  uint8_t Engineipaddress[4];
  uint8_t EnginePortID;
  uint16_t EnginePort;

  uint8_t TimeStampID;
  uint32_t TimeStamp;
	ProtocolFrameEnderUint ProtocolFrameEnderUint_Temp;
}DevResponConfigInfo_Stru;
#pragma pack()

/* 服务器请求设备信息结构体 */
#pragma pack(1)
typedef struct
{
  ProtocolFrameCtrlUint ProtocolFrameCtrlUint_Temp;
  uint16_t AttributePairsNum;
  uint8_t TimeStampID;
  uint32_t TimeStamp;
	ProtocolFrameEnderUint ProtocolFrameEnderUint_Temp;
}ServerFindAnchorInfo_Stru;
#pragma pack()

void PowerOnMessage_Handle(void);
void DeviceReportHeart(void);
void DeviceUpdateTimeRequest(void);
void UpdateTimeReqReponse_Handle(void);
void ServiceAndDeviceResponse(uint8_t DataFrameState, uint8_t CmdAck, uint32_t TimeStamp);
void DevResponConfigInfoRequst(uint8_t CmdAck, uint32_t TimeStamp);
void ServiceSendConfig_Handle(void);
void DeviceResponseHeart_Handle(void);
void ServerCtrlAnchor_Handle(void);
void ServerFindAnchorInfo_Handle(void);
#endif  //!__SERVERTOANCHOR__H__
