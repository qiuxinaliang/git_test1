#include "udp_demo.h"  

//UDP接收数据缓冲区
u8 udp_demo_recvbuf[UDP_DEMO_RX_BUFSIZE];	//UDP接收数据缓冲区 
uint8_t ip_D_temp = 0;
uint8_t Array_ip_D[5] = { 0 };
//UDP发送数据内容
const u8 tcp_demo_sendbuf[10] = "Recv OK\r\n";
u8 udp_Ack[10] = "Recv OK\r\n";
struct udp_pcb *udppcb = NULL;  	//定义一个TCP服务器控制块
struct ip_addr rmtipaddr;  	//远端ip地址

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
	Network_Manage_t.remoteip[3] = 100;
	Network_Manage_t.remote_port = 6009;
	
	Network_Manage_t.ip[0] = 192;	
	Network_Manage_t.ip[1] = 168;
	Network_Manage_t.ip[2] = 1;
	Network_Manage_t.ip[3] = 104;
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
		err=udp_bind(udppcb,IP_ADDR_ANY,UDP_DEMO_PORT);//绑定本地IP地址与端口号
		if(err==ERR_OK)	//绑定完成
		{
			udp_recv(udppcb,udp_demo_recv,NULL);//注册接收回调函数 
		}
		else
		{
			udp_remove(udppcb);
			printf("udp_bind fail\n");
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
			printf("recvdata= %s",(uint8_t *)&udp_demo_recvbuf);
		}
		upcb->remote_ip=*addr; 				//记录远程主机的IP地址
		upcb->remote_port=port;  			//记录远程主机的端口号
		lwipdev.remoteip[0]=upcb->remote_ip.addr&0xff; 		//IADDR4
		lwipdev.remoteip[1]=(upcb->remote_ip.addr>>8)&0xff; //IADDR3
		lwipdev.remoteip[2]=(upcb->remote_ip.addr>>16)&0xff;//IADDR2
		lwipdev.remoteip[3]=(upcb->remote_ip.addr>>24)&0xff;//IADDR1 
		ip_D_temp = lwipdev.remoteip[3];
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
		//pbuf_free(ptr);//释放内存
	} 
	pbuf_free(ptr);//释放内存
} 

//UDP服务器发送广播数据
Network_Manage Network_Manage_temp;
void udp_Broadcast_senddata(struct udp_pcb *upcb, uint8_t *data, int len)
{
	int i = 0;
	struct pbuf *ptr;
	 __set_PRIMASK(1);
	ptr=pbuf_alloc(PBUF_TRANSPORT, len, PBUF_POOL); //申请内存
	if(ptr)
	{
		for(i = 0; i < 5; i++)
		{
			ptr->payload=(void *)data; 
			Network_Manage_temp.ip[0] = Network_Manage_t.remoteip[0];
			Network_Manage_temp.ip[1] = Network_Manage_t.remoteip[1];
			Network_Manage_temp.ip[2] = Network_Manage_t.remoteip[2];
			Network_Manage_temp.ip[3] = Array_ip_D[i];
			log_print( DEBUG, ("ip[3] = %d\r\n", Network_Manage_temp.ip[3]));
			if((Network_Manage_temp.ip[3] > 2 ) && (Network_Manage_temp.ip[3] != Network_Manage_t.remoteip[3]))
			{
				IP4_ADDR(&rmtipaddr,Network_Manage_temp.ip[0],Network_Manage_temp.ip[1],Network_Manage_temp.ip[2],Network_Manage_temp.ip[3]);
				upcb->remote_ip = *(ip_addr_t *)&rmtipaddr;
				upcb->remote_port = UDP_DEMO_PORT;
				udp_send(upcb,ptr);	//udp发送数据 
			}
		}
		//pbuf_free(ptr);//释放内存
	}
	pbuf_free(ptr);//释放内存
	 __set_PRIMASK(0);
}

//UDP服务器发送数据
void udp_UWB_senddata(struct udp_pcb *upcb, uint8_t *data, int len)
{
	struct pbuf *ptr;
	__set_PRIMASK(1);
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
		udp_send(upcb,ptr);	//udp发送数据 
		//pbuf_free(ptr);//释放内存
	} 
	pbuf_free(ptr);//释放内存
	__set_PRIMASK(0);
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

extern CalibrationMode_Stru CalibrationMode_Stru_temp;
void UDP_RecvDataHandle(void)
{
	int j = 0;
	char temp[10] = { 0 };
	uint8_t Data_Mark[10] = { 0 };
	uint8_t cmd_mode = 0;
	uint8_t Anchors_Distance_Mark = 0;
	Network_Manage Network_Manage_pbuf = { 0 };
	Distance_ToAngle_Stru *Distance_ToAngle_Stru_temp = NULL;
	AnchorFollowCmd_Stru AnchorFollowCmd_Stru_buf = { 0 };
	uint8_t *chartemp = NULL;
	uint8_t *p = NULL;
	Network_Manage *Network_Manage_t = NULL;
	p = (uint8_t *)strstr((const char *)udp_demo_recvbuf, "HD-Robot-Cmd=1");
	if(p != NULL)
	{
		udp_UWB_senddata(udppcb, (uint8_t *)udp_Ack, 10);
		Upgrade_StatusWrite(); 
		SYS_REBOOT();
	}
	
	// 网路参数的配置
	p = NULL;
	p = (uint8_t *)strstr((const char *)udp_demo_recvbuf, "HD-Robot-Cmd=2,");
	if(p != NULL)
	{
		udp_UWB_senddata(udppcb, (uint8_t *)udp_Ack, 10);
		//Network_Manage_t = (Network_Manage *)mymalloc(SRAMIN, sizeof(Network_Manage));
		//Network_Manage_pbuf = Network_Manage_t;
		chartemp = p + strlen("HD-Robot-Cmd=2,") -1;
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
					Network_Manage_pbuf.remoteip[j - 1] = (uint32_t)atoi((const char *)temp);
				}
				if(j == 5)
				{
					memset((uint8_t *)&temp, 0, sizeof(temp));
					memcpy((uint8_t *)&temp, chartemp + Data_Mark[j - 1] + 1, Data_Mark[j] - Data_Mark[j -1] -1);
					Network_Manage_pbuf.remote_port = (uint32_t)atoi((const char *)temp);
				}
				if((j >= 6)&&(j <= 9))
				{
					memset((uint8_t *)&temp, 0, sizeof(temp));
					memcpy((uint8_t *)&temp, chartemp + Data_Mark[j - 1] + 1, Data_Mark[j] - Data_Mark[j -1] -1);
					Network_Manage_pbuf.ip[j - 6] = (uint32_t)atoi((const char *)temp);
				}
				if(j == 10)
				{
					memset((uint8_t *)&temp, 0, sizeof(temp));
					memcpy((uint8_t *)&temp, chartemp + Data_Mark[j - 1] + 1, Data_Mark[j] - Data_Mark[j -1] -1);
					Network_Manage_pbuf.local_port = (uint32_t)atoi((const char *)temp);
				}
				
				j++;
			}
		}
		Network_Manage_pbuf.net_mark = 0x01;
		STMFLASH_NetworkPara_Config_Write(Network_Manage_pbuf);
	  myfree(SRAMIN,Network_Manage_t);
		SYS_REBOOT();
	}
	
	// 不同模式下基站距离参数配置
	p = NULL;
	p = (uint8_t *)strstr((const char *)udp_demo_recvbuf, "HD-Robot-Cmd=3,");
	if(p != NULL)
	{
		udp_UWB_senddata(udppcb, (uint8_t *)udp_Ack, 10);
		chartemp = p + strlen("HD-Robot-Cmd=3,") -1;
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
					AnchorToAnchor_Distance[0] = (double)atoi((const char *)temp);
				}
				if(j == 2)
				{
					memset((uint8_t *)&temp, 0, sizeof(temp));
					memcpy((uint8_t *)&temp, chartemp + Data_Mark[j - 1] + 1, Data_Mark[j] - Data_Mark[j -1] - 1);
					AnchorToAnchor_Distance[1] = (double)atoi((const char *)temp);
				}
				if(j == 3)
				{
					memset((uint8_t *)&temp, 0, sizeof(temp));
					memcpy((uint8_t *)&temp, chartemp + Data_Mark[j - 1] + 1, Data_Mark[j] - Data_Mark[j -1] - 1);
					AnchorToAnchor_Distance[2] = (double)atoi((const char *)temp);
				}
				j++;
			}
		}
		STMFLASH_Anchors_Distance_Write(0x01);
	}
	
  // 测距数据校准命令
	p = NULL;
	p = (uint8_t *)strstr((const char *)udp_demo_recvbuf, "HD-Robot-Cmd=4,");
	if(p != NULL)
	{
		udp_UWB_senddata(udppcb, (uint8_t *)udp_Ack, 10);
		chartemp = p + strlen("HD-Robot-Cmd=4,") -1;
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
					CalibrationMode_Stru_temp.FollowMode = (uint8_t)atoi((const char *)temp);
					log_print( DEBUG, ("FollowMode = %d\r\n", CalibrationMode_Stru_temp.FollowMode));
				}
				if(j == 2)
				{
					memset((uint8_t *)&temp, 0, sizeof(temp));
					memcpy((uint8_t *)&temp, chartemp + Data_Mark[j - 1] + 1, Data_Mark[j] - Data_Mark[j -1] - 1);
					CalibrationMode_Stru_temp.A_standard_distance = (double)atoi((const char *)temp);
					log_print( DEBUG, ("A_standard_distance = %f\r\n", CalibrationMode_Stru_temp.A_standard_distance));
				}
				if(j == 3)
				{
					memset((uint8_t *)&temp, 0, sizeof(temp));
					memcpy((uint8_t *)&temp, chartemp + Data_Mark[j - 1] + 1, Data_Mark[j] - Data_Mark[j -1] - 1);
					CalibrationMode_Stru_temp.B_standard_distance = (double)atoi((const char *)temp);
					log_print( DEBUG, ("B_standard_distance = %f\r\n", CalibrationMode_Stru_temp.B_standard_distance));
				}
				if(j == 4)
				{
					memset((uint8_t *)&temp, 0, sizeof(temp));
					memcpy((uint8_t *)&temp, chartemp + Data_Mark[j - 1] + 1, Data_Mark[j] - Data_Mark[j -1] - 1);
					CalibrationMode_Stru_temp.standard_angle = (double)atoi((const char *)temp);
					log_print( DEBUG, ("standard_angle = %f\r\n", CalibrationMode_Stru_temp.standard_angle));
				}
				j++;
			}
		}
		ModeTemp = 1;
	}

	/////////////////////////////////////////////////////////////////////////////
	//基站测距数据包的获取 及接收跟随模式命令
	p = NULL;
	p = (uint8_t *)strstr((const char *)udp_demo_recvbuf, "HD");
	if(p != NULL)
	{
		Distance_ToAngle_Stru_temp = (Distance_ToAngle_Stru *)mymalloc(SRAMIN, sizeof(Distance_ToAngle_Stru));
		if(Distance_ToAngle_Stru_temp != NULL)
		{
			memset((uint8_t *)Distance_ToAngle_Stru_temp, 0, sizeof(Distance_ToAngle_Stru));
			memset((uint8_t *)&AnchorFollowCmd_Stru_buf, 0, sizeof(AnchorFollowCmd_Stru));
			memcpy((uint8_t *)Distance_ToAngle_Stru_temp, (uint8_t *)(p),  sizeof(Distance_ToAngle_Stru));
			memcpy((uint8_t *)&AnchorFollowCmd_Stru_buf, (uint8_t *)udp_demo_recvbuf,  sizeof(AnchorFollowCmd_Stru));
			
			cmd_mode = Distance_ToAngle_Stru_temp -> Follwmode;
			switch(cmd_mode)
			{
				case AnchorDataPack:
				{
					if(Distance_ToAngle_Stru_temp ->ender == 0x4225)
						UWB_DistanceToAngle(Distance_ToAngle_Stru_temp);
					break;
				}
				case AheadFollow:
				{
					if(AnchorFollowCmd_Stru_buf.ender == 0x4225)
					{
						log_print( DEBUG, (" AheadFollow\r\n"));
						memset((uint8_t *)&AnchorFollowCmd_Stru_temp, 0, sizeof(AnchorFollowCmd_Stru));
						AnchorFollowCmd_Stru_temp.header = 0x4448;
						AnchorFollowCmd_Stru_temp.ender = 0x4225;
						AnchorFollowCmd_Stru_temp.Follwmode = AheadFollow;
						udp_UWB_senddata(udppcb, (uint8_t *)udp_Ack, 10);
						udp_Broadcast_senddata(udppcb, (uint8_t *)&AnchorFollowCmd_Stru_temp, sizeof(AnchorFollowCmd_Stru));
					}
					break;
				}
				case LeftFollow:
				{
					if(AnchorFollowCmd_Stru_buf.ender == 0x4225)
					{
						log_print( DEBUG, ("LeftFollow\r\n"));
						memset((uint8_t *)&AnchorFollowCmd_Stru_temp, 0, sizeof(AnchorFollowCmd_Stru));
						AnchorFollowCmd_Stru_temp.header = 0x4448;
						AnchorFollowCmd_Stru_temp.ender = 0x4225;
						AnchorFollowCmd_Stru_temp.Follwmode = LeftFollow;
						udp_UWB_senddata(udppcb, (uint8_t *)udp_Ack, 10);
						udp_Broadcast_senddata(udppcb, (uint8_t *)&AnchorFollowCmd_Stru_temp, sizeof(AnchorFollowCmd_Stru));
					}
					break;
				}
				case RightFollow:
				{
					if(AnchorFollowCmd_Stru_buf.ender == 0x4225)
					{
						log_print( DEBUG, ("RightFollow\r\n"));
						memset((uint8_t *)&AnchorFollowCmd_Stru_temp, 0, sizeof(AnchorFollowCmd_Stru));
						AnchorFollowCmd_Stru_temp.header = 0x4448;
						AnchorFollowCmd_Stru_temp.ender = 0x4225;
						AnchorFollowCmd_Stru_temp.Follwmode = RightFollow;
						udp_UWB_senddata(udppcb, (uint8_t *)udp_Ack, 10);
						udp_Broadcast_senddata(udppcb, (uint8_t *)&AnchorFollowCmd_Stru_temp, sizeof(AnchorFollowCmd_Stru));
					}
					break;
				}
				default: break;
			}
		}
	  myfree(SRAMIN,Distance_ToAngle_Stru_temp);
	}
	udp_IPSaveHandle();
}

// 客户端IP保存
void udp_IPSaveHandle(void)
{
	int i = 0;
	uint8_t temp = 0;
	temp = ip_D_temp;
	ip_D_temp = 0;
	do
	{
		if((temp != Array_ip_D[i]) && (temp != 0))
			i++;
		else
		{
			break;
		}
	}while(i < 5);
	if(i == 5)
	{
		for(int j = 0; j < 4; j++)
		{
			Array_ip_D[j] = Array_ip_D[j + 1];
		}
		Array_ip_D[4] = temp;
	}
}
























