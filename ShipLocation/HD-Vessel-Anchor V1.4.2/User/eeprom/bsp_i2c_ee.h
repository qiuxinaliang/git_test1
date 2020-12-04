#ifndef __I2C_EE_H
#define	__I2C_EE_H

#include "common.h"

/* AT24C01/02每页有8个字节 */
//#define EEPROM_PAGESIZE    8
#define EEPROM_PAGESIZE 	   8
/* AT24C04/08A/16A每页有16个字节 */
//#define EEPROM_PAGESIZE           16			

/* 这个地址只要与STM32外挂的I2C器件地址不一样即可 */
#define I2C_OWN_ADDRESS7      0X0A   

#define I2Cx                             I2C1
#define I2Cx_CLK_ENABLE()                __HAL_RCC_I2C1_CLK_ENABLE()
#define I2Cx_SDA_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()
#define I2Cx_SCL_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE() 

#define I2Cx_FORCE_RESET()               __HAL_RCC_I2C1_FORCE_RESET()
#define I2Cx_RELEASE_RESET()             __HAL_RCC_I2C1_RELEASE_RESET()

/* Definition for I2Cx Pins */
#define I2Cx_SCL_PIN                    GPIO_PIN_8
#define I2Cx_SCL_GPIO_PORT              GPIOB
#define I2Cx_SCL_AF                     GPIO_AF4_I2C1
#define I2Cx_SDA_PIN                    GPIO_PIN_9
#define I2Cx_SDA_GPIO_PORT              GPIOB
#define I2Cx_SDA_AF                     GPIO_AF4_I2C1


/*等待超时时间*/
#define I2CT_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define I2CT_LONG_TIMEOUT         ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))

#define I2Cx_TIMEOUT_MAX                300
/* Maximum number of trials for HAL_I2C_IsDeviceReady() function */
#define EEPROM_MAX_TRIALS               300

/*信息输出*/
#define EEPROM_DEBUG_ON         0

#define EEPROM_INFO(fmt,arg...)           printf("<<-EEPROM-INFO->> "fmt"\n",##arg)
#define EEPROM_ERROR(fmt,arg...)          printf("<<-EEPROM-ERROR->> "fmt"\n",##arg)
#define EEPROM_DEBUG(fmt,arg...)          do{\
                                          if(EEPROM_DEBUG_ON)\
                                          printf("<<-EEPROM-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                          }while(0)

/* 
 * AT24C02 2kb = 2048bit = 2048/8 B = 256 B
 * 32 pages of 8 bytes each
 *
 * Device Address
 * 1 0 1 0 A2 A1 A0 R/W
 * 1 0 1 0 0  0  0  0 = 0XA0
 * 1 0 1 0 0  0  0  1 = 0XA1 
 */

/* EEPROM Addresses defines */
#define EEPROM_Block0_ADDRESS 0xA0   /* E2 = 0 */
#define EEPROM_Block1_ADDRESS 0xA2 /* E2 = 0 */
#define EEPROM_Block2_ADDRESS 0xA4 /* E2 = 0 */
#define EEPROM_Block3_ADDRESS 0xA6 /* E2 = 0 */

#define EEPROM_ADDRESS        0xA0

#define EE_TimeStamp_Addr 0x00
#define EE_UpgarteFlag_Addr 0x0A
#define EE_DeviceType_Addr 0x0B
#define EE_AnchorWorkMode_Addr 0x0C
#define EE_PowerOnType_Addr 0x0D
#define EE_Network_Addr 0x0E
#define EE_Version_Addr 0x30																					
#pragma pack(1)
typedef struct
{
	uint32_t timestamp;
}E_Staticgloabl_stru;
#pragma pack()

#pragma pack(1)
typedef struct
{
	uint8_t SoftwareVersion[3];		// 软件版本
  uint8_t HardwareVersion[3];		// 硬件版本
}E_Version_stru;
#pragma pack()

#pragma pack(1)
typedef struct
{
	uint8_t upgrate_mark;
	uint8_t upgrateFlag;		// 升级标志
}E_upgrateInfo_stru;
#pragma pack()

#pragma pack(1)
typedef struct
{
	uint8_t HardwareInfo_mark;
  uint8_t DeviceType;				// 设备类型
  uint8_t SelfCheckInfo;		// 自检信息
  uint8_t AnchorWorkMode;		// 基站工作模式
	uint8_t PowerOnType;			// 开机启动类型
}E_HardwareInfo_Stru;
#pragma pack()

#pragma pack(1)  
typedef struct 
{
	uint8_t net_mark;
  uint8_t Anchoripaddress[4];
  uint16_t AnchorPort;
  uint8_t Subnetmask[4];
  uint8_t Gateway[4];
  uint8_t Centeripaddress[4];
  uint16_t CenterPort;
  uint8_t Engineipaddress[4];
  uint16_t EnginePort;
}E_Network_Manage_stru;
#pragma pack()

extern E_Staticgloabl_stru E_Staticgloabl_stru_temp;
extern E_Version_stru E_Version_stru_temp;
extern E_Network_Manage_stru E_Network_Manage_stru_temp;
extern uint8_t upgrateFlag;		// 升级标志
extern uint8_t E_DeviceType;				// 设备类型
extern uint8_t E_AnchorWorkMode;		// 基站工作模式
extern uint8_t E_PowerOnType;			// 开机启动类型
//#pragma pack(1)
//typedef struct STMFlashReadWrite_Stru
//{
//	upgrateInfo_stru upgrateInfo_stru_temp;
//	HardwareInfo_Stru HardwareInfo_stru_temp;
//	Network_Manage Network_Manage_stru_temp; // 网络参数保存
//	Staticgloabl_stru Staticgloabl_stru_temp;
//}STMFlashReadWrite_Stru;
//#pragma pack()																					

void I2C_EE_Init(void);
										  
void I2C_EE_BufferWrite(uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite);
uint32_t I2C_EE_ByteWrite(uint8_t* pBuffer, uint8_t WriteAddr);
uint32_t I2C_EE_PageWrite(uint8_t* pBuffer, uint8_t WriteAddr, uint8_t NumByteToWrite);
uint32_t I2C_EE_BufferRead(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead);
uint8_t I2C_Test1(void);
uint8_t I2C_Test(void);
void EE_TimeStamp_Write(void);
void EE_TimeStamp_Read(void);
void EE_Version_Write(void);
void EE_Version_Read(void);
void EE_upgrateFlag_Write(void);
void EE_upgrateFlag_Read(void);
void EE_DeviceType_Write(void);
void EE_DeviceType_Read(void);
void EE_AnchorWorkMode_Write(void);
void EE_AnchorWorkMode_Read(void);
void EE_PowerOnType_Write(void);
void EE_PowerOnType_Read(void);
void EE_Network_Write(void);
void EE_Network_Read(void);
extern I2C_HandleTypeDef  I2C_Handle; 
#endif /* __I2C_EE_H */
