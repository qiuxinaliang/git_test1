/*
 * @:*********************************************************************************************************: 
 * @Date: 2020-09-30 10:11:06
 * @LastEditTime: 2020-09-30 13:49:51
 * @**********************************************************************************************************: 
 */
#ifndef __gpio_H
#define __gpio_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "common.h"

/* LED的PIN初始初始化配置 */
#define __HAL_RCC_LED_CLK_ENABLE() __HAL_RCC_GPIOE_CLK_ENABLE()
#define LED_GPIO_PIN GPIO_PIN_3
#define LED_GPIO_PORT GPIOE

void LED_Status_Init(void);

#ifdef __cplusplus
}
#endif
#endif 

