#ifndef _SYS_H
#define _SYS_H
#include "stm32f4xx.h"

///////////////////////////////////////////////////////////////////////////////////
//����һЩ���õ��������Ͷ̹ؼ��� 
typedef int32_t  s32;
typedef int16_t s16;
typedef int8_t  s8;

typedef const int32_t sc32;  
typedef const int16_t sc16;  
typedef const int8_t sc8;  

typedef __IO int32_t  vs32;
typedef __IO int16_t  vs16;
typedef __IO int8_t   vs8;

typedef __I int32_t vsc32;  
typedef __I int16_t vsc16; 
typedef __I int8_t vsc8;   

typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef const uint32_t uc32;  
typedef const uint16_t uc16;  
typedef const uint8_t uc8; 

typedef __IO uint32_t  vu32;
typedef __IO uint16_t vu16;
typedef __IO uint8_t  vu8;

typedef __I uint32_t vuc32;  
typedef __I uint16_t vuc16; 
typedef __I uint8_t vuc8;  
	 
//λ������,ʵ��51���Ƶ�GPIO���ƹ���
//����ʵ��˼��,�ο�<<CM3Ȩ��ָ��>>������(87ҳ~92ҳ).M4ͬM3����,ֻ�ǼĴ�����ַ����.
//IO�ڲ����궨��
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO�ڵ�ַӳ��
#define GPIOA_ODR_Addr    (GPIOA_BASE+20) //0x40020014
#define GPIOB_ODR_Addr    (GPIOB_BASE+20) //0x40020414 
#define GPIOC_ODR_Addr    (GPIOC_BASE+20) //0x40020814 
#define GPIOD_ODR_Addr    (GPIOD_BASE+20) //0x40020C14 
#define GPIOE_ODR_Addr    (GPIOE_BASE+20) //0x40021014 
#define GPIOF_ODR_Addr    (GPIOF_BASE+20) //0x40021414    
#define GPIOG_ODR_Addr    (GPIOG_BASE+20) //0x40021814   
#define GPIOH_ODR_Addr    (GPIOH_BASE+20) //0x40021C14    
#define GPIOI_ODR_Addr    (GPIOI_BASE+20) //0x40022014 
#define GPIOJ_ODR_ADDr    (GPIOJ_BASE+20) //0x40022414
#define GPIOK_ODR_ADDr    (GPIOK_BASE+20) //0x40022814

#define GPIOA_IDR_Addr    (GPIOA_BASE+16) //0x40020010 
#define GPIOB_IDR_Addr    (GPIOB_BASE+16) //0x40020410 
#define GPIOC_IDR_Addr    (GPIOC_BASE+16) //0x40020810 
#define GPIOD_IDR_Addr    (GPIOD_BASE+16) //0x40020C10 
#define GPIOE_IDR_Addr    (GPIOE_BASE+16) //0x40021010 
#define GPIOF_IDR_Addr    (GPIOF_BASE+16) //0x40021410 
#define GPIOG_IDR_Addr    (GPIOG_BASE+16) //0x40021810 
#define GPIOH_IDR_Addr    (GPIOH_BASE+16) //0x40021C10 
#define GPIOI_IDR_Addr    (GPIOI_BASE+16) //0x40022010 
#define GPIOJ_IDR_Addr    (GPIOJ_BASE+16) //0x40022410 
#define GPIOK_IDR_Addr    (GPIOK_BASE+16) //0x40022810 

/* Wifi debugging options */
#define LOG_ON		0

/* Exported types ------------------------------------------------------------*/ 
typedef enum
{
    NONE,
    ERR,
    WARN,
    INFO,
    DEBUG
} msg_level_t;

/* Exported constants --------------------------------------------------------*/

#define DEF_LOGLEVEL DEBUG  /** default message level */

/* Exported macro ------------------------------------------------------------*/
/** @brief  Print the debug info through uart.
  * @param  level: specifies the message level.
  *         This parameter can be one of the following values:
  *            @arg NONE:  
  *            @arg ERR:  
  *            @arg WARN:  
  *            @arg INFO:  
  *            @arg DEBUG: 
  * @param  message: output message.
  *        
  * @retval: none.
  */	
#if LOG_ON
#define log_print(level,message)  do{\
			if( level <= DEF_LOGLEVEL ){\
				switch(level){\
					case ERR:printf( "[Error]:");printf message;\
						break;\
					case WARN:printf( "[WARN]:");printf message;\
						break;\
					case INFO:printf( "[INFO]:");printf message;\
						break;\
					case DEBUG:printf( "[DEBUG]:");printf message;\
						break;\
					default:\
						printf( "[Unknown Msg]:");printf message;\
						break;\
				}\
			}\
		}while(0)
		
#define Print(x) do{printf x;}while(0)
#else
#define log_print(level,message)
#define Print(x)
#endif
	
/* Exported functions --------------------------------------------------------*/
#define SYS_REBOOT()\
 do {\
     printf("\r\nReboot device at line %d in %s\r\n", __LINE__, __FILE__);\
		 udp_demo_connection_close(udppcb); \
     __set_PRIMASK(1);\
     NVIC_SystemReset();\
 } while(0)
//__disable_fault_irq();

void Stm32_Clock_Init(u32 plln,u32 pllm,u32 pllp,u32 pllq);//ʱ��ϵͳ����
//����Ϊ��ຯ��
void WFI_SET(void);		//ִ��WFIָ��
void INTX_DISABLE(void);//�ر������ж�
void INTX_ENABLE(void);	//���������ж�
void MSR_MSP(u32 addr);	//���ö�ջ��ַ 
#endif

