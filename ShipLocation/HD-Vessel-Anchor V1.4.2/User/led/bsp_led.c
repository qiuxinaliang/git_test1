/*
 * @:*********************************************************************************************************: 
 * @Date: 2020-11-16 10:00:13
 * @LastEditTime: 2020-11-16 12:40:43
 * @**********************************************************************************************************: 
 */
#include "./led/bsp_led.h"   

/*
*********************************************************************************************************
*	函 数 名: LED_GPIO_Config
*	功能说明: LED引脚初始化
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/	
void LED_GPIO_Config(void)
{		
	GPIO_InitTypeDef  GPIO_InitStruct;

	__HAL_RCC_LED_CLK_ENABLE();

	GPIO_InitStruct.Pin = LED_GPIO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LED_GPIO_PORT, &GPIO_InitStruct);
}
/*********************************************END OF FILE**********************/

