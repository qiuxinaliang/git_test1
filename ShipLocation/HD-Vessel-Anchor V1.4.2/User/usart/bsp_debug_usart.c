/*
 * @:*********************************************************************************************************: 
 * @Date: 2020-11-03 18:07:18
 * @LastEditTime: 2020-11-12 11:47:27
 * @**********************************************************************************************************: 
 */

#include "./usart/bsp_debug_usart.h"
#include "common.h"

UART_HandleTypeDef UartHandle;
UART_HandleTypeDef husart3;
extern uint8_t ucTemp; 
extern Queue USART3_For_Lora_queue; 

/*
*******************************************************************************
*	函 数 名: DEBUG_USART_Config
*	功能说明: 串口通信初始化函数
*	形	  参: 无
*	返 回 值: 无
*******************************************************************************
*/
void DEBUG_USART_Config(void)
{ 
  UartHandle.Instance = DEBUG_USART;
  UartHandle.Init.BaudRate = DEBUG_USART_BAUDRATE;
  UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits = UART_STOPBITS_1;
  UartHandle.Init.Parity = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode = UART_MODE_TX_RX;
  HAL_UART_Init(&UartHandle); 
}

/*
*******************************************************************************
*	函 数 名: USART3_Lora_Config
*	功能说明: Lora通信串口通信初始化函数
*	形	  参: 无
*	返 回 值: 无
*******************************************************************************
*/
void USART3_Lora_Config(void)
{ 
  husart3.Instance = Lora_USART;
  husart3.Init.BaudRate = Lora_USART_BAUDRATE;
  husart3.Init.WordLength = UART_WORDLENGTH_8B;
  husart3.Init.StopBits = UART_STOPBITS_1;
  husart3.Init.Parity = UART_PARITY_NONE;
  husart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  husart3.Init.Mode = UART_MODE_TX_RX;
  HAL_UART_Init(&husart3); 
  __HAL_UART_ENABLE_IT(&husart3,UART_IT_RXNE);
}

/*
*******************************************************************************
*	函 数 名: HAL_UART_MspInit
*	功能说明: UART MSP 初始化
*	形	  参: huart: UART handle
*	返 回 值: 无
*******************************************************************************
*/
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{  
  GPIO_InitTypeDef  GPIO_InitStruct;

  DEBUG_USART_CLK_ENABLE();
	DEBUG_USART_RX_GPIO_CLK_ENABLE();
  DEBUG_USART_TX_GPIO_CLK_ENABLE();
 
  GPIO_InitStruct.Pin = DEBUG_USART_TX_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = DEBUG_USART_TX_AF;
  HAL_GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin = DEBUG_USART_RX_PIN;
  GPIO_InitStruct.Alternate = DEBUG_USART_RX_AF;
  HAL_GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStruct); 

  //HAL_NVIC_SetPriority(DEBUG_USART_IRQ ,0,1);	//抢占优先级0，子优先级1
  //HAL_NVIC_EnableIRQ(DEBUG_USART_IRQ );		    //使能USART1中断通道 

  Lora_USART_CLK_ENABLE();
	Lora_USART_RX_GPIO_CLK_ENABLE();
  Lora_USART_TX_GPIO_CLK_ENABLE();

  GPIO_InitStruct.Pin = Lora_USART_TX_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = Lora_USART_TX_AF;
  HAL_GPIO_Init(Lora_USART_TX_GPIO_PORT, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin = Lora_USART_RX_PIN;
  GPIO_InitStruct.Alternate = Lora_USART_RX_AF;
  HAL_GPIO_Init(Lora_USART_RX_GPIO_PORT, &GPIO_InitStruct);

  HAL_NVIC_SetPriority(Lora_USART_IRQ ,0,1);	//抢占优先级0，子优先级1
  HAL_NVIC_EnableIRQ(Lora_USART_IRQ );		    //使能USART1中断通道 
}

void DEBUG_USART_IRQHandler(void)
{
  uint8_t  a=0;
  uint8_t* ch=&a; 
  
	if(__HAL_UART_GET_FLAG( &UartHandle, UART_FLAG_RXNE ) != RESET)
	{		
    *ch=( uint16_t)READ_REG(UartHandle.Instance->DR);
		HAL_UART_Transmit( &UartHandle,ch ,1,6 );     
	}
  HAL_UART_IRQHandler(&UartHandle);	
}

void Lora_USART_IRQHandler(void)
{
  uint8_t  temp = 0;
  uint32_t ulReturn;
  /* 进入临界段，临界段可以嵌套 */
  ulReturn = taskENTER_CRITICAL_FROM_ISR();
 
	if(__HAL_UART_GET_FLAG( &husart3, UART_FLAG_RXNE ) != RESET)
	{		
    temp = (uint16_t)READ_REG(husart3.Instance->DR);
    en_queue(&USART3_For_Lora_queue, (Item)temp);
		HAL_UART_Transmit( &husart3, &temp,1,6 );     
	}
  HAL_UART_IRQHandler(&husart3);	 
  taskEXIT_CRITICAL_FROM_ISR( ulReturn );
}

/**
 * @:*********************************************************************************************************: 
 * @函 数 名: USART1_For_Debug_tx
 * @功能说明: 调试信息数据输出
 * @形    参: data_length:输出的数据长度， data：待输出的数据指针
 * @返 回 值: data_length:返回数据长度
 * @**********************************************************************************************************: 
 */
uint32_t USART1_For_Debug_tx(uint32_t data_length, const uint8_t* data)
{
    /** The amount of bytes still needed to be written. */
    uint32_t data_to_write = data_length;
    uint8_t *ptr = (uint8_t *)data;

    while(data_to_write--) {
        //USART_SendData(USART1, *(uint8_t *)ptr);
        HAL_UART_Transmit(&UartHandle, (uint8_t *)ptr, 1, 0xffff);
        /* Loop until the end of transmission */
        if(data_to_write) {
            ptr++;
        }
    }
    ptr = NULL;
    return data_length;
}

/**
 * @:*********************************************************************************************************: 
 * @函 数 名: USART3_For_Lora_rx
 * @功能说明: Lora模组数据接收函数
 * @形    参: data_length:输入的数据缓存长度， data：待输入的数据指针
 * @返 回 值: ret > 0 :接收到有效数据， ret = 0： 无效接收
 * @**********************************************************************************************************: 
 */
uint32_t USART3_For_Lora_rx(uint32_t data_length, uint8_t* data)
{
    /** Variable for storing function return values. */
    uint32_t ret = 0;
    /** The amount of bytes still needed to be read. */
    uint32_t data_to_read = data_length;

    while(data_to_read) {
        if(!de_queue(&USART3_For_Lora_queue, data)) {
            return ret;
        }
        data_to_read--;
        data++;
        ret++;
    }
    return ret;
}

/**
 * @:*********************************************************************************************************: 
 * @函 数 名: USART3_For_Lora_tx
 * @功能说明: Lora数据输出
 * @形    参: data_length:输出的数据长度， data：待输出的数据指针
 * @返 回 值: data_length:返回数据长度
 * @**********************************************************************************************************: 
 */
uint32_t USART3_For_Lora_tx(uint32_t data_length, const uint8_t* data)
{
    /** The amount of bytes still needed to be written. */
    uint32_t data_to_write = data_length;
    uint8_t *ptr = (uint8_t *)data;

    while(data_to_write--) {
        //USART_SendData(USART1, *(uint8_t *)ptr);
        HAL_UART_Transmit(&husart3, (uint8_t *)ptr, 1, 0xffff);
        /* Loop until the end of transmission */
        if(data_to_write) {
            ptr++;
        }
    }
    ptr = NULL;
    return data_length;
}

/*****************  发送字符串 **********************/
void Usart_SendString(uint8_t *str)
{
	unsigned int k=0;
  do 
  {
    HAL_UART_Transmit(&UartHandle,(uint8_t *)(str + k) ,1,1000);
    k++;
  } while(*(str + k)!='\0');
  
}
// 重定向c库函数printf到串口DEBUG_USART，重定向后可使用printf函数
int fputc(int ch, FILE *f)
{
	HAL_UART_Transmit(&UartHandle, (uint8_t *)&ch, 1, 1000);	/* 发送一个字节数据到串口DEBUG_USART */
	return (ch);
}

// 重定向c库函数scanf到串口DEBUG_USART，重写向后可使用scanf、getchar等函数
int fgetc(FILE *f)
{
	int ch;
	HAL_UART_Receive(&UartHandle, (uint8_t *)&ch, 1, 1000);	
	return (ch);
}
/*********************************************END OF FILE**********************/
