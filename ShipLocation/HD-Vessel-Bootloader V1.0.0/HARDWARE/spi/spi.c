#include "spi.h"

/*
*******************************************************************************
*	函 数 名: DW_SPI_Init
*	功能说明: DW1000的SPI通信硬件初始化
*	形	  参: 无
*	返 回 值: 无
*******************************************************************************
*/
SPI_HandleTypeDef hspi4;
void DW_SPI_Init(void)
{
	hspi4.Instance = DW_SPI;
	hspi4.Init.Mode = SPI_MODE_MASTER;
	hspi4.Init.Direction = SPI_DIRECTION_2LINES;
	hspi4.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi4.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi4.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi4.Init.NSS = SPI_NSS_SOFT;
	hspi4.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
	hspi4.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi4.Init.TIMode = SPI_TIMODE_DISABLED;
	hspi4.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
	hspi4.Init.CRCPolynomial = 10;
	HAL_SPI_Init(&hspi4);

	HAL_Delay(100);
}

/*
*******************************************************************************
*	函 数 名:  DW_SPI_Init_Fast
*	功能说明: DW1000的SPI通信硬件初始化,与DW_SPI_Init对比只有速率预分频（BaudRatePrescaler）存在差异
*	形	  参: 无
*	返 回 值: 无
*******************************************************************************
*/
void DW_SPI_Init_Fast(void)
{
	hspi4.Instance = DW_SPI;
	hspi4.Init.Mode = SPI_MODE_MASTER;
	hspi4.Init.Direction = SPI_DIRECTION_2LINES;
	hspi4.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi4.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi4.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi4.Init.NSS = SPI_NSS_SOFT;
	hspi4.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
	hspi4.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi4.Init.TIMode = SPI_TIMODE_DISABLED;
	hspi4.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
	hspi4.Init.CRCPolynomial = 10;
	HAL_SPI_Init(&hspi4);
	
	HAL_Delay(100);
}

/* 系统SPI硬件配置 */
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{

	GPIO_InitTypeDef GPIO_InitStruct;
	if(hspi->Instance == DW_SPI)
	{
		DW_SPI_CLK_ENABLE();
		DW_HAL_RCC_CLK_ENABLE();

		/**SPI1 GPIO Configuration
		PE12     ------> SPI4_SCK
		PE13     ------> SPI4_MISO
		PE14     ------> SPI4_MOSI
		*/
		GPIO_InitStruct.Pin = DW_SPI_SCK_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF5_SPI4;
		HAL_GPIO_Init(DW_SPI_SCK_PORT, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = DW_SPI_MISO_PIN;
		HAL_GPIO_Init(DW_SPI_MISO_PORT, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = DW_SPI_MOSI_PIN;
		HAL_GPIO_Init(DW_SPI_MOSI_PORT, &GPIO_InitStruct);

		/* PE11----SPI4_CS*/
		GPIO_InitStruct.Pin = DW_SPI4_CS_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(DW_SPI4_CS_PORT, &GPIO_InitStruct);
	}
}

/* 失能SPI */
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{

	if(hspi->Instance == DW_SPI)
	{
		DW_SPI_CLK_DISABLE();

		/**SPI1 GPIO Configuration
		PE12     ------> SPI4_SCK
		PE13     ------> SPI4_MISO
		PE14     ------> SPI4_MOSI
		*/
		HAL_GPIO_DeInit(DW_SPI_SCK_PORT, DW_SPI_SCK_PIN);
		HAL_GPIO_DeInit(DW_SPI_MISO_PORT, DW_SPI_MISO_PIN);
		HAL_GPIO_DeInit(DW_SPI_MOSI_PORT, DW_SPI_MOSI_PIN);
	}
}

