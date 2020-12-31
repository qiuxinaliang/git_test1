#include <QMainWindow>

#include "serial/mainwindow.h"

#include "rtls_posview.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QPainter>
#include <QPixmap>
#include <QPen>
#include <QMouseEvent>
#include <QFile>
#include <QDataStream>

extern int save_button_triggedFlag;
distance_SendPackets distance_SendPackets_temp;
myWidget::myWidget(QWidget *parent) :
    QWidget(parent)
{

}

myWidget::~myWidget()
{

}



//2. 以高度为例 定义卡尔曼结构体并初始化参数
KFP KFP_height={0.02,0,0,0,0.002,0.043};
KFP KFP_height1={0.02,0,0,0,0.002,0.043};
/**
 *卡尔曼滤波器
 *@param KFP *kfp 卡尔曼结构体参数
 *   float input 需要滤波的参数的测量值（即传感器的采集值）
 *@return 滤波后的参数（最优值）
 */
 static double kalmanFilter(KFP *kfp,double input)
 {
     //预测协方差方程：k时刻系统估算协方差 = k-1时刻的系统协方差 + 过程噪声协方差
     kfp->Now_P = kfp->LastP + kfp->Q;
     //卡尔曼增益方程：卡尔曼增益 = k时刻系统估算协方差 / （k时刻系统估算协方差 + 观测噪声协方差）
     kfp->Kg = kfp->Now_P / (kfp->Now_P + kfp->R);
     //更新最优值方程：k时刻状态变量的最优值 = 状态变量的预测值 + 卡尔曼增益 * （测量值 - 状态变量的预测值）
     kfp->out = kfp->out + kfp->Kg * (input -kfp->out);//因为这一次的预测值就是上一次的输出值
     //更新协方差方程: 本次的系统协方差付给 kfp->LastP 威下一次运算准备。
     kfp->LastP = (1-kfp->Kg) * kfp->Now_P;
     return kfp->out;
 }
 double distance_test;
 double myWidget::DistanceFilterHandle(double height)
 {
     double kalman_height = 0;
     double distance_buf = 0;
     double DistanceFilterInputBuffer_temp = 0;
     static int i = 0;
     distance_buf = height;
     //对输入数据进行滤波处理
     //double *DistanceFilterInputBuffer_temp = new double[DistanceFilterInputBuffer_Len];
     if(i > DistanceFilterInputBuffer_Len)
     {
         //对缓存数据差值进行平均
         for(int j = 0; j < DistanceFilterInputBuffer_Len - 1; j++)
         {
             DistanceFilterInputBuffer_temp += abs(DistanceFilterInputBuffer[j + 1] - DistanceFilterInputBuffer[j]);
         }
         DistanceFilterInputBuffer_temp =  DistanceFilterInputBuffer_temp/(DistanceFilterInputBuffer_Len - 1);

         // 前后两次测量误差太大，测距数据等于上一次测量数据
         if(abs(height - DistanceFilterInputBuffer[DistanceFilterInputBuffer_Len - 1]) > (DistanceFilterInputBuffer_temp + 100))
         {
             DistanceFilterAbnormalCount++;
             height = DistanceFilterInputBuffer[DistanceFilterInputBuffer_Len - 1];
         }
         else
         {
             DistanceFilterAbnormalCount = 0;
         }

         if(DistanceFilterAbnormalCount >= 3)
         {
             DistanceFilterAbnormalCount = 0;
             height =distance_buf;
         }
     }

     i++;
     if(i <= DistanceFilterInputBuffer_Len)
         DistanceFilterInputBuffer[i] = height;
     else if(i > DistanceFilterInputBuffer_Len)
     {
         for(int j = 0; j < DistanceFilterInputBuffer_Len; j++)
         {
             DistanceFilterInputBuffer[j] = DistanceFilterInputBuffer[j + 1];
         }
         DistanceFilterInputBuffer[DistanceFilterInputBuffer_Len - 1] = height;

         i =  DistanceFilterInputBuffer_Len + 1;
     }
     distance_test = height;
     //delete [] DistanceFilterInputBuffer_temp;
     kalman_height = kalmanFilter(&KFP_height,height);
     return kalman_height;
 }

 double myWidget::DistanceFilterHandle1(double height)
 {
     double kalman_height = 0;
     double distance_buf = 0;
     double DistanceFilterInputBuffer_temp = 0;
     static int i = 0;
     distance_buf = height;
     //对输入数据进行滤波处理
     //double *DistanceFilterInputBuffer_temp = new double[DistanceFilterInputBuffer_Len];
     if(i > DistanceFilterInputBuffer_Len)
     {
         //对缓存数据差值进行平均
         for(int j = 0; j < DistanceFilterInputBuffer_Len - 1; j++)
         {
             DistanceFilterInputBuffer_temp += abs(DistanceFilterInputBuffer1[j + 1] - DistanceFilterInputBuffer1[j]);
         }
         DistanceFilterInputBuffer_temp =  DistanceFilterInputBuffer_temp/(DistanceFilterInputBuffer_Len - 1);

         // 前后两次测量误差太大，测距数据等于上一次测量数据
         if(abs(height - DistanceFilterInputBuffer1[DistanceFilterInputBuffer_Len - 1]) > (DistanceFilterInputBuffer_temp + 100))
         {
             DistanceFilterAbnormalCount1++;
             height = DistanceFilterInputBuffer1[DistanceFilterInputBuffer_Len - 1];
         }
         else
         {
             DistanceFilterAbnormalCount1 = 0;
         }

         if(DistanceFilterAbnormalCount1 >= 3)
         {
             DistanceFilterAbnormalCount1 = 0;
             height =distance_buf;
         }
     }

     i++;
     if(i <= DistanceFilterInputBuffer_Len)
         DistanceFilterInputBuffer1[i] = height;
     else if(i > DistanceFilterInputBuffer_Len)
     {
         for(int j = 0; j < DistanceFilterInputBuffer_Len; j++)
         {
             DistanceFilterInputBuffer1[j] = DistanceFilterInputBuffer1[j + 1];
         }
         DistanceFilterInputBuffer1[DistanceFilterInputBuffer_Len - 1] = height;

         i =  DistanceFilterInputBuffer_Len + 1;
     }
     distance_test = height;
     //delete [] DistanceFilterInputBuffer_temp;
     kalman_height = kalmanFilter(&KFP_height1,height);
     return kalman_height;
 }





