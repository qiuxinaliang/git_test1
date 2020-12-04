/*
 * @:*********************************************************************************************************: 
 * @Date: 2020-11-16 10:05:53
 * @LastEditTime: 2020-11-16 10:21:16
 * @**********************************************************************************************************: 
 */
#ifndef __SPI__H__
#define __SPI__H__

#include "common.h"

#define Lora_SPI SPI1
#define Lora_SPI_CLK_ENABLE()  __SPI1_CLK_ENABLE()
#define Lora_SPI_CLK_DISABLE() __SPI1_CLK_DISABLE()
#define Lora_HAL_RCC_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()

#define Lora_SPI_SCK_PORT GPIOB
#define Lora_SPI_SCK_PIN GPIO_PIN_3

#define Lora_SPI_MISO_PORT GPIOB
#define Lora_SPI_MISO_PIN GPIO_PIN_4

#define Lora_SPI_MOSI_PORT GPIOB
#define Lora_SPI_MOSI_PIN GPIO_PIN_5

#define Lora_SPI_CS_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define Lora_SPI1_CS_PORT GPIOA
#define Lora_SPI1_CS_PIN	GPIO_PIN_4

extern SPI_HandleTypeDef hspi1;
void Lora_SPI_Init(void);

#endif  //!__SPI__H_
