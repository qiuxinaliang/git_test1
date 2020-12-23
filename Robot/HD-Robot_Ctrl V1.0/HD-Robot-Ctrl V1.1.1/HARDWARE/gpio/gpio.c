/*
 * @:*********************************************************************************************************: 
 * @Date: 2020-09-30 10:11:06
 * @LastEditTime: 2020-09-30 13:52:22
 * @**********************************************************************************************************: 
 */
#include "gpio.h"

/*
*******************************************************************************
*	函 数 名: DW_EXIT_GPIO_NVIC_Config
*	功能说明: DW1000的外部中断引脚配置，并打开外部中断
*	形	  参: 无
*	返 回 值: 无
*******************************************************************************
*/
void GPIO_Init_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	__HAL_RCC_LED_CLK_ENABLE();

	// LED 引脚配置
	GPIO_InitStruct.Pin = LED_GPIO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LED_GPIO_PORT, &GPIO_InitStruct);
}

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
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;  //推挽输出
	GPIO_Initure.Pull = GPIO_PULLUP;          //上拉
	GPIO_Initure.Speed = GPIO_SPEED_LOW; //     //高速
	HAL_GPIO_Init(LED_GPIO_PORT,&GPIO_Initure);
}




