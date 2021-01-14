/*
 * @:*********************************************************************************************************: 
 * @Date: 2020-09-30 10:11:06
 * @LastEditTime: 2020-09-30 13:52:22
 * @**********************************************************************************************************: 
 */
#include "gpio.h"

/*
*******************************************************************************
*	函 数 名: LED_Status_Init
*	功能说明: LED的PIN初始初始化配置
*	形	  参: 无
*	返 回 值: 无
*******************************************************************************
*/
void LED_Status_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;

	__HAL_RCC_LED_CLK_ENABLE();			//开启GPIOA时钟
	GPIO_Initure.Pin = LED_GPIO_PIN; //PB1,0
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;  
	GPIO_Initure.Pull = GPIO_PULLUP;          
	GPIO_Initure.Speed = GPIO_SPEED_LOW; 
	HAL_GPIO_Init(LED_GPIO_PORT,&GPIO_Initure);
}




