/*
 * @:*********************************************************************************************************: 
 * @Date: 2020-11-03 18:07:18
 * @LastEditTime: 2020-11-12 10:26:20
 * @**********************************************************************************************************: 
 */
#ifndef __DEBUG_USART_H
#define	__DEBUG_USART_H

//#include "common.h"
#include "stm32f4xx.h"
#include <stdio.h>
#include "iacs_queue.h"

//串口波特率
#define DEBUG_USART_BAUDRATE                    115200
#define Lora_USART_BAUDRATE                    115200

//引脚定义
/*******************************************************/
#define DEBUG_USART                             USART1
#define DEBUG_USART_CLK_ENABLE()                __USART1_CLK_ENABLE();

#define RCC_PERIPHCLK_UARTx                     RCC_PERIPHCLK_USART1
#define RCC_UARTxCLKSOURCE_SYSCLK               RCC_USART1CLKSOURCE_SYSCLK

#define DEBUG_USART_RX_GPIO_PORT                GPIOB
#define DEBUG_USART_RX_GPIO_CLK_ENABLE()        __GPIOB_CLK_ENABLE()
#define DEBUG_USART_RX_PIN                      GPIO_PIN_7
#define DEBUG_USART_RX_AF                       GPIO_AF7_USART1

#define DEBUG_USART_TX_GPIO_PORT                GPIOB
#define DEBUG_USART_TX_GPIO_CLK_ENABLE()        __GPIOB_CLK_ENABLE()
#define DEBUG_USART_TX_PIN                      GPIO_PIN_6
#define DEBUG_USART_TX_AF                       GPIO_AF7_USART1

#define DEBUG_USART_IRQHandler                  USART1_IRQHandler
#define DEBUG_USART_IRQ                 		    USART1_IRQn

#define Lora_USART                             USART3
#define Lora_USART_CLK_ENABLE()                __USART3_CLK_ENABLE();

#define Lora_USART_RX_GPIO_PORT                GPIOD
#define Lora_USART_RX_GPIO_CLK_ENABLE()        __GPIOD_CLK_ENABLE()
#define Lora_USART_RX_PIN                      GPIO_PIN_9
#define Lora_USART_RX_AF                       GPIO_AF7_USART3

#define Lora_USART_TX_GPIO_PORT                GPIOD
#define Lora_USART_TX_GPIO_CLK_ENABLE()        __GPIOD_CLK_ENABLE()
#define Lora_USART_TX_PIN                      GPIO_PIN_8
#define Lora_USART_TX_AF                       GPIO_AF7_USART3

#define Lora_USART_IRQHandler                  USART3_IRQHandler
#define Lora_USART_IRQ                 		     USART3_IRQn

extern UART_HandleTypeDef UartHandle;


/************************************************************/
void Usart_SendString(uint8_t *str);
void DEBUG_USART_Config(void);
void USART3_Lora_Config(void);
//int fputc(int ch, FILE *f);
void Lora_USART_IRQHandler(void);
uint32_t USART1_For_Debug_tx(uint32_t data_length, const uint8_t* data);
uint32_t USART3_For_Lora_rx(uint32_t data_length, uint8_t* data);
uint32_t USART3_For_Lora_tx(uint32_t data_length, const uint8_t* data);
#endif /* __USART1_H */
