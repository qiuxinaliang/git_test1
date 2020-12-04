#ifndef __LED_H
#define	__LED_H

#include "common.h"
#define __HAL_RCC_LED_CLK_ENABLE() __HAL_RCC_GPIOE_CLK_ENABLE()
#define LED_GPIO_PIN GPIO_PIN_3
#define LED_GPIO_PORT GPIOE

void LED_GPIO_Config(void);

#endif /* __LED_H */
