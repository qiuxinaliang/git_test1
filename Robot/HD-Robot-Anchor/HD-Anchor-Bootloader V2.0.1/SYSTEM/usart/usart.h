#ifndef __usart_H
#define __usart_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "stdio.h"

#define Debug_USART_TX_Pin 			GPIO_PIN_6
#define Debug_USART_TX_GPIO_Port    GPIOB
#define Debug_USART_RX_Pin          GPIO_PIN_7
#define Debug_USART_RX_GPIO_Port    GPIOB
#define Debug_USART_IRQn            USART1_IRQn
#define Debug_USART_GPIO_AF         GPIO_AF7_USART1

#define USART2_TX_Pin GPIO_PIN_2
#define USART2_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA

extern UART_HandleTypeDef huart1;

extern void _Error_Handler(char *, int);
void Debug_USART1_Init(void);
HAL_StatusTypeDef USART1_For_Bytes_tx(uint32_t data_length, uint8_t* data);
#ifdef __cplusplus
}
#endif
#endif 


