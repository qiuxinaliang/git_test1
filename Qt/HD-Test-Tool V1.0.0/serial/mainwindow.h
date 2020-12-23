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

extern QString SerialRecvData;
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

private slots:
    void on_clearButton_clicked();
    void on_sendButton_clicked();
    void on_openButton_clicked();

    void onReadData();
    void onUpdateChart();
    void onCreateTestData();
    void onTestClicked(bool status);

    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

    void on_tabWidget_currentChanged(int index);

    void distance_to_angle();
    void on_pause_clicked();

    void on_y_sub_clicked();

    void on_y_add_clicked();

    void on_x_add_clicked();

    void on_x_sub_clicked();

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

    int m_xLength;
    double m_serialUpdateData;
    double x_serialUpdaeData;
    double YAxis_AddOrSub;
    double XAxis_AddOrSub;
    //uint8_t m_serialUpdateData;

    QTimer m_testTimer;
    QTimer m_SerialPortReadDataTimer;
    QTimer m_updateChartTimer;

    QTimer PianterTimer;
    QTimer PianterTimerIng;

};

#endif // MAINWINDOW_H
