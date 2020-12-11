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

//1. �ṹ�����Ͷ���
#pragma pack(1)
typedef struct
{
	double LastP;//�ϴι���Э���� ��ʼ��ֵΪ0.02
	double Now_P;//��ǰ����Э���� ��ʼ��ֵΪ0
	double out;//�������˲������ ��ʼ��ֵΪ0
	double Kg;//���������� ��ʼ��ֵΪ0
	double Q;//��������Э���� ��ʼ��ֵΪ0.001
	double R;//�۲�����Э���� ��ʼ��ֵΪ0.543
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

