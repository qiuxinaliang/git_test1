/*
 * @:*********************************************************************************************************: 
 * @Date: 2020-11-16 10:31:55
 * @LastEditTime: 2020-11-17 09:39:30
 * @**********************************************************************************************************: 
 */
#ifndef __LORA_DEMO_H__
#define __LORA_DEMO_H__

#include "common.h"

#define EPAULET_HEART           1   //肩章定时上传
#define WATCH_HEART             2   //肩章+手环定时上传
#define WATCH_READ              3   //手环短消息已读
#define EPAULET_ACK             0xFF//肩章应答

/**
 * @brief 开机 定时上传 报警
 * 
 */
typedef struct 
{
    u8 cmd;         //1开机  定时上传     
    u8 index;       //增量序号
    u8 id0;
    u8 id1;
    u8 soft_versen; //软件版本
    u8 vol;         //电量
    u8 alarm;       //手环低电量:0x01,低电量:0x02
}epaulet;

/**
 * @brief 应答
 * 
 */
typedef struct 
{
    u8 cmd;         //3:短消息已读  FF：应答
    u8 index;       //接收到数据的index
    u8 id0;
    u8 id1;
}epaulet_ack;

/**
 * @brief 落水
 * 
 */
typedef struct      //4:落水
{ 
    epaulet epqu;
    float lat;   //纬度
    u8 lat_NorS; //NorS
    float lon;   //经度
    u8 log_EorW; //EorW
    uint16_t section;
    u8 direction;
} epaulet_gps;

/**
 * @brief 带手环上传
 * 
 */
typedef struct{     //2：带手环定时上传    
    epaulet epau;
    u8 watch_vol;
    u8 watch_heart;
    uint16_t watch_step;
}epaulet_watch;



//server
typedef struct{
    u8 cmd;         //1：越界报警   2：关机     3：重启 4：解除报警 5：蓝牙绑定 6：蓝牙解除 7:间隔时间 255:应答
    u8 index;
    u8 id0;
    u8 id1;
    u8 xor;
}server_epaulet;

typedef struct{
    u8 cmd;         //1：越界报警   2：关机     3：重启 4：解除报警 5：蓝牙绑定 6：蓝牙解除 7:间隔时间 255:应答
    u8 index;
    u8 id0;
    u8 id1;
    u32 time;       //间隔时间 单位秒
    u8 xor;
}server_time;

typedef struct{
    u8 cmd;         //8：短消息
    u8 index;
    u8 id0;
    u8 id1;
    u8 data[40];    //短消息 最大20字   
    u8 xor;
}server_msg;

//void Lora_RecvData_Handle(void);
void Demo_Lora_RecvData_Handle(void);
uint8_t Lora_SendData_Handle(uint8_t  *data, int len, uint32_t timeoutMs);
uint8_t Lora_RecvData_Handle(uint8_t *buf, uint8_t *len, uint32_t timeoutMs);
void JianZhang_LoraData_Handle(void);

#endif	//end of __EXAMPLE_RX__
