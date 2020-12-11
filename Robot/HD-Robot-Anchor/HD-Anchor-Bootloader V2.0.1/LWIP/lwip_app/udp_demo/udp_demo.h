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
   
#define UDP_DEMO_RX_BUFSIZE		1200		//定义udp最大接收数据长度 
#define UDP_DEMO_PORT			6009	//定义udp连接的端口 
#define UDP_LOCAL_PORT		6010	//定义udp连接的端口 

extern u8 udp_demo_recvbuf[UDP_DEMO_RX_BUFSIZE];	//UDP接收数据缓冲区  
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

#endif

