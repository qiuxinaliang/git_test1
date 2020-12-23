#ifndef ANCHOR_PARA_SET_H
#define ANCHOR_PARA_SET_H

#include <QMainWindow>
#include <QDebug>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>

#include "serial/mainwindow.h"
#include <math.h>

#define angle_pi 3.1415926


typedef struct  anchor_para
{
    uint32_t anchor_id;
    double anchor_x;
    double anchor_y;
    double anchor_z;
    double anchor_delay;
    uint8_t anchor_enable;

}anchor_para;

typedef struct AOA_Target_PosXYZ
{
    double AOA_Target_pos_x;
    double AOA_Target_pos_y;
    double AOA_Target_pos_z;
}AOA_Target_PosXYZ;



#endif // ANCHOR_PARA_SET_H
