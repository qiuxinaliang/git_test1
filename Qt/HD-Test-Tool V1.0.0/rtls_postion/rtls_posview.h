#ifndef RTLS_POSVIEW_H
#define RTLS_POSVIEW_H

#include <QWidget>
#include <QTimer>

#define DistanceFilterInputBuffer_Len 20
//double DetectionThreshold = 5000;  //5000mm
#pragma pack(1)
typedef struct distance_SendPackets
{
  uint16_t header;
  uint8_t id_anchor;
  uint8_t A_id_tag;
  uint32_t A_distance;
  uint8_t B_id_tag;
  uint32_t B_distance;
  uint32_t reserved;
  uint16_t ender;
}distance_SendPackets;
#pragma pack()

//1. 结构体类型定义
typedef struct
{
    double LastP;//上次估算协方差 初始化值为0.02
    double Now_P;//当前估算协方差 初始化值为0
    double out;//卡尔曼滤波器输出 初始化值为0
    double Kg;//卡尔曼增益 初始化值为0
    double Q;//过程噪声协方差 初始化值为0.001
    double R;//观测噪声协方差 初始化值为0.543
}KFP;       //Kalman Filter parameter

extern distance_SendPackets distance_SendPackets_temp;
extern double distance_test;
class myWidget : public QWidget
{
    Q_OBJECT

public:
    explicit myWidget(QWidget *parent = nullptr);
    ~myWidget();
protected:
    double DistanceFilterInputBuffer[DistanceFilterInputBuffer_Len];
    double DistanceFilterInputBuffer1[DistanceFilterInputBuffer_Len];
    int DistanceFilterAbnormalCount = 0;
    int DistanceFilterAbnormalCount1 = 0;

private slots:
    void SerialRecvDataHandle();

private:
    double DistanceFilterHandle(double height);
    double DistanceFilterHandle1(double height);

    //QString SerialRecvData;

};



#endif // RTLS_POSVIEW_H
