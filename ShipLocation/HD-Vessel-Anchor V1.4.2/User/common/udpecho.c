#include "udpecho.h"
#include "lwip/opt.h"
#include <lwip/sockets.h>
#if LWIP_NETCONN
#include "lwip/api.h"
#include "lwip/sys.h"

extern Network_Manage Network_Manage_stru_temp; // 网络参数保存

#define IPADDR_Remote      (u32_t)((E_Network_Manage_stru_temp.Centeripaddress[0] <<24) | (E_Network_Manage_stru_temp.Centeripaddress[1] <<16)|\
                              (E_Network_Manage_stru_temp.Centeripaddress[2] <<8) | (E_Network_Manage_stru_temp.Centeripaddress[3]))

#define IPADDR_Local      (u32_t)((E_Network_Manage_stru_temp.Anchoripaddress[0] <<24) | (E_Network_Manage_stru_temp.Anchoripaddress[1] <<16)|\
                              (E_Network_Manage_stru_temp.Anchoripaddress[2] <<8) | (E_Network_Manage_stru_temp.Anchoripaddress[3]))

/*
*********************************************************************************************************
*	函 数 名: udpecho_thread
*	功能说明: 创建UDP线程
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
int sock = -1;
extern uint8_t NetworkRecvData_Len;
extern NetworkRecvData_Stru NetworkRecvData_Stru_temp;
extern uint8_t Network_Communciton_Flag;
static void udpecho_thread(void *arg)
{
  char *recv_data;
  struct sockaddr_in udp_addr,seraddr;
	ip_addr_t rmtipaddr;
  int recv_data_len;
  socklen_t addrlen;
  ip4_addr_t ipaddr;

  log_print(DEBUG,("local_port = %d\n\n",LOCAL_PORT));
  while(1)
  {
    recv_data = (char *)pvPortMalloc(RECV_DATA);
    if (recv_data == NULL)
    {
			log_print(DEBUG,("No memory\n"));
			goto __exit;
    }
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
      log_print(ERR,("Socket error\n"));
      goto __exit;
    }
    udp_addr.sin_family = AF_INET;
    udp_addr.sin_addr.s_addr = htonl(IPADDR_Local);//192.168.1.104)  IPADDR_ANY IPADDR_Remote  ipaddr
    udp_addr.sin_port = htons(Network_Manage_stru_temp.AnchorPort);
    memset(&(udp_addr.sin_zero), 0, sizeof(udp_addr.sin_zero));
    if (bind(sock, (struct sockaddr *)&udp_addr, sizeof(struct sockaddr)) == -1)
    {
      log_print(ERR,("Unable to bind\n"));
      goto __exit;
    }
		
		udp_addr.sin_family = AF_INET;
    udp_addr.sin_addr.s_addr = htonl(IPADDR_Remote);//192.168.1.104)  IPADDR_ANY IPADDR_Remote
    udp_addr.sin_port = htons(E_Network_Manage_stru_temp.CenterPort);
    memset(&(udp_addr.sin_zero), 0, sizeof(udp_addr.sin_zero));
    if (connect(sock, (struct sockaddr *)&udp_addr, sizeof(struct sockaddr)) == -1)
    {
      log_print(ERR,("Unable to connect\n"));
      goto __exit;
    }
		
    while(1)
    {
      recv_data_len = recvfrom(sock,recv_data, RECV_DATA,0, (struct sockaddr*)&seraddr, &addrlen);   
      if(recv_data_len >= 0)
      {
        memset((uint8_t*)&NetworkRecvData_Stru_temp, 0, sizeof(NetworkRecvData_Stru));
        NetworkRecvData_Stru_temp.NetworkRecvData_Len = recv_data_len;
        memcpy((uint8_t *)NetworkRecvData_Stru_temp.NetworkRecvData_Buffer, (uint8_t *)recv_data, recv_data_len);
        Network_Communciton_Flag = 1;
				//printf("RX\n");
				USART1_For_Debug_tx(NetworkRecvData_Stru_temp.NetworkRecvData_Len, (uint8_t *)NetworkRecvData_Stru_temp.NetworkRecvData_Buffer);
				//printf("\n");
        //log_print(DEBUG,("receive from %s\n",inet_ntoa(seraddr.sin_addr)));		/*显示发送端的IP地址*/
				//log_print(DEBUG,("len = %d\n",NetworkRecvData_Len));		/*显示发送端的IP地址*/
        //log_print(DEBUG,("recevce:%s",(uint8_t *)&NetworkRecvData_Stru_temp.NetworkRecvData_Buffer));		/*显示发送端发来的字串*/
        HAL_GPIO_WritePin(LED_GPIO_PORT, LED_GPIO_PIN, GPIO_PIN_RESET);
        vTaskDelay(10);
        HAL_GPIO_WritePin(LED_GPIO_PORT, LED_GPIO_PIN, GPIO_PIN_SET);
        vTaskDelay(20);
				//vTaskDelay(20);/* 延时2000个tick */
				//HAL_GPIO_TogglePin(LED_GPIO_PORT, LED_GPIO_PIN);
      }
			//vTaskDelay(50);/* 延时2000个tick */
    }
__exit:
    if (sock >= 0) closesocket(sock);
    if (recv_data) free(recv_data);
  }
}

/*
*********************************************************************************************************
*	函 数 名: UDP_SendDataPacket
*	功能说明: UDP发送函数封装
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void UDP_SendDataPacket(uint8_t *data,uint16_t data_len)
{
	struct sockaddr_in seraddr;
	seraddr.sin_family = AF_INET;
	seraddr.sin_addr.s_addr = htonl(IPADDR_Remote); //IPADDR_ANY IPADDR_Remote
	seraddr.sin_port = htons(E_Network_Manage_stru_temp.CenterPort);
	memset(&(seraddr.sin_zero), 0, sizeof(seraddr.sin_zero));
  taskENTER_CRITICAL();	
	sendto(sock,data, data_len, 0,(struct sockaddr*)&seraddr,16);
	taskEXIT_CRITICAL();
}

/*
*********************************************************************************************************
*	函 数 名: udpecho_init
*	功能说明: UDP线程初始化
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void udpecho_init(void)
{
  sys_thread_new("udpecho_thread", udpecho_thread, NULL, 2048, 4);
}

#endif /* LWIP_NETCONN */
