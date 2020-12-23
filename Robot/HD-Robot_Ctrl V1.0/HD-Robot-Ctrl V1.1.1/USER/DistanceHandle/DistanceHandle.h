/*
 * @:*********************************************************************************************************: 
 * @Date: 2020-10-10 16:26:10
 * @LastEditTime: 2020-10-10 16:27:21
 * @**********************************************************************************************************: 
 */
#ifndef __DISTANCEHANDLE__H__
#define __DISTANCEHANDLE__H__

#include "common.h"

#define  DistanceData_Queue_Size 200
#define M_PI		3.14159265358979323846
#define AnchorDataPack 0xFE

#define heade 0x4448
#define end 0x4225

//1. 结构体类型定义
#pragma pack(1)
typedef struct
{
    double LastP;//上次估算协方差 初始化值为0.02
    double Now_P;//当前估算协方差 初始化值为0
    double out;//卡尔曼滤波器输出 初始化值为0
    double Kg;//卡尔曼增益 初始化值为0
    double Q;//过程噪声协方差 初始化值为0.001
    double R;//观测噪声协方差 初始化值为0.543
}KFP;       //Kalman Filter parameter
#pragma pack()

//extern DistanceOffset_Stru DistanceOffset_Stru_temp;
extern double AnchorToAnchor_Distance[3];
extern uint32_t AB_Distance;
extern uint32_t Distance_AntDelay;
extern uint8_t tim6_irq_flag;
extern Queue DistanceData_queue;
#define DistanceData_QUEUE_INIT(queue_size)\
{\
    DistanceData_queue.item = NULL;\
    init_queue(&DistanceData_queue,queue_size);\
}

// 发送待解算数据包
#pragma pack(1)
typedef struct Distance_ToAngle_Stru
{
  uint16_t header;
	uint8_t Follwmode;
  uint8_t id_anchor; 
  uint8_t A_id_tag;  
  uint32_t A_distance;  
	uint8_t B_id_tag;  
  uint32_t B_distance;
  uint32_t AB_distance;
  uint32_t reserved; 
	uint8_t BatteryVoltage;
  uint16_t ender; 
}Distance_ToAngle_Stru;
#pragma pack()

// 校验模式下数据结构体
#pragma pack(1)
typedef struct
{
	uint8_t FollowMode;
	double standard_angle;
	double A_standard_distance;
	double B_standard_distance;
}CalibrationMode_Stru;
#pragma pack()

// 给基站发送跟随命令
#pragma pack(1)
typedef struct AnchorFollowCmd_Stru
{
  uint16_t header;
	uint8_t Follwmode;
  uint16_t ender; 
}AnchorFollowCmd_Stru;
#pragma pack()

extern Distance_ToAngle_Stru Distance_ToAngle_Stru_temp;
void UWB_DistanceToAngle(Distance_ToAngle_Stru *Distance_ToAngle_Stru_temp);
void Distance_DefaultPara_Config(void);
#endif  //!__DISTANCEHANDLE__H__

