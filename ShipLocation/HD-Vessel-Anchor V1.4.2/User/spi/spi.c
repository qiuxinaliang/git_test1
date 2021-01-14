/*
 * @:*********************************************************************************************************: 
 * @Date: 2020-11-16 10:05:33
 * @LastEditTime: 2020-12-07 16:29:45
 * @**********************************************************************************************************: 
 */
#include "spi.h"

/*
*******************************************************************************
*	函 数 名: Lora_SPI_Init
*	功能说明: Lora的SPI通信硬件初始化
*	形	  参: 无
*	返 回 值: 无
*******************************************************************************
*/
SPI_HandleTypeDef hspi4;
void Lora_SPI_Init(void)
{
	hspi4.Instance = Lora_SPI;
	hspi4.Init.Mode = SPI_MODE_MASTER;
	hspi4.Init.Direction = SPI_DIRECTION_2LINES;
	hspi4.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi4.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi4.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi4.Init.NSS = SPI_NSS_SOFT;
	hspi4.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
	hspi4.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi4.Init.TIMode = SPI_TIMODE_DISABLED;
	hspi4.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
	hspi4.Init.CRCPolynomial = 7;
	HAL_SPI_Init(&hspi4);

	//HAL_Delay(100);
}

/*
*******************************************************************************
*	函 数 名:  DW_SPI_Init_Fast
*	功能说明: DW1000的SPI通信硬件初始化,与DW_SPI_Init对比只有速率预分频（BaudRatePrescaler）存在差异
*	形	  参: 无
*	返 回 值: 无
*******************************************************************************
*/
void Lora_SPI_Init_Fast(void)
{
	hspi4.Instance = Lora_SPI;
	hspi4.Init.Mode = SPI_MODE_MASTER;
	hspi4.Init.Direction = SPI_DIRECTION_2LINES;
	hspi4.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi4.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi4.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi4.Init.NSS = SPI_NSS_SOFT;
	hspi4.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
	hspi4.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi4.Init.TIMode = SPI_TIMODE_DISABLED;
	hspi4.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
	hspi4.Init.CRCPolynomial = 7;
	HAL_SPI_Init(&hspi4);
	
	HAL_Delay(100);
}

/* 系统SPI硬件配置 */
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	if(hspi->Instance == Lora_SPI)
	{
		Lora_SPI_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();
    Lora_SPI_CS_CLK_ENABLE();

		/**SPI1 GPIO Configuration
		PE12     ------> SPI4_SCK
		PE13     ------> SPI4_MISO
		PE14     ------> SPI4_MOSI
		*/
		//RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO, ENABLE );
    //GPIO_PinRemapConfig( GPIO_Remap_SWJ_JTAGDisable, ENABLE );
		
		GPIO_InitStruct.Pin = Lora_SPI_SCK_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF5_SPI4;
		HAL_GPIO_Init(Lora_SPI_SCK_PORT, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = Lora_SPI_MISO_PIN;
		HAL_GPIO_Init(Lora_SPI_MISO_PORT, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = Lora_SPI_MOSI_PIN;
		HAL_GPIO_Init(Lora_SPI_MOSI_PORT, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = Lora_SPI_CS_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(Lora_SPI_CS_PORT, &GPIO_InitStruct);
	}
}

/* 失能SPI */
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{

	if(hspi->Instance == Lora_SPI)
	{
		Lora_SPI_CLK_DISABLE();
		HAL_GPIO_DeInit(Lora_SPI_SCK_PORT, Lora_SPI_SCK_PIN);
		HAL_GPIO_DeInit(Lora_SPI_MISO_PORT, Lora_SPI_MISO_PIN);
		HAL_GPIO_DeInit(Lora_SPI_MOSI_PORT, Lora_SPI_MOSI_PIN);
	}
}