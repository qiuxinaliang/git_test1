/*
 * @:*********************************************************************************************************: 
 * @Date: 2020-11-03 18:07:18
 * @LastEditTime: 2020-11-04 11:11:33
 * @**********************************************************************************************************: 
 */
#include "./eth/bsp_eth.h"

ETH_HandleTypeDef heth;   /* Global Ethernet handle */

#if defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4   
#endif
__ALIGN_BEGIN ETH_DMADescTypeDef  DMARxDscrTab[ETH_RXBUFNB] __ALIGN_END;/* Ethernet Rx MA Descriptor */

#if defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4   
#endif
__ALIGN_BEGIN ETH_DMADescTypeDef  DMATxDscrTab[ETH_TXBUFNB] __ALIGN_END;/* Ethernet Tx DMA Descriptor */

#if defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4   
#endif
__ALIGN_BEGIN uint8_t Rx_Buff[ETH_RXBUFNB][ETH_RX_BUF_SIZE] __ALIGN_END; /* Ethernet Receive Buffer */

#if defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4   
#endif
__ALIGN_BEGIN uint8_t Tx_Buff[ETH_TXBUFNB][ETH_TX_BUF_SIZE] __ALIGN_END; /* Ethernet Transmit Buffer */

/*
*********************************************************************************************************
*	函 数 名:HAL_ETH_MspInit
*	功能说明: 以太网硬件初始化
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void HAL_ETH_MspInit(ETH_HandleTypeDef* ethHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  if(ethHandle->Instance == ETH)
  {
    __HAL_RCC_ETH_CLK_ENABLE();
    GPIO_InitStruct.Pin = ETH_MDC_Pin | ETH_RXD0_Pin | ETH_RXD1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = ETH_REF_CLK_Pin | ETH_MDIO_Pin | ETH_CRS_DV_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = ETH_TX_EN_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(ETH_TX_EN_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = ETH_TXD0_Pin | ETH_TXD1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(ETH_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(ETH_IRQn);
    
    /* Enable ETHERNET clock  */
    __HAL_RCC_ETH_CLK_ENABLE();
    /* USER CODE END ETH_MspInit 1 */
  }
}

// 硬件复位
static void Eth_Reset(void)
{ 
   /* PHY RESET: PI1 */
  GPIO_InitTypeDef GPIO_InitStructure;
  __HAL_RCC_GPIOA_CLK_ENABLE();
  
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Pull  = GPIO_PULLUP;
  GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
  GPIO_InitStructure.Pin = GPIO_PIN_3;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
  HAL_Delay(50);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
  HAL_Delay(50);
}

void HAL_ETH_MspDeInit(ETH_HandleTypeDef* ethHandle)
{
  if(ethHandle->Instance==ETH)
  {
    __HAL_RCC_ETH_CLK_DISABLE();
    HAL_GPIO_DeInit(GPIOC, ETH_MDC_Pin|ETH_RXD0_Pin|ETH_RXD1_Pin);
    HAL_GPIO_DeInit(GPIOA, ETH_REF_CLK_Pin|ETH_MDIO_Pin|ETH_CRS_DV_Pin);
    HAL_GPIO_DeInit(ETH_TX_EN_GPIO_Port, ETH_TX_EN_Pin);
    HAL_GPIO_DeInit(GPIOB, ETH_TXD0_Pin|ETH_TXD1_Pin);
  }
}

HAL_StatusTypeDef Bsp_Eth_Init(void)
{
  HAL_StatusTypeDef ret;
  uint8_t MACAddr[6] ;
  uint32_t sn0;
	sn0=*(uint32_t*)(0x1FFF7A10);//获取STM32的唯一ID的前24位作为MAC地址后三字节

  HAL_ETH_DeInit(&heth);
  Eth_Reset();    //硬件复位
  ETH->DMABMR |= ETH_DMABMR_SR;
  
  MACAddr[0] = 0x02;
  MACAddr[1] = 0x00;
  MACAddr[2] = 0x00;
  MACAddr[3] = (sn0>>16)&0XFF;//低三字节用STM32的唯一ID
  MACAddr[4] = (sn0>>8)&0XFFF;
  MACAddr[5] = sn0&0XFF;
  heth.Instance = ETH;
  heth.Init.AutoNegotiation = ETH_AUTONEGOTIATION_ENABLE;
  heth.Init.PhyAddress = LAN8720_PHY_ADDRESS;
  heth.Init.MACAddr = &MACAddr[0];
  heth.Init.RxMode = ETH_RXINTERRUPT_MODE;  // rx mode   
  heth.Init.ChecksumMode = ETH_CHECKSUM_BY_HARDWARE;
  heth.Init.MediaInterface = ETH_MEDIA_INTERFACE_RMII;
  heth.Init.Speed = ETH_SPEED_100M;       //speed
  heth.Init.DuplexMode = ETH_MODE_FULLDUPLEX;
  
	/* configure ethernet peripheral (GPIOs, clocks, MAC, DMA) */
  ret = HAL_ETH_Init(&heth);
	if (ret == HAL_OK)
		PRINT_DEBUG("eth hardware init sucess...\n");
  else
    PRINT_DEBUG("eth hardware init faild...\n");
    
  /* Initialize Tx Descriptors list: Chain Mode */
  HAL_ETH_DMATxDescListInit(&heth, DMATxDscrTab, &Tx_Buff[0][0], ETH_TXBUFNB);
     
  /* Initialize Rx Descriptors list: Chain Mode  */
  HAL_ETH_DMARxDescListInit(&heth, DMARxDscrTab, &Rx_Buff[0][0], ETH_RXBUFNB);
    /* Enable MAC and DMA transmission and reception */    
  return ret;
}

// 以太网中断处理
void ETH_IRQHandler(void)
{
  uint32_t ulReturn;
  ulReturn = taskENTER_CRITICAL_FROM_ISR();   /* 进入临界段，临界段可以嵌套 */
  HAL_ETH_IRQHandler(&heth);
  taskEXIT_CRITICAL_FROM_ISR( ulReturn );     /* 退出临界段 */
}

// 以太网接收中断回调处理
extern xSemaphoreHandle s_xSemaphore;
void HAL_ETH_RxCpltCallback(ETH_HandleTypeDef *heth)
{
  portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
  xSemaphoreGiveFromISR( s_xSemaphore, &xHigherPriorityTaskWoken );
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

// 以太网发送中断回调初六
void HAL_ETH_TxCpltCallback(ETH_HandleTypeDef *heth)
{
  ;
}

void HAL_ETH_ErrorCallback(ETH_HandleTypeDef *heth)
{
  PRINT_ERR("eth err\n");
}
