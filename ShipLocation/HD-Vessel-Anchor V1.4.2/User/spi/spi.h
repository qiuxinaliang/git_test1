/*
 * @:*********************************************************************************************************: 
 * @Date: 2020-11-16 10:05:53
 * @LastEditTime: 2020-12-07 16:26:41
 * @**********************************************************************************************************: 
 */
#ifndef __SPI__H__
#define __SPI__H__

#include "common.h"

#define Lora_SPI SPI4
#define Lora_SPI_CLK_ENABLE()  __SPI4_CLK_ENABLE()
#define Lora_SPI_CLK_DISABLE() __SPI4_CLK_DISABLE()
#define Lora_HAL_RCC_CLK_ENABLE() __HAL_RCC_GPIOE_CLK_ENABLE()

#define Lora_SPI_SCK_PORT GPIOE
#define Lora_SPI_SCK_PIN GPIO_PIN_12

#define Lora_SPI_MISO_PORT GPIOE
#define Lora_SPI_MISO_PIN GPIO_PIN_13

#define Lora_SPI_MOSI_PORT GPIOE
#define Lora_SPI_MOSI_PIN GPIO_PIN_14

#define Lora_SPI_CS_CLK_ENABLE() __HAL_RCC_GPIOE_CLK_ENABLE()
#define Lora_SPI_CS_PORT GPIOE
#define Lora_SPI_CS_PIN	GPIO_PIN_11

extern SPI_HandleTypeDef hspi1;
void Lora_SPI_Init(void);

#endif  //!__SPI__H_
