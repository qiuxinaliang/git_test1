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
#define  M_PI		3.14159265358979323846

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

void UWB_DistanceToAngle(void);
#endif  //!__DISTANCEHANDLE__H__

