/*
 * @:*********************************************************************************************************: 
 * @Date: 2020-11-03 18:07:18
 * @LastEditTime: 2020-11-04 11:11:18
 * @**********************************************************************************************************: 
 */
#ifndef __BSP_ETH_H__
#define __BSP_ETH_H__

#include "common.h"
/**ETH GPIO Configuration    
PC1     ------> ETH_MDC
PA1     ------> ETH_REF_CLK
PA2     ------> ETH_MDIO
PA7     ------> ETH_CRS_DV
PC4     ------> ETH_RXD0
PC5     ------> ETH_RXD1
PB11     ------> ETH_TX_EN
PB12     ------> ETH_TXD0
PB13     ------> ETH_TXD1 
*/

#define ETH_MDC_Pin                 GPIO_PIN_1
#define ETH_MDC_GPIO_Port           GPIOC
#define ETH_REF_CLK_Pin             GPIO_PIN_1
#define ETH_REF_CLK_GPIO_Port       GPIOA
#define ETH_MDIO_Pin                GPIO_PIN_2
#define ETH_MDIO_GPIO_Port          GPIOA
#define ETH_CRS_DV_Pin              GPIO_PIN_7
#define ETH_CRS_DV_GPIO_Port        GPIOA
#define ETH_RXD0_Pin                GPIO_PIN_4
#define ETH_RXD0_GPIO_Port          GPIOC
#define ETH_RXD1_Pin                GPIO_PIN_5
#define ETH_RXD1_GPIO_Port          GPIOC
#define ETH_TX_EN_Pin               GPIO_PIN_11
#define ETH_TX_EN_GPIO_Port         GPIOB
#define USART1_TX_Pin               GPIO_PIN_6
#define USART1_TX_GPIO_Port         GPIOB
#define USART1_TXA10_Pin            GPIO_PIN_7
#define USART1_TXA10_GPIO_Port      GPIOB
#define ETH_TXD0_Pin                GPIO_PIN_12
#define ETH_TXD0_GPIO_Port          GPIOB
#define ETH_TXD1_Pin                GPIO_PIN_13
#define ETH_TXD1_GPIO_Port          GPIOB

HAL_StatusTypeDef Bsp_Eth_Init(void);

#endif
