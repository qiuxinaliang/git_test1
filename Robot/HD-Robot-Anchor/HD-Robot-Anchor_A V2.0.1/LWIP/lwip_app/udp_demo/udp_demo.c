#include "udp_demo.h"  
//UDP接收数据缓冲区
u8 udp_demo_recvbuf[UDP_DEMO_RX_BUFSIZE];	//UDP接收数据缓冲区 
//UDP发送数据内容
const u8 *tcp_demo_sendbuf="Apollo STM32F4/F7 UDP demo send data\r\n";
const u8 udp_Ack[10] = "Recv OK\r\n";
const u8 udp_Err[10] = "Recv Err\r\n";
const u8 udp_ipErr[15] = "ip config err\r\n";
struct udp_pcb *udppcb = NULL;  	//定义一个TCP服务器控制块
struct ip_addr rmtipaddr;  	//远端ip地址
//UDP 测试全局状态标记变量
//bit7:没有用到
//bit6:0,没有收到数据;1,收到数据了.
//bit5:0,没有连接上;1,连接上了.
//bit4~0:保留
u8 udp_demo_flag;
u8 UDP_Create_Status = true;

Network_Manage Network_Manage_t;
// 网络配置初始化
void udp_Network_Init(void)
{
		//默认远端IP为:192.168.1.100
	Network_Manage_t.remoteip[0] = 192;	
	Network_Manage_t.remoteip[1] = 168;
	Network_Manage_t.remoteip[2] = 1;
	Network_Manage_t.remoteip[3] = 104;
	Network_Manage_t.remote_port = 6009;
	
	Network_Manage_t.ip[0] = 192;	
	Network_Manage_t.ip[1] = 168;
	Network_Manage_t.ip[2] = 1;
	Network_Manage_t.ip[3] = 32;
	Network_Manage_t.local_port = 6009;
}

//设置远端IP地址
void udp_demo_set_remoteip(void)
{                  
	lwipdev.remoteip[0] = Network_Manage_t.remoteip[0];
	lwipdev.remoteip[1] = Network_Manage_t.remoteip[1];
	lwipdev.remoteip[2] = Network_Manage_t.remoteip[2]; 
	lwipdev.remoteip[3] = Network_Manage_t.remoteip[3];
} 

void udp_create(void)
{
	err_t err;
	udp_demo_set_remoteip();//先选择IP
	udppcb = udp_new();
	if(udppcb)//创建成功
	{ 
		IP4_ADDR(&rmtipaddr,lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2],lwipdev.remoteip[3]);
		err=udp_connect(udppcb,&rmtipaddr,Network_Manage_t.remote_port);//UDP客户端连接到指定IP地址和端口号的服务器
		if(err==ERR_OK)
		{
			err=udp_bind(udppcb,IP_ADDR_ANY,Network_Manage_t.local_port);//绑定本地IP地址与端口号
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
	}else
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
void udp_UWB_senddata(struct udp_pcb *upcb, uint8_t *data, int len)
{
	Network_Manage Network_Manage_temp;
	struct pbuf *ptr;
	ptr=pbuf_alloc(PBUF_TRANSPORT,len,PBUF_POOL); //申请内存
	if(ptr)
	{
		ptr->payload=(void *)data; 
		Network_Manage_temp.ip[0] = Network_Manage_t.remoteip[0];
		Network_Manage_temp.ip[1] = Network_Manage_t.remoteip[1];
		Network_Manage_temp.ip[2] = Network_Manage_t.remoteip[2];
		Network_Manage_temp.ip[3] = Network_Manage_t.remoteip[3];
		IP4_ADDR(&rmtipaddr,Network_Manage_temp.ip[0],Network_Manage_temp.ip[1],Network_Manage_temp.ip[2],Network_Manage_temp.ip[3]);
		upcb->remote_ip = *(ip_addr_t *)&rmtipaddr;
		upcb->remote_port = Network_Manage_t.remote_port;
		upcb->local_port = Network_Manage_t.local_port;
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

// 接收数据处理
uint8_t ModeTemp_Flag = 0;
AnchorFollowCmd_Stru AnchorFollowCmd_Stru_temp;
Network_Manage *Network_Manage_pbuf = NULL;
//AnchorFollowMode_Stru AnchorFollowMode_Stru_temp;
extern int AnchorMode_Flag; 
extern uint8_t UWB_address_temp;
void UDP_RecvDataHandle(void)
{
	int j = 0;
	char temp[10] = { 0 };
	uint8_t *chartemp = NULL;
	uint8_t cmd_mode = 0;
	uint8_t *p = NULL;
	Network_Manage *Network_Manage_t = NULL;
	Distance_ToAngle_Stru *Distance_ToAngle_Stru_temp = NULL;
	uint8_t Data_Mark[10] = { 0 };
	// 基站UWB地址写入
	p = NULL;
	p = (uint8_t *)strstr((const char *)udp_demo_recvbuf, "HD-Robot-Cmd=0,");
	if(p != NULL)
	{
		chartemp = p + strlen("HD-Robot-Cmd=0,") -1;
		log_print( DEBUG, ("%s\r\n", chartemp));
		j = 0;
		for(int i = 0; i < strlen((const char *)chartemp); i++)
		{
			if(chartemp[i] == ',')
			{
				Data_Mark[j] = i;
				if(j == 1)
				{
					memset((uint8_t *)&temp, 0, sizeof(temp));
					memcpy((uint8_t *)&temp, chartemp + Data_Mark[j - 1] + 1, Data_Mark[j] - Data_Mark[j -1] - 1);
					UWB_address_temp = (uint8_t)atoi((const char *)temp);
					log_print( DEBUG, ("UWB_address_temp = %d\r\n", UWB_address_temp));
				}
				j++;
			}
		}
		if((UWB_address_temp > 0) && (UWB_address_temp < 10))
		{
			udp_UWB_senddata(udppcb, (uint8_t *)udp_Ack, 10);
			UWB_AnchorID_Write(UWB_address_temp);
      SYS_REBOOT();
		}
		else
		{
			udp_UWB_senddata(udppcb, (uint8_t *)udp_Err, 10);
		}
	}
	// 升级命令发送
	p = NULL;
	p = (uint8_t *)strstr((const char *)udp_demo_recvbuf, "HD-Robot-Cmd=1");
	if(p != NULL)
	{
		//Upgrade_StatusWrite(); 
    udp_UWB_senddata(udppcb, (uint8_t *)udp_Ack, 10);
    Dev_Upgrade_Write(0x01);
		SYS_REBOOT();
	}
	// 基站工作模式配置
	p = NULL;
	p = (uint8_t *)strstr((const char *)udp_demo_recvbuf, "HD-Robot-Cmd=2,");
	if(p != NULL)
	{
		udp_UWB_senddata(udppcb, (uint8_t *)udp_Ack, 10);
		chartemp = p + strlen("HD-Robot-Cmd=2,") -1;
		log_print( DEBUG, ("%s\r\n", chartemp));
		j = 0;
		for(int i = 0; i < strlen((const char *)chartemp); i++)
		{
			if((chartemp[i] == ',') ||(chartemp[i] == '.'))
			{
				Data_Mark[j] = i;
				if(j == 1)
				{
					memset((uint8_t *)&temp, 0, sizeof(temp));
					memcpy((uint8_t *)&temp, chartemp + Data_Mark[j - 1] + 1, Data_Mark[j] - Data_Mark[j -1] - 1);
					Anchor_Workmode = (uint8_t)atoi((const char *)temp);
					log_print( DEBUG, ("WorkingMode = %d\r\n", Anchor_Workmode));
				}
				if(j == 2)
				{
					memset((uint8_t *)&temp, 0, sizeof(temp));
					memcpy((uint8_t *)&temp, chartemp + Data_Mark[j - 1] + 1, Data_Mark[j] - Data_Mark[j -1] -1);
					//AnchorFollowMode_Stru_temp.DeviceId = (uint32_t)atoi((const char *)temp);
					//log_print( DEBUG, ("DeviceId = %d\r\n", AnchorFollowMode_Stru_temp.DeviceId));
				}
				j++;
			}
		}
		//AnchorFollowMode_Stru_temp.WorkingMode_Mark = 0x01;
    Dev_Workmode_Write(Anchor_Workmode);
		//STMFLASH_AnchorFollowMode_Write();
	}
	
	
		// 网路参数的配置
	p = NULL;
	p = (uint8_t *)strstr((const char *)udp_demo_recvbuf, "HD-Robot-Cmd=3,");
	if(p != NULL)
	{
		udp_UWB_senddata(udppcb, (uint8_t *)udp_Ack, 10);
		Network_Manage_t = (Network_Manage *)mymalloc(SRAMIN, sizeof(Network_Manage));
		Network_Manage_pbuf = Network_Manage_t;
		chartemp = p + strlen("HD-Robot-Cmd=3,") -1;
		log_print( DEBUG, ("%s\r\n", chartemp));
		j = 0;
		for(int i = 0; i < strlen((const char *)chartemp); i++)
		{
			if((chartemp[i] == ',') ||(chartemp[i] == '.'))
			{
				Data_Mark[j] = i;
				if((j >= 1)&&(j <= 4))
				{
					memset((uint8_t *)&temp, 0, sizeof(temp));
					memcpy((uint8_t *)&temp, chartemp + Data_Mark[j - 1] + 1, Data_Mark[j] - Data_Mark[j -1] - 1);
					Network_Manage_t->remoteip[j - 1] = (uint32_t)atoi((const char *)temp);
				}
				if(j == 5)
				{
					memset((uint8_t *)&temp, 0, sizeof(temp));
					memcpy((uint8_t *)&temp, chartemp + Data_Mark[j - 1] + 1, Data_Mark[j] - Data_Mark[j -1] -1);
					Network_Manage_t->remote_port = (uint32_t)atoi((const char *)temp);
				}
				if((j >= 6)&&(j <= 9))
				{
					memset((uint8_t *)&temp, 0, sizeof(temp));
					memcpy((uint8_t *)&temp, chartemp + Data_Mark[j - 1] + 1, Data_Mark[j] - Data_Mark[j -1] -1);
					Network_Manage_t->ip[j - 6] = (uint32_t)atoi((const char *)temp);
				}
				if(j == 10)
				{
					memset((uint8_t *)&temp, 0, sizeof(temp));
					memcpy((uint8_t *)&temp, chartemp + Data_Mark[j - 1] + 1, Data_Mark[j] - Data_Mark[j -1] -1);
					Network_Manage_t->local_port = (uint32_t)atoi((const char *)temp);
					log_print(DEBUG,("local_port = %d\r\n",Network_Manage_t->local_port));
				}
				
				j++;
			}
		}
		if((Network_Manage_t->remoteip[0] == Network_Manage_t->ip[0])&& (Network_Manage_t->remoteip[1] == Network_Manage_t->ip[1])
			&&(Network_Manage_t->remoteip[2] == Network_Manage_t->ip[2]) && (Network_Manage_t->remoteip[3] != Network_Manage_t->ip[3]))
		{
			Network_Manage_pbuf->net_mark = 0x01;
			Network_Manage_pbuf->local_port = 6009;
			STMFLASH_NetworkPara_Config_Write(Network_Manage_pbuf);
		}
		else
		{
			udp_UWB_senddata(udppcb, (uint8_t *)udp_ipErr, 15);
		}
	  myfree(SRAMIN,Network_Manage_t);
		SYS_REBOOT();
	}
	
	// UWB基站IP配置
	
	
	// 基站跟随模式配置
	p = NULL;
	p = (uint8_t *)strstr((const char *)udp_demo_recvbuf, "HD");
	if(p != NULL)
	{
		Distance_ToAngle_Stru_temp = (Distance_ToAngle_Stru *)mymalloc(SRAMIN, sizeof(Distance_ToAngle_Stru));
		if(Distance_ToAngle_Stru_temp != NULL)
		{
			memset((uint8_t *)Distance_ToAngle_Stru_temp, 0, sizeof(Distance_ToAngle_Stru));
			memcpy((uint8_t *)Distance_ToAngle_Stru_temp, (uint8_t *)(p),  sizeof(Distance_ToAngle_Stru));
			cmd_mode = Distance_ToAngle_Stru_temp -> Follwmode;
			switch(cmd_mode)
			{
				case AheadFollow:
				{
					log_print( DEBUG, (" AheadFollow\r\n"));
					memset((uint8_t *)&AnchorFollowCmd_Stru_temp, 0, sizeof(AnchorFollowCmd_Stru));
					AnchorFollowCmd_Stru_temp.Follwmode = AheadFollow;
					if((Anchor_Workmode == 0x02)||(Anchor_Workmode == 0x03))
					{
						AnchorMode_Flag = 1;
					}
					else
					{
						AnchorMode_Flag = 2;
					}
					break;
				}
				case LeftFollow:
				{
					log_print( DEBUG, ("LeftFollow\r\n"));
					memset((uint8_t *)&AnchorFollowCmd_Stru_temp, 0, sizeof(AnchorFollowCmd_Stru));
					AnchorFollowCmd_Stru_temp.Follwmode = LeftFollow;
					if((Anchor_Workmode == 0x03)||(Anchor_Workmode == 0x05))
					{
						AnchorMode_Flag = 1;
					}
					else
					{
						AnchorMode_Flag = 2;
					}
					break;
				}
				case RightFollow:
				{
					log_print( DEBUG, ("RightFollow\r\n"));
					memset((uint8_t *)&AnchorFollowCmd_Stru_temp, 0, sizeof(AnchorFollowCmd_Stru));
					AnchorFollowCmd_Stru_temp.Follwmode = RightFollow;
					if((Anchor_Workmode == 0x02)||(Anchor_Workmode == 0x04))
					{
						AnchorMode_Flag = 1;
					}
					else
					{
						AnchorMode_Flag = 2;
					}
					break;
				}
				default: break;
			}
      AnchorFollowCmd_Stru_temp.header = 0x4448;
      AnchorFollowCmd_Stru_temp.header = 0x4B4F;
      udp_UWB_senddata(udppcb, (uint8_t *)&AnchorFollowCmd_Stru_temp, sizeof(AnchorFollowCmd_Stru));
		}
	  myfree(SRAMIN,Distance_ToAngle_Stru_temp);
	}
}

#if 0
void AnchorFollowMode_default_set(void)
{
	AnchorFollowMode_Stru_temp.WorkingMode_Mark = 0x00;
	AnchorFollowMode_Stru_temp.DeviceId = address[0];
	AnchorFollowMode_Stru_temp.WorkingMode = AheadAndLeftFollow;
}
#endif

























