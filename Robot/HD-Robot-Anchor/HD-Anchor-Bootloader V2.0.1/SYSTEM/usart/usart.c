#include "usart.h"


/*
*******************************************************************************
*	函 数 名:  Debug_USART1_Init
*	功能说明: 串口通信初始化函数
*	形	  参: 无
*	返 回 值: 无
*******************************************************************************
*/
UART_HandleTypeDef huart1;
void Debug_USART1_Init(void)
{
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart1) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}
}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

	GPIO_InitTypeDef GPIO_InitStruct;
	
	if(uartHandle->Instance==USART1)
	{
		__HAL_RCC_USART1_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();
		/**USART1 GPIO Configuration    
		PB6     ------> USART1_TX
		PB7     ------> USART7_RX 
		*/
		GPIO_InitStruct.Pin = Debug_USART_TX_Pin |Debug_USART_RX_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStruct.Alternate = Debug_USART_GPIO_AF;
		HAL_GPIO_Init(Debug_USART_RX_GPIO_Port, &GPIO_InitStruct);

		HAL_NVIC_SetPriority(Debug_USART_IRQn , 0, 0);
		HAL_NVIC_EnableIRQ(Debug_USART_IRQn );
	}
	
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{
	if(uartHandle->Instance==USART1)
	{
	    __HAL_RCC_USART1_CLK_DISABLE();

		/**USART1 GPIO Configuration    
		PB6     ------> USART1_TX
		PB7     ------> USART7_RX 
		*/
		HAL_GPIO_DeInit(Debug_USART_RX_GPIO_Port, Debug_USART_TX_Pin |Debug_USART_RX_Pin);
		HAL_NVIC_DisableIRQ(Debug_USART_IRQn);

	}

} 

#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
PUTCHAR_PROTOTYPE
{
	size_t count =1;
	uint8_t chTemp = (uint8_t)ch;
	HAL_UART_Transmit(&huart1, &chTemp, count, HAL_MAX_DELAY);
	return count;
}

/*
*********************************************************************************************************
*	函 数 名: USART1_For_Bytes_tx
*	功能说明: USART1输出函数
*	形    参: data_length字符串长度；data字符串内容
*	返 回 值: 无
*********************************************************************************************************
*/
HAL_StatusTypeDef USART1_For_Bytes_tx(uint32_t data_length, uint8_t* data)
{
    /** The amount of bytes still needed to be written. */
	HAL_StatusTypeDef ret = HAL_OK;
    uint32_t data_to_write = data_length;
    uint8_t *ptr = (uint8_t *)data;
	ret = HAL_UART_Transmit(&huart1, ptr, data_length, HAL_MAX_DELAY);
	if(ret != HAL_OK)
	{
		return HAL_ERROR;
	}
    return HAL_OK;
}


