#include <QMainWindow>

#include "serial/mainwindow.h"

#include "rtls_posview.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QPainter>
#include <QPixmap>
#include <QPen>
#include <QMouseEvent>
#include "serial/mainwindow.h"

distance_SendPackets distance_SendPackets_temp;
myWidget::myWidget(QWidget *parent) :
    QWidget(parent)
{

}

myWidget::~myWidget()
{

}

void myWidget::SerialRecvDataHandle()
{
    bool RecvPacketHeader;
    bool RecvPacketEnder;
    uint32_t AOrBdistance_temp = 0;
    static uint32_t AOrBdistance_last[2] = {0};
    uint8_t distance_temp[8] = { 0 };
    QString SerialHex_temp ;
    static uint32_t Simu_distance[2] = {0};
    Simu_distance[0] += 50;
    Simu_distance[1] += 50;

    RecvPacketHeader =  SerialRecvData.startsWith("4844");   /* 对接收字符串进行校验 */
    RecvPacketEnder =  SerialRecvData.endsWith("2542");
    if((RecvPacketHeader == true) || (RecvPacketEnder == true))    /* 如果校验为真 */
    {
        SerialHex_temp.clear();
        SerialHex_temp =  SerialRecvData.mid(4, 2);    /* 读取字符串中有效值 */
        distance_SendPackets_temp.id_anchor = uint8_t(SerialHex_temp.toInt(nullptr, 16));
        SerialHex_temp.clear();
        SerialHex_temp =  SerialRecvData.mid(6, 2);    /* 读取字符串中有效值 */
        distance_SendPackets_temp.A_id_tag = uint8_t(SerialHex_temp.toInt(nullptr, 16));
        SerialHex_temp.clear();
        for(int i = 0; i < 4;i++)
        {
            SerialHex_temp.clear();
            SerialHex_temp =  SerialRecvData.mid((8 + i*2), 2);    /* 读取字符串中有效值 */
            distance_temp[i] = uint8_t(SerialHex_temp.toInt(nullptr, 16));
            SerialHex_temp.clear();
        }
        AOrBdistance_temp = uint32_t(distance_temp[0] | (distance_temp[1]<<8) | (distance_temp[2]<<16) | (distance_temp[3]<<24));
        distance_SendPackets_temp.A_distance = uint32_t(DistanceFilterHandle( AOrBdistance_temp));
        if(AOrBdistance_temp != 0)
            AOrBdistance_last[0] = AOrBdistance_temp;
        AOrBdistance_temp = 0;
        qDebug() << "distance_tempA" << distance_SendPackets_temp.A_distance;

        SerialHex_temp.clear();
        SerialHex_temp =  SerialRecvData.mid(16, 2);    /* 读取字符串中有效值 */
        distance_SendPackets_temp.B_id_tag = uint8_t(SerialHex_temp.toInt(nullptr, 16));
        SerialHex_temp.clear();
        for(int i = 0; i < 4;i++)
        {
            SerialHex_temp.clear();
            SerialHex_temp =  SerialRecvData.mid((18 + i*2), 2);    /* 读取字符串中有效值 */
            distance_temp[i] = uint8_t(SerialHex_temp.toInt(nullptr, 16));
            SerialHex_temp.clear();
        }
        AOrBdistance_temp = uint32_t(distance_temp[0] | (distance_temp[1]<<8) | (distance_temp[2]<<16) | (distance_temp[3]<<24));
        distance_SendPackets_temp.B_distance = uint32_t(DistanceFilterHandle1( AOrBdistance_temp));
        if(AOrBdistance_temp != 0)
            AOrBdistance_last[1] = AOrBdistance_temp;
        AOrBdistance_temp = 0;
        qDebug() << "distance_tempB" << distance_SendPackets_temp.B_distance;
        if( distance_SendPackets_temp.A_distance ==  distance_SendPackets_temp.B_distance)
        {
             distance_SendPackets_temp.A_distance = AOrBdistance_last[0];
             distance_SendPackets_temp.B_distance = AOrBdistance_last[1];
        }

        for(int i = 0; i < 4;i++)
        {
            SerialHex_temp.clear();
            SerialHex_temp =  SerialRecvData.mid((26 + i*2), 2);    /* 读取字符串中有效值 */
            distance_temp[i] = uint8_t(SerialHex_temp.toInt(nullptr, 16));
            SerialHex_temp.clear();
        }
        AOrBdistance_temp = uint32_t(distance_temp[0] | (distance_temp[1]<<8) | (distance_temp[2]<<16) | (distance_temp[3]<<24));
        distance_SendPackets_temp.reserved = AOrBdistance_temp;
        AOrBdistance_temp = 0;
        //qDebug() << "id_anchor" << distance_SendPackets_temp.id_anchor;


        //Hex_out = uint32_t (m_serialhexdata_1.toInt(nullptr, 10));
        //qDebug() << Hex_out;
        //rtls_serialUpdateData = Hex_out;   /* 对待更新参数进行赋值 */
        //m_serialUpdateData = ((double)Hex_out)/1000;
    }

    if(! SerialRecvData.isEmpty())
    {
         SerialRecvData.clear();
    }

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





