#include "dwOps.h"

static dwDevice_t *dev;

/* 中断初始化 */
void dwOpsInit(dwDevice_t *device)
{
	dev = device;
	NVIC_EnableIRQ(DW_IRQn);
}

/* 中断查询 */
static int checkIrq()
{
	return HAL_GPIO_ReadPin(DW_IRQ_PORT, DW_IRQ_PIN);
}

/* 外部中断回调函数 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch (GPIO_Pin) 
	{
		case DW_IRQ_PIN:
			do
			{
				dwHandleInterrupt(dev);
			} while(checkIrq() != 0); //while IRS line active (ARM can only do edge sensitive interrupts)
			HAL_NVIC_ClearPendingIRQ(DW_IRQn);
			break;
		case GPIO_PIN_8:
			break;
		default:
			break;
	} 
}

/*
*******************************************************************************
*	函 数 名: spiWrite
*	功能说明: DW1000的SPI通信写函数
*	形	  参: header：数据头（1 - 3 byte） data: 待写入函数
*	返 回 值: 无
*******************************************************************************
*/
uint16_t alignedBuffer[64];
static void spiWrite(dwDevice_t* dev, const void *header, size_t headerLength,const void* data, size_t dataLength)
{
	HAL_GPIO_WritePin(DW_SPI4_CS_PORT, DW_SPI4_CS_PIN, GPIO_PIN_RESET);

	memcpy(alignedBuffer, header, headerLength);
	HAL_SPI_Transmit(&hspi4, (uint8_t *)alignedBuffer, headerLength, HAL_MAX_DELAY);
	memcpy(alignedBuffer, data, dataLength);
	HAL_SPI_Transmit(&hspi4, (uint8_t *)alignedBuffer, dataLength, HAL_MAX_DELAY);

	HAL_GPIO_WritePin(DW_SPI4_CS_PORT, DW_SPI4_CS_PIN, GPIO_PIN_SET);
}

/*
*******************************************************************************
*	函 数 名: spiRead
*	功能说明: DW1000的SPI通信读函数
*	形	  参: header：数据头（1 - 3 byte） data: 读出的数据
*	返 回 值: 无
*******************************************************************************
*/
static void spiRead(dwDevice_t* dev, const void *header, size_t headerLength,void* data, size_t dataLength)
{
	HAL_GPIO_WritePin(DW_SPI4_CS_PORT, DW_SPI4_CS_PIN, GPIO_PIN_RESET);

	memcpy(alignedBuffer, header, headerLength);
	HAL_SPI_Transmit(&hspi4, (uint8_t *)alignedBuffer, headerLength, HAL_MAX_DELAY);
	HAL_SPI_Receive(&hspi4, (uint8_t *)alignedBuffer, dataLength, HAL_MAX_DELAY);
	memcpy(data, alignedBuffer, dataLength);

	HAL_GPIO_WritePin(DW_SPI4_CS_PORT, DW_SPI4_CS_PIN, GPIO_PIN_SET);
}

/* SPI速度设置 */
static void spiSetSpeed(dwDevice_t* dev, dwSpiSpeed_t speed)
{
	if(speed == dwSpiSpeedLow) 
	{
		DW_SPI_Init();
	} 
	else 
	{
		DW_SPI_Init_Fast();
	}
}

void delayms(dwDevice_t* dev, unsigned int delay)
{
	HAL_Delay(delay);
}

/* 硬件复位 */
void reset(dwDevice_t *dev)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.Pin = DW_RST_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(DE_RST_PORT, &GPIO_InitStructure);	
	
	HAL_GPIO_WritePin(DE_RST_PORT, GPIO_PIN_10,GPIO_PIN_RESET);
	HAL_Delay(1);

	GPIO_InitStructure.Pin = DW_RST_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(DE_RST_PORT, &GPIO_InitStructure);
    HAL_Delay(2);
}

dwOps_t dwOps = {
	.spiRead = spiRead,
	.spiWrite = spiWrite,
	.spiSetSpeed = spiSetSpeed,
	.delayms = delayms,
    .reset = reset, 
};
