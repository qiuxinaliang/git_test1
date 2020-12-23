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
 
#define UDP_DEMO_RX_BUFSIZE		200		//定义udp最大接收数据长度 
#define UDP_DEMO_PORT			6009	//定义udp连接的端口 
#define UDP_LOCAL_PORT		6010	//定义udp连接的端口 

#define AheadFollow 0x01
#define LeftFollow 0x02
#define RightFollow 0x03

// 基站跟随命令处理结构体
#pragma pack(1)
typedef struct 
{
	uint32_t DeviceId;
	uint8_t AnchorEnOrDisable;
	uint8_t DataSendEnable;
  uint32_t reserve; 
}AnchorFollowMode_Stru;
#pragma pack()

// 控制器命令发送结构体
#pragma pack(1)
typedef struct
{
	uint8_t RobotFollowMode;
	AnchorFollowMode_Stru AnchorFollowMode_Stru_temp[4];
}RobotFollowMode_Stru;
#pragma pack()

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
	uint8_t FollowMode;
	uint8_t BatteryVoltage;
  uint16_t ender; 
}distance_SendPackets;
#pragma pack()

extern uint8_t Array_ip_D[5]; 
extern u8 udp_demo_recvbuf[UDP_DEMO_RX_BUFSIZE];
extern uint8_t ModeTemp_Flag; 
extern struct udp_pcb *udppcb; 
extern struct ip_addr rmtipaddr;  	//远端ip地址
extern u8 udp_demo_flag;
 
void udp_Network_Init(void);
void udp_create(void);
void udp_demo_test(void);
void udp_demo_recv(void *arg,struct udp_pcb *upcb,struct pbuf *p,struct ip_addr *addr,u16_t port);
void udp_demo_senddata(struct udp_pcb *upcb);
void udp_UWB_senddata(struct udp_pcb *upcb, uint8_t *data, int len);
void udp_demo_connection_close(struct udp_pcb *upcb);
void UDP_RecvDataHandle(void);
void udp_Broadcast_senddata(struct udp_pcb *upcb, uint8_t *data, int len);
void udp_IPSaveHandle(void);

#endif

