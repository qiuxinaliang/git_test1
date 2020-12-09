#include "udp_demo.h"  
//UDP接收数据缓冲区
u8 udp_demo_recvbuf[UDP_DEMO_RX_BUFSIZE];	//UDP接收数据缓冲区 
//UDP发送数据内容
const u8 *tcp_demo_sendbuf="Apollo STM32F4/F7 UDP demo send data\r\n";
struct udp_pcb *udppcb = NULL;  	//定义一个TCP服务器控制块
struct ip_addr rmtipaddr;  	//远端ip地址
//UDP 测试全局状态标记变量
//bit7:没有用到
//bit6:0,没有收到数据;1,收到数据了.
//bit5:0,没有连接上;1,连接上了.
//bit4~0:保留
u8 udp_demo_flag;
u8 UDP_Create_Status = true;

extern Network_Manage Network_Manage_stru_temp; // 网络参数保存
//Network_Manage Network_Manage_t;

// 网络配置初始化
void udp_Network_Init(void)
{
	
	Network_Manage_stru_temp.ip[0] = 192;	
	Network_Manage_stru_temp.ip[1] = 168;
	Network_Manage_stru_temp.ip[2] = 1;
	Network_Manage_stru_temp.ip[3] = 104;
	Network_Manage_stru_temp.local_port = 6008;
	
	Network_Manage_stru_temp.Subnetmask[0] = 255;
  Network_Manage_stru_temp.Subnetmask[1] = 255;
  Network_Manage_stru_temp.Subnetmask[2] = 255;
  Network_Manage_stru_temp.Subnetmask[3] = 0;

  Network_Manage_stru_temp.Gateway[0] = 192;
  Network_Manage_stru_temp.Gateway[1] = 168;
  Network_Manage_stru_temp.Gateway[2] = 1;
  Network_Manage_stru_temp.Gateway[3] = 1;
	
	Network_Manage_stru_temp.remoteip[0] = 192;
  Network_Manage_stru_temp.remoteip[1] = 168;
  Network_Manage_stru_temp.remoteip[2] = 1;
  Network_Manage_stru_temp.remoteip[3] = 100;
  Network_Manage_stru_temp.remote_port = 6008;

  Network_Manage_stru_temp.Engineipaddress[0] = 192;
  Network_Manage_stru_temp.Engineipaddress[1] = 168;
  Network_Manage_stru_temp.Engineipaddress[2] = 1;
  Network_Manage_stru_temp.Engineipaddress[3] = 100;
  Network_Manage_stru_temp.EnginePort = 6008;
}


//设置远端IP地址
//设置远端IP地址
void udp_demo_set_remoteip(void)
{                  
	lwipdev.remoteip[0] = Network_Manage_stru_temp.remoteip[0];
	lwipdev.remoteip[1] = Network_Manage_stru_temp.remoteip[1];
	lwipdev.remoteip[2] = Network_Manage_stru_temp.remoteip[2]; 
	lwipdev.remoteip[3] = Network_Manage_stru_temp.remoteip[3];
} 

void udp_create(void)
{
	err_t err;
	udp_demo_set_remoteip();//先选择IP
	udppcb = udp_new();
	if(udppcb)//创建成功
	{ 
		IP4_ADDR(&rmtipaddr,lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2],lwipdev.remoteip[3]);
		err=udp_connect(udppcb,&rmtipaddr,UDP_DEMO_PORT);//UDP客户端连接到指定IP地址和端口号的服务器
		if(err==ERR_OK)
		{
			err=udp_bind(udppcb,IP_ADDR_ANY,UDP_DEMO_PORT);//绑定本地IP地址与端口号
			if(err==ERR_OK)	//绑定完成 
			{
				udp_recv(udppcb,udp_demo_recv,NULL);//注册接收回调函数 
			}
			else
			{
				printf("udp_bind fail\n");
        UDP_Create_Status = false;
			}
		}
		else
		{
			printf("udp_connect fail\n");
      UDP_Create_Status = false;
		}
	}
	else
	{
		printf("udp_create fail\n");
    UDP_Create_Status = false;
	}
	// 创建失败，指示灯慢闪
	if(UDP_Create_Status == false)
	{
		while(1)
    {
      HAL_GPIO_TogglePin(LED_GPIO_PORT, LED_GPIO_PIN);
      delay_ms(1000);
    }
	}

}

void udp_demo_test(void)
{
	udp_create();
	while(1)
	{
		
		udp_demo_senddata(udppcb);
		
		if(udp_demo_flag&1<<6)//是否收到数据?
		{			
			udp_demo_flag&=~(1<<6);//标记数据已经被处理了.
		} 
		lwip_periodic_handle();
		delay_ms(2000);
	}
	udp_demo_connection_close(udppcb); 
} 

//UDP服务器回调函数
void udp_demo_recv(void *arg,struct udp_pcb *upcb,struct pbuf *p,struct ip_addr *addr,u16_t port)
{
	u32 data_len = 0;
	struct pbuf *q;
	if(p!=NULL)	//接收到不为空的数据时
	{
		memset(udp_demo_recvbuf,0,UDP_DEMO_RX_BUFSIZE);  //数据接收缓冲区清零
		for(q=p;q!=NULL;q=q->next)  //遍历完整个pbuf链表
		{
			//判断要拷贝到UDP_DEMO_RX_BUFSIZE中的数据是否大于UDP_DEMO_RX_BUFSIZE的剩余空间，如果大于
			//的话就只拷贝UDP_DEMO_RX_BUFSIZE中剩余长度的数据，否则的话就拷贝所有的数据
			if(q->len > (UDP_DEMO_RX_BUFSIZE-data_len)) memcpy(udp_demo_recvbuf+data_len,q->payload,(UDP_DEMO_RX_BUFSIZE-data_len));//拷贝数据
			else memcpy(udp_demo_recvbuf+data_len,q->payload,q->len);
			data_len += q->len;  	
			if(data_len > UDP_DEMO_RX_BUFSIZE) break; //超出TCP客户端接收数组,跳出	
			//printf("recvdata= %s",(uint8_t *)&udp_demo_recvbuf);
		}
		upcb->remote_ip=*addr; 				//记录远程主机的IP地址
		upcb->remote_port=port;  			//记录远程主机的端口号
		lwipdev.remoteip[0]=upcb->remote_ip.addr&0xff; 		//IADDR4
		lwipdev.remoteip[1]=(upcb->remote_ip.addr>>8)&0xff; //IADDR3
		lwipdev.remoteip[2]=(upcb->remote_ip.addr>>16)&0xff;//IADDR2
		lwipdev.remoteip[3]=(upcb->remote_ip.addr>>24)&0xff;//IADDR1 
		udp_demo_flag|=1<<6;	//标记接收到数据了
		pbuf_free(p);//释放内存
	}
	else
	{
		udp_disconnect(upcb); 
	} 
} 
//UDP服务器发送数据
void udp_demo_senddata(struct udp_pcb *upcb)
{
	struct pbuf *ptr;
	ptr=pbuf_alloc(PBUF_TRANSPORT,strlen((char*)tcp_demo_sendbuf),PBUF_POOL); //申请内存
	if(ptr)
	{
		ptr->payload=(void *)tcp_demo_sendbuf; 
		udp_send(upcb,ptr);	//udp发送数据 
		pbuf_free(ptr);//释放内存
	} 
} 

//UDP服务器发送数据
//extern Network_Manage Network_Manage_temp;
void udp_UWB_senddata(struct udp_pcb *upcb, uint8_t *data, int len)
{
	//uint8_t ip[4];
	Network_Manage udp_Network_Manage_temp;
	uint16_t remote_port = 6009;
	struct pbuf *ptr;
	ptr=pbuf_alloc(PBUF_TRANSPORT,len,PBUF_POOL); //申请内存
	if(ptr)
	{
		ptr->payload=(void *)data; 
		udp_Network_Manage_temp.ip[0] = Network_Manage_stru_temp.remoteip[0];
		udp_Network_Manage_temp.ip[1] = Network_Manage_stru_temp.remoteip[1];
		udp_Network_Manage_temp.ip[2] = Network_Manage_stru_temp.remoteip[2];
		udp_Network_Manage_temp.ip[3] = Network_Manage_stru_temp.remoteip[3];
		IP4_ADDR(&rmtipaddr,udp_Network_Manage_temp.ip[0],udp_Network_Manage_temp.ip[1],udp_Network_Manage_temp.ip[2],udp_Network_Manage_temp.ip[3]);
		upcb->remote_ip = *(ip_addr_t *)&rmtipaddr;
		upcb->remote_port = Network_Manage_stru_temp.remote_port;
		udp_send(upcb,ptr);	//udp发送数据 
		pbuf_free(ptr);//释放内存
	} 
} 

//关闭tcp连接
void udp_demo_connection_close(struct udp_pcb *upcb)
{
	udp_disconnect(upcb); 
	udp_remove(upcb);		//断开UDP连接 
}

























