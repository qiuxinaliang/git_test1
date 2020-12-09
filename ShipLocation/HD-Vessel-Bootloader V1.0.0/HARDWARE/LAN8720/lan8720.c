#include "lan8720.h"
#include "lwip_comm.h"
#include "delay.h"
#include "malloc.h"
 	
ETH_HandleTypeDef ETH_Handler;      //以太网句柄

ETH_DMADescTypeDef *DMARxDscrTab;	//以太网DMA接收描述符数据结构体指针
ETH_DMADescTypeDef *DMATxDscrTab;	//以太网DMA发送描述符数据结构体指针 
uint8_t *Rx_Buff; 								//以太网底层驱动接收buffers指针 
uint8_t *Tx_Buff; 								//以太网底层驱动发送buffers指针
  
/*
*********************************************************************************************************
*	函 数 名:LAN8720_Init
*	功能说明: 以太网硬件初始化
*	形    参: 无
*	返 回 值: 0:成功， 1：失败
*********************************************************************************************************
*/
u8 LAN8720_Init(void)
{      
	u8 macaddress[6];
	
	INTX_DISABLE();                         //关闭所有中断，复位过程不能被打断！
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_RESET);	//PB0置1
	delay_ms(100);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_SET);	//PB0置1
	delay_ms(100);
	INTX_ENABLE();                          //开启所有中断

  macaddress[0]=lwipdev.mac[0]; 
	macaddress[1]=lwipdev.mac[1]; 
	macaddress[2]=lwipdev.mac[2];
	macaddress[3]=lwipdev.mac[3];   
	macaddress[4]=lwipdev.mac[4];
	macaddress[5]=lwipdev.mac[5];
        
	ETH_Handler.Instance = ETH;
	ETH_Handler.Init.AutoNegotiation = ETH_AUTONEGOTIATION_ENABLE;//使能自协商模式 
	ETH_Handler.Init.Speed = ETH_SPEED_100M;//速度100M,如果开启了自协商模式，此配置就无效
	ETH_Handler.Init.DuplexMode = ETH_MODE_FULLDUPLEX;//全双工模式，如果开启了自协商模式，此配置就无效
	ETH_Handler.Init.PhyAddress = LAN8720_PHY_ADDRESS;//LAN8720地址  
	ETH_Handler.Init.MACAddr = macaddress;            //MAC地址  
	ETH_Handler.Init.RxMode = ETH_RXINTERRUPT_MODE;   //中断接收模式 
	ETH_Handler.Init.ChecksumMode = ETH_CHECKSUM_BY_HARDWARE;//硬件帧校验  
	ETH_Handler.Init.MediaInterface = ETH_MEDIA_INTERFACE_RMII;//RMII接口  
	if(HAL_ETH_Init(&ETH_Handler) == HAL_OK)
	{
		return 0;   //成功
	}
	else return 1;  //失败
}

/*
*********************************************************************************************************
*	函 数 名:HAL_ETH_MspInit
*	功能说明: ETH底层驱动，时钟使能，引脚配置，此函数会被HAL_ETH_Init()调用
*	形    参: heth:以太网句柄
*	返 回 值: 无
*********************************************************************************************************
*/
void HAL_ETH_MspInit(ETH_HandleTypeDef *heth)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	__HAL_RCC_ETH_CLK_ENABLE();             //开启ETH时钟
	__HAL_RCC_GPIOA_CLK_ENABLE();			//开启GPIOA时钟
	__HAL_RCC_GPIOB_CLK_ENABLE();			//开启GPIOB时钟
	__HAL_RCC_GPIOC_CLK_ENABLE();			//开启GPIOC时钟
	

	GPIO_Initure.Pin=GPIO_PIN_3; //PB1,0
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
	GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);

	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_SET);	//PB0置1 
	
	//PA1,2,7
	GPIO_Initure.Pin=GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_7; 
	GPIO_Initure.Mode=GPIO_MODE_AF_PP;          //推挽复用
	GPIO_Initure.Pull=GPIO_NOPULL;              //不带上下拉
	GPIO_Initure.Speed=GPIO_SPEED_HIGH;         //高速
	GPIO_Initure.Alternate=GPIO_AF11_ETH;       //复用为ETH功能
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);         //初始化

	GPIO_Initure.Pin=GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13;               //PB11
	HAL_GPIO_Init(GPIOB,&GPIO_Initure);         //始化
	
	GPIO_Initure.Pin=GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5; //PC1,4,5
	HAL_GPIO_Init(GPIOC,&GPIO_Initure);         //初始化

	HAL_NVIC_SetPriority(ETH_IRQn,1,0);         //网络中断优先级应该高一点
	HAL_NVIC_EnableIRQ(ETH_IRQn);
}
/* 读取PHY寄存器值 */
u32 LAN8720_ReadPHY(u16 reg)
{
	u32 regval;
	HAL_ETH_ReadPHYRegister(&ETH_Handler,reg, &regval);
	return regval;
}

/*
*********************************************************************************************************
*	函 数 名: LAN8720_WritePHY
*	功能说明: 向LAN8720指定寄存器写入值
*	形    参: reg:要写入的寄存器, value:要写入的值
*	返 回 值: 无
*********************************************************************************************************
*/
void LAN8720_WritePHY(u16 reg,u16 value)
{
	u32 temp=value;
	HAL_ETH_ReadPHYRegister(&ETH_Handler,reg,&temp);
}

/*
*********************************************************************************************************
*	函 数 名: LAN8720_Get_Speed
*	功能说明: 得到8720的速度模式
*	形    参: 无
*	返 回 值: 001:10M半双工，101:10M全双工，010:100M半双工，110:100M全双工，其他:错误.
*********************************************************************************************************
*/
u8 LAN8720_Get_Speed(void)
{
	u8 speed;
	speed=((LAN8720_ReadPHY(31)&0x1C)>>2); //从LAN8720的31号寄存器中读取网络速度和双工模式
	return speed;
}

extern void lwip_pkt_handle(void);		//在lwip_comm.c里面定义

/* 中断服务函数 */
void ETH_IRQHandler(void)
{
	while(ETH_GetRxPktSize(ETH_Handler.RxDesc))   
	{
			lwip_pkt_handle();//处理以太网数据，即将数据提交给LWIP
	}
	//清除中断标志位
	__HAL_ETH_DMA_CLEAR_IT(&ETH_Handler,ETH_DMA_IT_NIS);    //清除DMA中断标志位
	__HAL_ETH_DMA_CLEAR_IT(&ETH_Handler,ETH_DMA_IT_R);      //清除DMA接收中断标志位
}

/*
*********************************************************************************************************
*	函 数 名: ETH_GetRxPktSize
*	功能说明: 获取接收到的帧长度
*	形    参: DMARxDesc:接收DMA描述符
*	返 回 值: 接收到的帧长度
*********************************************************************************************************
*/
u32  ETH_GetRxPktSize(ETH_DMADescTypeDef *DMARxDesc)
{
	u32 frameLength = 0;
	if(((DMARxDesc->Status&ETH_DMARXDESC_OWN)==(uint32_t)RESET) &&
	 ((DMARxDesc->Status&ETH_DMARXDESC_ES)==(uint32_t)RESET) &&
	 ((DMARxDesc->Status&ETH_DMARXDESC_LS)!=(uint32_t)RESET)) 
	{
		frameLength=((DMARxDesc->Status&ETH_DMARXDESC_FL)>>ETH_DMARXDESC_FRAME_LENGTHSHIFT);
	}
	return frameLength;
}

/* 以外网底层驱动内存申请 */
u8 ETH_Mem_Malloc(void)
{ 
	DMARxDscrTab=mymalloc(SRAMIN,ETH_RXBUFNB*sizeof(ETH_DMADescTypeDef));//申请内存
	DMATxDscrTab=mymalloc(SRAMIN,ETH_TXBUFNB*sizeof(ETH_DMADescTypeDef));//申请内存  
	Rx_Buff=mymalloc(SRAMIN,ETH_RX_BUF_SIZE*ETH_RXBUFNB);	//申请内存
	Tx_Buff=mymalloc(SRAMIN,ETH_TX_BUF_SIZE*ETH_TXBUFNB);	//申请内存
	if(!(u32)&DMARxDscrTab||!(u32)&DMATxDscrTab||!(u32)&Rx_Buff||!(u32)&Tx_Buff)
	{
		ETH_Mem_Free();
		return 1;	//申请失败
	}	
	return 0;		//申请成功
}

/* 释放ETH 底层驱动申请的内存 */
void ETH_Mem_Free(void)
{ 
	myfree(SRAMIN,DMARxDscrTab);//释放内存
	myfree(SRAMIN,DMATxDscrTab);//释放内存
	myfree(SRAMIN,Rx_Buff);		//释放内存
	myfree(SRAMIN,Tx_Buff);		//释放内存  
}



