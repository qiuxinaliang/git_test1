#ifndef __spi_H
#define __spi_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "common.h"

#define DW_SPI SPI4
#define DW_SPI_CLK_ENABLE()  __SPI4_CLK_ENABLE()
#define DW_SPI_CLK_DISABLE() __SPI4_CLK_DISABLE()
#define DW_HAL_RCC_CLK_ENABLE() __HAL_RCC_GPIOE_CLK_ENABLE()

#define DW_SPI_SCK_PORT GPIOE
#define DW_SPI_SCK_PIN GPIO_PIN_12

#define DW_SPI_MISO_PORT GPIOE
#define DW_SPI_MISO_PIN GPIO_PIN_13

#define DW_SPI_MOSI_PORT GPIOE
#define DW_SPI_MOSI_PIN GPIO_PIN_14

#define DW_SPI4_CS_PORT GPIOE
#define DW_SPI4_CS_PIN	GPIO_PIN_11

///////////////////////////////////////////////////////////////
/* DW1000 中断引脚配置 */
#define DW_IRQn EXTI9_5_IRQn
#define DW_IRQ_PIN GPIO_PIN_9
#define DW_IRQ_PORT GPIOE



/* 复位引脚配置 */
#define DW_RST_PIN GPIO_PIN_10
#define DE_RST_PORT GPIOE

extern SPI_HandleTypeDef hspi4;
extern void _Error_Handler(char *, int);

void DW_SPI_Init(void);
void DW_SPI_Init_Fast(void);

#ifdef __cplusplus
}
#endif

#endif 


