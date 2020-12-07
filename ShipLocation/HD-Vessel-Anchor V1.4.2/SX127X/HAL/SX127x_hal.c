/*
 * @:*********************************************************************************************************: 
 * @Date: 2020-12-07 16:14:03
 * @LastEditTime: 2020-12-07 16:44:17
 * @**********************************************************************************************************: 
 */
#include "sx127x_hal.h"

extern SPI_HandleTypeDef hspi4;
void Soft_delay_ms(uint16_t time)
{    
   uint16_t i=0;  
   while(time--)
   {
      i = 7950; 
      while(i--) ;    
   }
}

void SX1276HALInit( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	HAL_GPIO_WritePin(Lora_SPI_CS_PORT, Lora_SPI_CS_PIN, GPIO_PIN_SET );
	Lora_SPI_Init();
	
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_MEDIUM;
	GPIO_InitStructure.Pin = RESET_PIN;
	GPIO_InitStructure.Mode =  GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init( RESET_IOPORT, &GPIO_InitStructure );
	HAL_GPIO_WritePin( RESET_IOPORT, RESET_PIN, GPIO_PIN_SET );

	// Configure radio DIO as inputs
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;

	// Configure DIO0
	GPIO_InitStructure.Pin =  DIO0_PIN;
	HAL_GPIO_Init( DIO0_IOPORT, &GPIO_InitStructure );
	
	// Configure DIO1
	GPIO_InitStructure.Pin =  DIO1_PIN;
	HAL_GPIO_Init( DIO1_IOPORT, &GPIO_InitStructure );
	
	// Configure DIO3
	GPIO_InitStructure.Pin =  DIO3_PIN;
	HAL_GPIO_Init( DIO3_IOPORT, &GPIO_InitStructure );
	
	// Configure DIO4
	GPIO_InitStructure.Pin =  DIO4_PIN;
	HAL_GPIO_Init( DIO4_IOPORT, &GPIO_InitStructure );
}

uint8_t SpiInOut( uint8_t outData )
{
	uint8_t temp = 0;
    /* Send SPIy data */
	HAL_SPI_TransmitReceive(&hspi4, (uint8_t *)&outData, (uint8_t *)&temp, 1, HAL_MAX_DELAY);
	//HAL_SPI_Transmit(&hspi1, (uint8_t *)&outData, 1, HAL_MAX_DELAY);
	//HAL_SPI_Receive(&hspi1, (uint8_t *)&temp, 1, HAL_MAX_DELAY);
  return temp;
}


void SpiNSSEnable( uint8_t status )
{
  HAL_GPIO_WritePin(Lora_SPI_CS_PORT, Lora_SPI_CS_PIN, status );
}


void SX127X_ResetPinControl( uint8_t status ){
	HAL_GPIO_WritePin( RESET_IOPORT, RESET_PIN, status );
	
}


uint8_t SX1276ReadDio0(void){
	return HAL_GPIO_ReadPin( DIO0_IOPORT, DIO0_PIN );
}


uint8_t SX1276ReadDio1(void){
	return HAL_GPIO_ReadPin( DIO1_IOPORT, DIO1_PIN );
}

uint8_t SX1276ReadDio3(void){
	return HAL_GPIO_ReadPin( DIO3_IOPORT, DIO3_PIN );
}

uint8_t SX1276ReadDio4(void){
	return HAL_GPIO_ReadPin( DIO4_IOPORT, DIO4_PIN );
}


