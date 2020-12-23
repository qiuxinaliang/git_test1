#include "anchor_para_set.h"
#include "ui_mainwindow.h"

#include <QSettings>  //读写ini文件。
#include <string.h>

static AOA_Target_PosXYZ BLE_AOA_AnglePos_algorithm(double A1_pos_x, double A1_pos_y, double A1_toTargetangle, double A2_pos_x, double A2_pos_y, double A2_toTargetangle);

/**
 * @:*********************************************************************************************************:
 * @函 数 名: on_pushButton_2_clicked()
 * @功能说明: 用于保存基站初始化参数
 * @形    参: textEdit 输入
 * @返 回 值: typedef anchor_para 参数保存于该结构体
 * @**********************************************************************************************************:
 */
void MainWindow::on_pushButton_2_clicked()
{
    anchor_para anchor_para_set[4];     /* 结构体数组定义，用于存放基站初始化参数 */
    QString str;
    memset(anchor_para_set, 0, sizeof(struct  anchor_para)*4);  /* 清除标签参数缓存 */
    ///////////////////////////////////////////////////////////////////////////////
    str = ui->textEdit_A1_ID->toPlainText();   /* 基站1的ID读取 */
    anchor_para_set[0].anchor_id = uint32_t(str.toInt(nullptr, 10));
    str.clear();

    str = ui->textEdit_A1_X->toPlainText();
    anchor_para_set[0].anchor_x = double(str.toDouble(nullptr));
    str.clear();

    str = ui->textEdit_A1_Y->toPlainText();
    anchor_para_set[0].anchor_y = double(str.toDouble(nullptr));
    str.clear();

    str = ui->textEdit_A1_Z->toPlainText();
    anchor_para_set[0].anchor_z = double(str.toDouble(nullptr));
    str.clear();

    str = ui->textEdit_A1_delay->toPlainText();
    anchor_para_set[0].anchor_delay = double(str.toDouble(nullptr));
    str.clear();

    str = ui->textEdit_A1_EN->toPlainText();   /* 读取基站使能参数 */
    anchor_para_set[0].anchor_enable = uint8_t(str.toInt(nullptr, 10));
    str.clear();

    ///////////////////////////////////////////////////////////////////////////////
    str = ui->textEdit_A2_ID->toPlainText();   /* 基站1的ID读取 */
    anchor_para_set[1].anchor_id = uint32_t(str.toInt(nullptr, 10));
    str.clear();

    str = ui->textEdit_A2_X->toPlainText();
    anchor_para_set[1].anchor_x = double(str.toDouble(nullptr));
    str.clear();

    str = ui->textEdit_A2_Y->toPlainText();
    anchor_para_set[1].anchor_y = double(str.toDouble(nullptr));
    str.clear();

    str = ui->textEdit_A2_Z->toPlainText();
    anchor_para_set[1].anchor_z = double(str.toDouble(nullptr));
    str.clear();

    str = ui->textEdit_A2_delay->toPlainText();
    anchor_para_set[1].anchor_delay = double(str.toDouble(nullptr));
    str.clear();

    str = ui->textEdit_A2_EN->toPlainText();   /* 读取基站使能参数 */
    anchor_para_set[1].anchor_enable = uint8_t(str.toInt(nullptr, 10));
    str.clear();

    ///////////////////////////////////////////////////////////////////////////////
    str = ui->textEdit_A3_ID->toPlainText();   /* 基站1的ID读取 */
    anchor_para_set[2].anchor_id = uint32_t(str.toInt(nullptr, 10));
    str.clear();

    str = ui->textEdit_A3_X->toPlainText();
    anchor_para_set[2].anchor_x = double(str.toDouble(nullptr));
    str.clear();

    str = ui->textEdit_A3_Y->toPlainText();
    anchor_para_set[2].anchor_y = double(str.toDouble(nullptr));
    str.clear();

    str = ui->textEdit_A3_Z->toPlainText();
    anchor_para_set[2].anchor_z = double(str.toDouble(nullptr));
    str.clear();

    str = ui->textEdit_A3_delay->toPlainText();
    anchor_para_set[2].anchor_delay = double(str.toDouble(nullptr));
    str.clear();

    str = ui->textEdit_A3_EN->toPlainText();   /* 读取基站使能参数 */
    anchor_para_set[2].anchor_enable = uint8_t(str.toInt(nullptr, 10));
    str.clear();

    ///////////////////////////////////////////////////////////////////////////////
    str = ui->textEdit_A4_ID->toPlainText();   /* 基站1的ID读取 */
    anchor_para_set[3].anchor_id = uint32_t(str.toInt(nullptr, 10));
    str.clear();

    str = ui->textEdit_A4_X->toPlainText();
    anchor_para_set[3].anchor_x = double(str.toDouble(nullptr));
    str.clear();

    str = ui->textEdit_A4_Y->toPlainText();
    anchor_para_set[3].anchor_y = double(str.toDouble(nullptr));
    str.clear();

    str = ui->textEdit_A4_Z->toPlainText();
    anchor_para_set[3].anchor_z = double(str.toDouble(nullptr));
    str.clear();

    str = ui->textEdit_A4_delay->toPlainText();
    anchor_para_set[3].anchor_delay = double(str.toDouble(nullptr));
    str.clear();

    str = ui->textEdit_A4_EN->toPlainText();   /* 读取基站使能参数 */
    anchor_para_set[3].anchor_enable = uint8_t(str.toInt(nullptr, 10));
    str.clear();
    qDebug() << str << "saved";

    ///////////////////////////////////////////////////////////////////
    int i;
    for(i = 0; i < 4; i++)
    {
        qDebug("anchor_para_set[%d].ID = %d", i, anchor_para_set[i].anchor_id);
        qDebug("anchor_para_set[%d].x = %lf", i, anchor_para_set[i].anchor_x);
        qDebug("anchor_para_set[%d].y = %lf", i, anchor_para_set[i].anchor_y);
        qDebug("anchor_para_set[%d].z = %lf", i, anchor_para_set[i].anchor_z);
        qDebug("anchor_para_set[%d].delay = %lf", i, anchor_para_set[i].anchor_delay);
        qDebug("anchor_para_set[%d].enable = %d", i, anchor_para_set[i].anchor_enable);
        qDebug("\n");
    }
    /* 将配置参数存入配置文件 */
    QSettings *configIni = new QSettings("anchor_para.ini", QSettings::IniFormat);
    configIni->clear();

    configIni-> setValue("/anchor1_id", anchor_para_set[0].anchor_id);
    configIni-> setValue("/anchor1_x", anchor_para_set[0].anchor_x);
    configIni-> setValue("/anchor1_y", anchor_para_set[0].anchor_y);
    configIni-> setValue("/anchor1_z", anchor_para_set[0].anchor_z);
    configIni-> setValue("/anchor1_delay", anchor_para_set[0].anchor_delay);
    configIni-> setValue("/anchor1_enable", anchor_para_set[0].anchor_enable);

    configIni-> setValue("/anchor2_id", anchor_para_set[1].anchor_id);
    configIni-> setValue("/anchor2_x", anchor_para_set[1].anchor_x);
    configIni-> setValue("/anchor2_y", anchor_para_set[1].anchor_y);
    configIni-> setValue("/anchor2_z", anchor_para_set[1].anchor_z);
    configIni-> setValue("/anchor2_delay", anchor_para_set[1].anchor_delay);
    configIni-> setValue("/anchor2_enable", anchor_para_set[1].anchor_enable);

    configIni-> setValue("/anchor3_id", anchor_para_set[2].anchor_id);
    configIni-> setValue("/anchor3_x", anchor_para_set[2].anchor_x);
    configIni-> setValue("/anchor3_y", anchor_para_set[2].anchor_y);
    configIni-> setValue("/anchor3_z", anchor_para_set[2].anchor_z);
    configIni-> setValue("/anchor3_delay", anchor_para_set[2].anchor_delay);
    configIni-> setValue("/anchor3_enable", anchor_para_set[2].anchor_enable);

    configIni-> setValue("/anchor4_id", anchor_para_set[3].anchor_id);
    configIni-> setValue("/anchor4_x", anchor_para_set[3].anchor_x);
    configIni-> setValue("/anchor4_y", anchor_para_set[3].anchor_y);
    configIni-> setValue("/anchor4_z", anchor_para_set[3].anchor_z);
    configIni-> setValue("/anchor4_delay", anchor_para_set[3].anchor_delay);
    configIni-> setValue("/anchor4_enable", anchor_para_set[3].anchor_enable);

    delete  configIni;
}

/**
 * @:*********************************************************************************************************:
 * @函 数 名: on_pushButton_clicked
 * @功能说明: 用于清空textEdit数据输入，并对 anchor_para.ini进行读写测试。
 * @形    参: 无
 * @返 回 值: 无
 * @**********************************************************************************************************:
 */
void MainWindow::on_pushButton_clicked()
{
    ui->textEdit_A1_NUM->clear();
    ui->textEdit_A1_ID->clear();
    ui->textEdit_A1_X->clear();
    ui->textEdit_A1_Y->clear();
    ui->textEdit_A1_Z->clear();
    ui->textEdit_A1_delay->clear();
    ui->textEdit_A1_EN->clear();

    ui->textEdit_A2_NUM->clear();
    ui->textEdit_A2_ID->clear();
    ui->textEdit_A2_X->clear();
    ui->textEdit_A2_Y->clear();
    ui->textEdit_A2_Z->clear();
    ui->textEdit_A2_delay->clear();
    ui->textEdit_A2_EN->clear();

    ui->textEdit_A3_NUM->clear();
    ui->textEdit_A3_ID->clear();
    ui->textEdit_A3_X->clear();
    ui->textEdit_A3_Y->clear();
    ui->textEdit_A3_Z->clear();
    ui->textEdit_A3_delay->clear();
    ui->textEdit_A3_EN->clear();

    ui->textEdit_A4_NUM->clear();
    ui->textEdit_A4_ID->clear();
    ui->textEdit_A4_X->clear();
    ui->textEdit_A4_Y->clear();
    ui->textEdit_A4_Z->clear();
    ui->textEdit_A4_delay->clear();
    ui->textEdit_A4_EN->clear();

    QSettings *configIni = new QSettings("anchor_para.ini", QSettings::IniFormat);
    //QString config_para_test = configIni->value("/anchor4_x").toString();
   // qDebug() << config_para_test;
    AOA_Target_PosXYZ AOA_Target_PosXYZ_temp;
    AOA_Target_PosXYZ_temp = BLE_AOA_AnglePos_algorithm(20, 0, -60, 0, 0, 30);
    qDebug("AOA_Target_pos_x = %f\n", AOA_Target_PosXYZ_temp.AOA_Target_pos_x);
    qDebug("AOA_Target_pos_y = %f\n", AOA_Target_PosXYZ_temp.AOA_Target_pos_y);
    delete  configIni;
}

/**
 * @:*********************************************************************************************************:
 * @函 数 名: BLE_AOA_AnglePos_algorithm
 * @功能说明: 用户三角定位解算
 * @形    参: A1:x,y坐标值，待解算设备对应角度值，A2:x,y坐标值，待解算设备对应角度值，
 * @返 回 值: AOA_Target_PosXYZ:待测目标的坐标值
 * @**********************************************************************************************************:
 */
static AOA_Target_PosXYZ BLE_AOA_AnglePos_algorithm(double A1_pos_x, double A1_pos_y, double A1_toTargetangle, double A2_pos_x, double A2_pos_y, double A2_toTargetangle)
{
    AOA_Target_PosXYZ AOA_Target_PosXYZ_buf;
    double A1_pos_x_buf = 0, A1_pos_y_buf = 0, A1_toTargetangle_buf = 0;
    double A2_pos_x_buf = 0, A2_pos_y_buf = 0, A2_toTargetangle_buf = 0;

    /* 对输入变量进行缓存 */
    A1_pos_x_buf = A1_pos_x;
    A1_pos_y_buf = A1_pos_y;
    A1_toTargetangle_buf = A1_toTargetangle;

    A2_pos_x_buf = A2_pos_x;
    A2_pos_y_buf = A2_pos_y;
    A2_toTargetangle_buf = A2_toTargetangle;

    /* 算法缓存 */
    double AOA_tan_buf_a = 0;
    double AOA_tan_buf_b = 0;

    AOA_tan_buf_a = tan((A1_toTargetangle_buf) * (angle_pi/180));
    AOA_tan_buf_b = tan((A2_toTargetangle_buf) * (angle_pi/180));
    qDebug("AOA_Target_angle_A1 = %f\n",  AOA_tan_buf_a);
    qDebug("AOA_Target_angle_A2 = %f\n",  AOA_tan_buf_b);
    memset((double *)&AOA_Target_PosXYZ_buf, 0.0, sizeof(AOA_Target_PosXYZ_buf));

    AOA_Target_PosXYZ_buf.AOA_Target_pos_x = (A2_pos_y_buf - A1_pos_y_buf - (A2_pos_x_buf * AOA_tan_buf_b) +
                                             (A1_pos_x_buf * AOA_tan_buf_a))/(AOA_tan_buf_a -AOA_tan_buf_b);
    AOA_Target_PosXYZ_buf.AOA_Target_pos_y = ((A2_pos_x_buf * AOA_tan_buf_a) - (A1_pos_y_buf * AOA_tan_buf_b) +
                                              (A1_pos_x_buf * AOA_tan_buf_a * AOA_tan_buf_b) - (A2_pos_x_buf * AOA_tan_buf_a
                                              * AOA_tan_buf_b))/(AOA_tan_buf_a -AOA_tan_buf_b);
    AOA_Target_PosXYZ_buf.AOA_Target_pos_z = 0;


    return AOA_Target_PosXYZ_buf;


    //qDebug() << "test_aoa";
}




