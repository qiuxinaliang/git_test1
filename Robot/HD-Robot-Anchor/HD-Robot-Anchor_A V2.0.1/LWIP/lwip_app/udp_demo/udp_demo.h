/*
 * @:*********************************************************************************************************: 
 * @Date: 2020-09-29 16:10:56
 * @LastEditTime: 2020-09-30 14:02:17
 * @**********************************************************************************************************: 
 */
#ifndef __UDP_DEMO_H
#define __UDP_DEMO_H
#include "sys.h"
#include "lwip_comm.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
#include "common.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//UDP 测试代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/1/25
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//*******************************************************************************
//修改信息
//无
////////////////////////////////////////////////////////////////////////////////// 	   
 
#define UDP_DEMO_RX_BUFSIZE		200		//定义udp最大接收数据长度 
#define UDP_DEMO_PORT			6009	//定义udp连接的端口 
#define UDP_LOCAL_PORT		6010	//定义udp连接的端口 

#define AheadFollow 0x01
#define LeftFollow 0x02
#define RightFollow 0x03
#define AheadAndLeftFollow 0x04
#define AheadAndRightFollow 0x05

// 基站跟随命令处理结构体

// 给基站发送跟随命令
#pragma pack(1)
typedef struct AnchorFollowCmd_Stru
{
  uint16_t header;
	uint8_t Follwmode;
  uint16_t ender; 
}AnchorFollowCmd_Stru;
#pragma pack()

extern uint8_t ModeTemp_Flag; 
extern struct udp_pcb *udppcb; 
extern struct ip_addr rmtipaddr;  	//远端ip地址
extern u8 udp_demo_flag;
 
void udp_Network_Init(void);
void udp_demo_set_remoteip(void);
void udp_create(void);
void udp_demo_test(void);
void udp_demo_recv(void *arg,struct udp_pcb *upcb,struct pbuf *p,struct ip_addr *addr,u16_t port);
void udp_demo_senddata(struct udp_pcb *upcb);
void udp_UWB_senddata(struct udp_pcb *upcb, uint8_t *data, int len);
void udp_demo_connection_close(struct udp_pcb *upcb);
void UDP_RecvDataHandle(void);
void AnchorFollowMode_default_set(void);

#endif

