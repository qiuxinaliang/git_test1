/**********************************************************
Author: Qt君
微信公众号: Qt君(首发)
QQ群: 732271126
LICENSE: MIT
**********************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>
#include <QPaintEvent>
#include <QUdpSocket>

extern QString SerialRecvData;
extern uint8_t udpRecv_hexData[20];
extern int save_button_triggedFlag;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void serialUpdateData(double value);
    QFile *hexfile;

private slots:
    void on_clearButton_clicked();
    void on_sendButton_clicked();
    void on_openButton_clicked();

    void onReadData();
    void onUpdateChart();
    void onCreateTestData();
    void onTestClicked(bool status);
    void SerialRecvDataHandle();

    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

    void on_tabWidget_currentChanged(int index);

    void on_pause_clicked();

    void on_y_sub_clicked();

    void on_y_add_clicked();

    void on_x_add_clicked();

    void on_x_sub_clicked();

    void on_save_button_clicked();

protected:
    int i, j;

private:
    Ui::MainWindow *ui;
    QSerialPort rtls_serialPort;
    /* 定义两个可变数组存放绘图的坐标数据 */
    QVector<double> m_xs;
    QVector<double> m_ys;
    QVector<double> m1_xs;
    QVector<double> m1_ys;
    QVector<double> m2_xs;
    QVector<double> m2_ys;

    int m_xLength;
    double m_serialUpdateData;
    double x_serialUpdaeData;
    double YAxis_AddOrSub;
    double XAxis_AddOrSub;

    QTimer m_testTimer;
    QTimer m_SerialPortReadDataTimer;
    QTimer m_updateChartTimer;

    QTimer PianterTimer;
    QTimer PianterTimerIng;

    int port;
    bool isStarted;
    QUdpSocket *udpSocket;
    QTimer *timer;
    class Public *myPublic;
    class udpsever *myudpserver;

};

#endif // MAINWINDOW_H
