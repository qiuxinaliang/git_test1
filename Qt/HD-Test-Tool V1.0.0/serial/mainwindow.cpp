﻿/**********************************************************
Author: Qt君
微信公众号: Qt君(首发)
QQ群: 732271126
LICENSE: MIT
**********************************************************/
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <math.h>
#include <rtls_postion/rtls_posview.h>
#include "public/public.h"
#include "network/udpsever.h"

QString SerialRecvData;
MainWindow::MainWindow(QWidget *parent) :QMainWindow(parent), ui(new Ui::MainWindow), m_xLength(0)
{
    ui->setupUi(this);
    myPublic = nullptr;
    myPublic = new Public();
    myudpserver = new udpsever();
    /* 向绘图区域QCustomPlot添加一条曲线 */
    ui->qCustomPlot->addGraph();
    ui->qCustomPlot->addGraph();
    ui->qCustomPlot->addGraph();
    QPen pen;
    pen.setWidth(1);//线宽

    //pen.setStyle(Qt::PenStyle::DashLine);//虚线
    pen.setColor(Qt::red);//黄色
    ui->qCustomPlot->graph(0)->setPen(pen);
    pen.setColor(Qt::green);//黄色
    ui->qCustomPlot->graph(2)->setPen(pen);

    m_SerialPortReadDataTimer.start(1);
    m_testTimer.start(10);

    connect(&m_updateChartTimer, SIGNAL(timeout()), this, SLOT(onUpdateChart()));

    /* 连接数据信号槽 */
    connect(&m_SerialPortReadDataTimer, SIGNAL(timeout()), this, SLOT(onReadData()));
    //myWidget *myWidget_serialHandel = new myWidget();
    connect(&m_testTimer, SIGNAL(timeout()), this, SLOT(SerialRecvDataHandle()));

    /* 设置坐标轴标签名称 */
    ui->qCustomPlot->xAxis->setLabel("时间");
    ui->qCustomPlot->yAxis->setLabel("距离（mm)");

    /* 设置坐标轴显示范围 */
    XAxis_AddOrSub = 1000;
    ui->qCustomPlot->xAxis->setRange(0,  XAxis_AddOrSub);
    YAxis_AddOrSub = 1000;
    ui->qCustomPlot->yAxis->setRange(0, YAxis_AddOrSub);

    /* 查找可用的串口 */
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        ui->PortBox->addItem(info.portName());
    }

    /* 设置波特率下拉菜单默认显示第三项(9600) */
    ui->BaudBox->setCurrentIndex(5);


    /* 关闭发送按钮的使能 */
    ui->sendButton->setEnabled(false);

    //PianterTimer.start(1000);
    //PianterTimerIng.start(10);
    //connect(&PianterTimer, SIGNAL(timeout()), this, SLOT(T_PiantEvent()));
    //connect(&PianterTimerIng, SIGNAL(timeout()), this, SLOT(paintEvent()));
    qDebug()<<"界面初始化成功！";
}

MainWindow::~MainWindow()
{
    delete ui;
}

/* 清空接收窗口 */
void MainWindow::on_clearButton_clicked()
{
    ui->textEdit->clear();
}

/* 发送数据 */
void MainWindow::on_sendButton_clicked()
{
    rtls_serialPort.write(ui->textEdit_2->toPlainText().toLatin1());
}

/**
 * @:*********************************************************************************************************:
 * @功能说明: 读取接收到的数据
 * @形    参: 无
 * @返 回 值: 无
 * @**********************************************************************************************************:
 */
void MainWindow::onReadData()
{
    QByteArray buf;
    static int len = 0;
    QString m_serialhexdata ;

    buf = rtls_serialPort.readAll();       /* 将字符换成 */
    m_serialhexdata = buf.toHex();      /* 以16进制字符缓存 */
    len = m_serialhexdata.length();     /* 获取字符串的长度 */
    if(len >= 20) /* 判断缓存字符串的长度 */
    {
        SerialRecvData.insert(0, m_serialhexdata);   /* 换成当前字符串 */
        len = 0;
    }
    if(!buf.isEmpty())
    {
#if 0
        QString str = ui->textEdit->toPlainText();
        str += '\n';
        str += tr(buf.toHex());     /* 16进制数转为字符 */
        str.toStdString();
        ui->textEdit->clear();
        ui->textEdit->append(str); /* 显示字符串 */
        str.clear();
#endif
    }
    buf.clear();
}

/**
 * @:*********************************************************************************************************:
 * @功能说明: 更新显示
 * @形    参: 无
 * @返 回 值: 无
 * @**********************************************************************************************************:
 */
void MainWindow::onUpdateChart()
{
    if(m_xLength <= XAxis_AddOrSub)    /* 设置x 轴显示坐标范围 */
    {
         ui->qCustomPlot->xAxis->setRange(0, XAxis_AddOrSub);
    }
    else
    {
         ui->qCustomPlot->xAxis->setRange(m_xLength - XAxis_AddOrSub, m_xLength);
    }

    ui->qCustomPlot->clearFocus();      /* 刷新显示 */
    m_xs.append(m_xLength);     /* 更新X坐标 */
    m_ys.append(distance_SendPackets_temp.B_distance);   /* 更新Y坐标 */
    m1_xs.append(m_xLength);     /* 更新X坐标 */
    m1_ys.append(distance_SendPackets_temp.A_distance);   /* 更新Y坐标 */

    m2_xs.append(m_xLength);     /* 更新X坐标 */
    m2_ys.append(distance_SendPackets_temp.reserved);   /* 更新Y坐标 */

    ui->qCustomPlot->replot(QCustomPlot::rpQueuedReplot);      /* 刷新显示 */
    ui->qCustomPlot->graph(0)->setData(m_xs, m_ys);
    ui->qCustomPlot->graph(1)->setData(m1_xs, m1_ys);
    ui->qCustomPlot->graph(2)->setData(m2_xs, m2_ys);
    m_xLength++;
    //QString str = "123";
   // ui->qCustomPlot->xAxis->setLabel("角度:" + str);
}

/* 随机数的创建 */
void MainWindow::onCreateTestData()
{
    static double i = 0;
    i += 0.002;
    if(i >= 2) i = 0;

    m_serialUpdateData = 500*sin(3.14*i) + 500;
   // x_serialUpdaeData = rand()%100 + 5;
}

/**
 * @:*********************************************************************************************************:
 * @功能说明: 测试程序，目前功能未知
 * @形    参: status
 * @返 回 值: 无
 * @**********************************************************************************************************:
 */
void MainWindow::onTestClicked(bool status)
{
#if 0
    if (m_testTimer.isActive())
    {
        m_testTimer.stop();
        ui->m_testBtn->setText("开启");
    }
    else
    {
        m_testTimer.start();
        ui->m_testBtn->setText("关闭");
        m_serialUpdateData = 0;
    }
#endif
}

/**
 * @:*********************************************************************************************************:
 * @功能说明: 打开串口，串口参数初始化
 * @形    参: 无
 * @返 回 值: 无
 * @**********************************************************************************************************:
 */
void MainWindow::on_openButton_clicked()
{
    if(ui->openButton->text()==tr("打开串口"))
    {
        if (rtls_serialPort.isOpen())
        {
            rtls_serialPort.close();
            rtls_serialPort.clear();
        }
        rtls_serialPort.setPortName(ui->PortBox->currentText());    /* 设置串口名 */
        rtls_serialPort.open(QIODevice::ReadWrite);    /* 打开串口 */
        rtls_serialPort.setBaudRate(ui->BaudBox->currentText().toInt());   /* 设置波特率 */
        if (ui->BitNumBox->currentIndex() == 8)     /* 设置数据位数 */
        {
            rtls_serialPort.setDataBits(QSerialPort::Data8);
        }
        if (ui->ParityBox->currentIndex() == 0)     /* 设置奇偶校验 */
        {

            rtls_serialPort.setParity(QSerialPort::NoParity);
        }
        switch(ui->StopBox->currentIndex())     /* 设置停止位 */
        {
            case 1: rtls_serialPort.setStopBits(QSerialPort::OneStop); break;
            case 2: rtls_serialPort.setStopBits(QSerialPort::TwoStop); break;
            default: break;
        }
        rtls_serialPort.setFlowControl(QSerialPort::NoFlowControl);    /* 设置流控制 */
        /* 关闭设置菜单使能 */
        ui->PortBox->setEnabled(false);
        ui->BaudBox->setEnabled(false);
        ui->BitNumBox->setEnabled(false);
        ui->ParityBox->setEnabled(false);
        ui->StopBox->setEnabled(false);
        ui->openButton->setText(tr("关闭串口"));
        ui->sendButton->setEnabled(true);
    }
    else
    {
        rtls_serialPort.close();   /* 关闭串口 */
        rtls_serialPort.clear();

        /* 恢复设置使能 */
        ui->PortBox->setEnabled(true);
        ui->BaudBox->setEnabled(true);
        ui->BitNumBox->setEnabled(true);
        ui->ParityBox->setEnabled(true);
        ui->StopBox->setEnabled(true);
        ui->openButton->setText(tr("打开串口"));
        ui->sendButton->setEnabled(false);
    }
}



void MainWindow::on_tabWidget_currentChanged(int index)
{

}

//波形启动与暂停操作
void MainWindow::on_pause_clicked()
{
    if(ui->pause->text() == tr("开始"))
    {
        m_updateChartTimer.start(1);
        ui->pause->setText(tr("暂停"));
    }
    else if(ui->pause->text() == tr("暂停"))
    {
        m_updateChartTimer.stop();
        ui->pause->setText(tr("开始"));
    }
}

void MainWindow::on_y_sub_clicked()
{
     YAxis_AddOrSub -= 200;
    ui->qCustomPlot->yAxis->setRange(0, YAxis_AddOrSub);
}

void MainWindow::on_y_add_clicked()
{
    YAxis_AddOrSub += 200;
    ui->qCustomPlot->yAxis->setRange(0, YAxis_AddOrSub);
}

void MainWindow::on_x_add_clicked()
{
    XAxis_AddOrSub += 100;
}

void MainWindow::on_x_sub_clicked()
{
    XAxis_AddOrSub -= 100;
}

int save_button_triggedFlag  = 0;
void MainWindow::on_save_button_clicked()
{
    hexfile = new QFile();
    hexfile->setFileName("save_original.dat");
    hexfile->resize(100000);
    if(ui->save_button->text() == tr("保存"))
    {
        hexfile->open(QIODevice::WriteOnly | QIODevice::Truncate);
        QDataStream out(hexfile);
        save_button_triggedFlag = 1;
        ui->save_button->setText(tr("结束保存"));

    }
    else if(ui->save_button->text() == tr("结束保存"))
    {
        save_button_triggedFlag = 0;
        hexfile->close();
        ui->save_button->setText(tr("保存"));
    }
}

void MainWindow::SerialRecvDataHandle()
{
    uint8_t temp_data = 0;
    bool RecvPacketHeader;
    bool RecvPacketEnder;
    uint32_t AOrBdistance_temp = 0;
    static uint32_t AOrBdistance_last[2] = {0};
    uint8_t distance_temp[8] = { 0 };
    QString SerialHex_temp ;
    QDataStream out(hexfile);

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
        distance_SendPackets_temp.A_distance = AOrBdistance_temp;

        if(AOrBdistance_temp != 0)
        {
            AOrBdistance_last[0] = AOrBdistance_temp;
            if(save_button_triggedFlag == 1)
            {
               out << 0x12345678;
               qDebug() << "distance_tempA" << distance_SendPackets_temp.A_distance;
            }
        }
        AOrBdistance_temp = 0;

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
        distance_SendPackets_temp.B_distance = AOrBdistance_temp;
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
