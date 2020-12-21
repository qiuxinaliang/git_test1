/*
 * @:*********************************************************************************************************: 
 * @Date: 2020-11-04 10:36:13
 * @LastEditTime: 2020-11-11 15:16:12
 * @**********************************************************************************************************: 
 */
#ifndef __BOARD__H__
#define __BOARD__H__

#include "common.h"
/* Wifi debugging options */
#define LOG_ON		1
#define u32 uint32_t
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
     __set_PRIMASK(1);\
     NVIC_SystemReset();\
 } while(0)
//__disable_fault_irq();

#pragma pack(1)
typedef struct 
{
  uint8_t SoftwareVersionID;		// 软件版本
  uint8_t SoftwareVersion[3];		// 软件版本
  uint8_t HardwareVersionID;		// 硬件版本
  uint8_t HardwareVersion[3];		// 硬件版本
  uint8_t SelfCheckInfoID;
  uint8_t SelfCheckInfo;		// 自检信息
  uint8_t AnchorWorkModeID;		// 基站工作模式
  uint8_t AnchorWorkMode;		// 基站工作模式
  uint8_t PowerOnTypeID;
	uint8_t PowerOnType;			// 开机启动类型
  uint8_t AnchoripaddressID;
  uint8_t Anchoripaddress[4];
  uint8_t AnchorPortID;
  uint16_t AnchorPort;
  uint8_t SubnetmaskID;
  uint8_t Subnetmask[4];
  uint8_t GatewayID;
  uint8_t Gateway[4];
  uint8_t CenteripaddressID;
  uint8_t Centeripaddress[4];
  uint8_t CenterPortID;
  uint16_t CenterPort;
  uint8_t EngineipaddressID;
  uint8_t Engineipaddress[4];
  uint16_t EnginePortID;
  uint16_t EnginePort;
}DeviceConfigInfo_Stru;
#pragma pack()

//以下为汇编函数
void WFI_SET(void);		//执行WFI指令
void INTX_DISABLE(void);//关闭所有中断
void INTX_ENABLE(void);	//开启所有中断
void MSR_MSP(u32 addr);	//设置堆栈地址 

extern uint8_t DeviceSelfCheck;
#endif  //!__BOARD__H__
