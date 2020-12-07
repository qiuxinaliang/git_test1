/*
 * @:*********************************************************************************************************: 
 * @Date: 2020-11-16 10:31:55
 * @LastEditTime: 2020-12-07 16:37:11
 * @**********************************************************************************************************: 
 */
#ifndef __SX127X_HAL_H__
#define __SX127X_HAL_H__


#include "common.h"

#define RESET_IOPORT                                GPIOE
#define RESET_PIN                                   GPIO_PIN_10

#define DIO0_IOPORT                                 GPIOE
#define DIO0_PIN                                    GPIO_PIN_9

#define DIO1_IOPORT                                 GPIOA
#define DIO1_PIN                                    GPIO_PIN_11

#define DIO3_IOPORT                                 GPIOD
#define DIO3_PIN                                    GPIO_PIN_9

#define DIO4_IOPORT                                 GPIOD
#define DIO4_PIN                                    GPIO_PIN_6

#define DEBUG( format , ... )	printf( format , ##__VA_ARGS__ )	//打印log信息的函数，可以不设置
#define GET_TICK_COUNT() HAL_GetTick()	                        //获取systick的函数

void Soft_delay_ms(uint16_t time);
void SX1276HALInit( void );
uint8_t SpiInOut( uint8_t outData );
void SpiNSSEnable( uint8_t status );
void SX127X_ResetPinControl( uint8_t status );
uint8_t SX1276ReadDio0(void);
uint8_t SX1276ReadDio1(void);
uint8_t SX1276ReadDio2(void);
uint8_t SX1276ReadDio3(void);
uint8_t SX1276ReadDio4(void);
uint8_t SX1276ReadDio5(void);

#endif //end of __SX127X_HAL_H__
