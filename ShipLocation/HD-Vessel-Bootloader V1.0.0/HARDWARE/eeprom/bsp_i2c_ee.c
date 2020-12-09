/**
  ******************************************************************************
  * @file    bsp_i2c_ee.c
  * @author  STMicroelectronics
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   i2c EEPROM(AT24C02)应用函数bsp
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F429 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

#include "bsp_i2c_ee.h"

#define  DATA_Size			256
#define  EEP_Firstpage      0x00
uint8_t I2c_Buf_Write[DATA_Size];
uint8_t I2c_Buf_Read[DATA_Size];

E_Staticgloabl_stru E_Staticgloabl_stru_temp;
E_Version_stru E_Version_stru_temp;
E_Network_Manage_stru E_Network_Manage_stru_temp;
uint8_t upgrateFlag;		// 升级标志
uint8_t E_DeviceType;				// 设备类型
uint8_t E_AnchorWorkMode;		// 基站工作模式
uint8_t E_PowerOnType;			// 开机启动类型

I2C_HandleTypeDef  I2C_Handle; 

/**
  * @brief I2C MSP Initialization 
  *        This function configures the hardware resources used in this example: 
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration  
  *           - DMA configuration for transmission request by peripheral 
  *           - NVIC configuration for DMA interrupt request enable
  * @param hi2c: I2C handle pointer
  * @retval None
  */
void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{  
  GPIO_InitTypeDef  GPIO_InitStruct;
  
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO TX/RX clock */
  I2Cx_SCL_GPIO_CLK_ENABLE();
  I2Cx_SDA_GPIO_CLK_ENABLE();
  /* Enable I2C1 clock */
  I2Cx_CLK_ENABLE(); 
  
  /*##-2- Configure peripheral GPIO ##########################################*/  
  /* I2C TX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = I2Cx_SCL_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull      = GPIO_NOPULL;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
  GPIO_InitStruct.Alternate = I2Cx_SCL_AF;
  
  HAL_GPIO_Init(I2Cx_SCL_GPIO_PORT, &GPIO_InitStruct);
    
  /* I2C RX GPIO pin configuration  */
  GPIO_InitStruct.Pin = I2Cx_SDA_PIN;
  GPIO_InitStruct.Alternate = I2Cx_SDA_AF;
    
  HAL_GPIO_Init(I2Cx_SDA_GPIO_PORT, &GPIO_InitStruct);
  
  	/* Force the I2C peripheral clock reset */  
	I2Cx_FORCE_RESET() ; 

	/* Release the I2C peripheral clock reset */  
	I2Cx_RELEASE_RESET(); 
}


/**
  * @brief  I2C 工作模式配置
  * @param  无
  * @retval 无
  */
static void I2C_Mode_Config(void)
{
   
  I2C_Handle.Instance             = I2Cx;
  
  I2C_Handle.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
  I2C_Handle.Init.ClockSpeed      = 400000;
  I2C_Handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  I2C_Handle.Init.DutyCycle       = I2C_DUTYCYCLE_2;
  I2C_Handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  I2C_Handle.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;
  I2C_Handle.Init.OwnAddress1     = I2C_OWN_ADDRESS7 ;
  I2C_Handle.Init.OwnAddress2     = 0; 
    /* Init the I2C */
  HAL_I2C_Init(&I2C_Handle);	

  HAL_I2CEx_AnalogFilter_Config(&I2C_Handle, I2C_ANALOGFILTER_ENABLE);    
}

/**
  * @brief  I2C 外设(EEPROM)初始化
  * @param  无
  * @retval 无
  */
void I2C_EE_Init(void)
{
	I2C_Mode_Config();
}

/**
  * @brief   将缓冲区中的数据写到I2C EEPROM中
  * @param   
  *		@arg pBuffer:缓冲区指针
  *		@arg WriteAddr:写地址
  *     @arg NumByteToWrite:写的字节数
  * @retval  无
  */
void I2C_EE_BufferWrite(uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite)
{ 
  uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0;

  Addr = WriteAddr % EEPROM_PAGESIZE;
  count = EEPROM_PAGESIZE - Addr;
  NumOfPage =  NumByteToWrite / EEPROM_PAGESIZE;
  NumOfSingle = NumByteToWrite % EEPROM_PAGESIZE;
 
  /* If WriteAddr is I2C_PageSize aligned  */
  if(Addr == 0) 
  {
    /* If NumByteToWrite < I2C_PageSize */
    if(NumOfPage == 0) 
    {
      I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
    }
    /* If NumByteToWrite > I2C_PageSize */
    else  
    {
      while(NumOfPage--)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, EEPROM_PAGESIZE); 
        WriteAddr +=  EEPROM_PAGESIZE;
        pBuffer += EEPROM_PAGESIZE;
      }

      if(NumOfSingle!=0)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
  /* If WriteAddr is not I2C_PageSize aligned  */
  else 
  {
    /* If NumByteToWrite < I2C_PageSize */
    if(NumOfPage== 0) 
    {
      I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
    }
    /* If NumByteToWrite > I2C_PageSize */
    else
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / EEPROM_PAGESIZE;
      NumOfSingle = NumByteToWrite % EEPROM_PAGESIZE;	
      
      if(count != 0)
      {  
        I2C_EE_PageWrite(pBuffer, WriteAddr, count);
        WriteAddr += count;
        pBuffer += count;
      } 
      
      while(NumOfPage--)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, EEPROM_PAGESIZE);
        WriteAddr +=  EEPROM_PAGESIZE;
        pBuffer += EEPROM_PAGESIZE;  
      }
      if(NumOfSingle != 0)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle); 
      }
    }
  }  
}

/**
  * @brief   写一个字节到I2C EEPROM中
  * @param   
  *		@arg pBuffer:缓冲区指针
  *		@arg WriteAddr:写地址 
  * @retval  无
  */
uint32_t I2C_EE_ByteWrite(uint8_t* pBuffer, uint8_t WriteAddr)
{
	HAL_StatusTypeDef status = HAL_OK;

	status = HAL_I2C_Mem_Write(&I2C_Handle, EEPROM_ADDRESS, (uint16_t)WriteAddr, I2C_MEMADD_SIZE_8BIT, pBuffer, 1, 100); 

	/* Check the communication status */
	if(status != HAL_OK)
	{
	/* Execute user timeout callback */
	//I2Cx_Error(Addr);
	}
	while (HAL_I2C_GetState(&I2C_Handle) != HAL_I2C_STATE_READY)
	{
		
	}

	/* Check if the EEPROM is ready for a new operation */
	while (HAL_I2C_IsDeviceReady(&I2C_Handle, EEPROM_ADDRESS, EEPROM_MAX_TRIALS, I2Cx_TIMEOUT_MAX) == HAL_TIMEOUT);

	/* Wait for the end of the transfer */
	while (HAL_I2C_GetState(&I2C_Handle) != HAL_I2C_STATE_READY)
	{
		
	}

	return status;
}

/**
  * @brief   在EEPROM的一个写循环中可以写多个字节，但一次写入的字节数
  *          不能超过EEPROM页的大小，AT24C02每页有8个字节
  * @param   
  *		@arg pBuffer:缓冲区指针
  *		@arg WriteAddr:写地址
  *     @arg NumByteToWrite:写的字节数
  * @retval  无
  */
uint32_t I2C_EE_PageWrite(uint8_t* pBuffer, uint8_t WriteAddr, uint8_t NumByteToWrite)
{
	HAL_StatusTypeDef status = HAL_OK;
	/* Write EEPROM_PAGESIZE */
	status=HAL_I2C_Mem_Write(&I2C_Handle, EEPROM_ADDRESS,WriteAddr, I2C_MEMADD_SIZE_8BIT, (uint8_t*)(pBuffer),NumByteToWrite, 100);

	while (HAL_I2C_GetState(&I2C_Handle) != HAL_I2C_STATE_READY)
	{
		
	}

	/* Check if the EEPROM is ready for a new operation */
	while (HAL_I2C_IsDeviceReady(&I2C_Handle, EEPROM_ADDRESS, EEPROM_MAX_TRIALS, I2Cx_TIMEOUT_MAX) == HAL_TIMEOUT);

	/* Wait for the end of the transfer */
	while (HAL_I2C_GetState(&I2C_Handle) != HAL_I2C_STATE_READY)
	{
		
	}
	return status;
}

/**
  * @brief   从EEPROM里面读取一块数据 
  * @param   
  *		@arg pBuffer:存放从EEPROM读取的数据的缓冲区指针
  *		@arg WriteAddr:接收数据的EEPROM的地址
  *     @arg NumByteToWrite:要从EEPROM读取的字节数
  * @retval  无
  */
uint32_t I2C_EE_BufferRead(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead)
{
	HAL_StatusTypeDef status = HAL_OK;
	status=HAL_I2C_Mem_Read(&I2C_Handle,EEPROM_ADDRESS,ReadAddr, I2C_MEMADD_SIZE_8BIT, (uint8_t *)pBuffer, NumByteToRead,1000);
	return status;
}


uint8_t I2C_Test1(void)
{
	uint16_t i;

	//EEPROM_INFO("写入的数据");

	for ( i=0; i<DATA_Size; i++ ) //填充缓冲
	{   
		I2c_Buf_Write[i] =i;
		printf("0x%02X ", I2c_Buf_Write[i]);
		if(i%16 == 15)    
		printf("\n\r");    
	}

	//将I2c_Buf_Write中顺序递增的数据写入EERPOM中 
	I2C_EE_BufferWrite( I2c_Buf_Write, EEP_Firstpage, DATA_Size);

	//EEPROM_INFO("读出的数据");
	//将EEPROM读出数据顺序保持到I2c_Buf_Read中
	I2C_EE_BufferRead(I2c_Buf_Read, EEP_Firstpage, DATA_Size); 
	//将I2c_Buf_Read中的数据通过串口打印
	for (i=0; i<DATA_Size; i++)
	{	
		if(I2c_Buf_Read[i] != I2c_Buf_Write[i])
		{
			printf("0x%02X ", I2c_Buf_Read[i]);
			//EEPROM_ERROR("错误:I2C EEPROM写入与读出的数据不一致");
			return 0;
		}
		printf("0x%02X ", I2c_Buf_Read[i]);
		if(i%16 == 15)    
		printf("\n\r");

	}
	//EEPROM_INFO("I2C(AT24C02)读写测试成功");
	return 1;
}

#if 1
uint8_t I2C_Test(void)
{
	#if 1
	E_Staticgloabl_stru_temp.timestamp = 1604489403;
	E_Version_stru_temp.HardwareVersion[0] = 1;
	E_Version_stru_temp.HardwareVersion[1] = 0;
	E_Version_stru_temp.HardwareVersion[2] = 1;
	E_Version_stru_temp.SoftwareVersion[0] = 1;
	E_Version_stru_temp.SoftwareVersion[1] = 4;
	E_Version_stru_temp.SoftwareVersion[2] = 2;
	#endif
	
	E_Network_Manage_stru_temp.net_mark = 0x00;
  E_Network_Manage_stru_temp.Anchoripaddress[0] = 192;
  E_Network_Manage_stru_temp.Anchoripaddress[1] = 168;
  E_Network_Manage_stru_temp.Anchoripaddress[2] = 1;
  E_Network_Manage_stru_temp.Anchoripaddress[3] = 31;
  E_Network_Manage_stru_temp.AnchorPort = 6009;
  
  E_Network_Manage_stru_temp.Subnetmask[0] = 255;
  E_Network_Manage_stru_temp.Subnetmask[1] = 255;
  E_Network_Manage_stru_temp.Subnetmask[2] = 255;
  E_Network_Manage_stru_temp.Subnetmask[3] = 0;

  E_Network_Manage_stru_temp.Gateway[0] = 192;
  E_Network_Manage_stru_temp.Gateway[1] = 168;
  E_Network_Manage_stru_temp.Gateway[2] = 1;
  E_Network_Manage_stru_temp.Gateway[3] = 1;

  E_Network_Manage_stru_temp.Centeripaddress[0] = 192;
  E_Network_Manage_stru_temp.Centeripaddress[1] = 168;
  E_Network_Manage_stru_temp.Centeripaddress[2] = 1;
  E_Network_Manage_stru_temp.Centeripaddress[3] = 100;
  E_Network_Manage_stru_temp.CenterPort = 6009;

  E_Network_Manage_stru_temp.Engineipaddress[0] = 192;
  E_Network_Manage_stru_temp.Engineipaddress[1] = 168;
  E_Network_Manage_stru_temp.Engineipaddress[2] = 1;
  E_Network_Manage_stru_temp.Engineipaddress[3] = 100;
  E_Network_Manage_stru_temp.EnginePort = 6009;
	#if 1
	upgrateFlag = 0x00;
	E_DeviceType = 0x03;
	E_AnchorWorkMode = 0x02;
	E_PowerOnType = 0x01;
	
	EE_TimeStamp_Write();
	EE_Version_Write();
	EE_upgrateFlag_Write();
	EE_DeviceType_Write();
	EE_AnchorWorkMode_Write();
	EE_PowerOnType_Write();
	#endif
	EE_Network_Write();
	
	E_Staticgloabl_stru_temp.timestamp = 0;
	E_Version_stru_temp.HardwareVersion[0] = 0;
	E_Version_stru_temp.HardwareVersion[1] = 0;
	E_Version_stru_temp.HardwareVersion[2] = 0;
	E_Version_stru_temp.SoftwareVersion[0] = 0;
	E_Version_stru_temp.SoftwareVersion[1] = 0;
	E_Version_stru_temp.SoftwareVersion[2] = 0;
	
	E_Network_Manage_stru_temp.net_mark = 0x00;
  E_Network_Manage_stru_temp.Anchoripaddress[0] = 0;
  E_Network_Manage_stru_temp.Anchoripaddress[1] = 0;
  E_Network_Manage_stru_temp.Anchoripaddress[2] = 0;
  E_Network_Manage_stru_temp.Anchoripaddress[3] = 0;
  E_Network_Manage_stru_temp.AnchorPort = 0;
  
  E_Network_Manage_stru_temp.Subnetmask[0] = 0;
  E_Network_Manage_stru_temp.Subnetmask[1] = 0;
  E_Network_Manage_stru_temp.Subnetmask[2] = 0;
  E_Network_Manage_stru_temp.Subnetmask[3] = 0;

  E_Network_Manage_stru_temp.Gateway[0] = 0;
  E_Network_Manage_stru_temp.Gateway[1] = 0;
  E_Network_Manage_stru_temp.Gateway[2] = 0;
  E_Network_Manage_stru_temp.Gateway[3] = 0;

  E_Network_Manage_stru_temp.Centeripaddress[0] = 0;
  E_Network_Manage_stru_temp.Centeripaddress[1] = 0;
  E_Network_Manage_stru_temp.Centeripaddress[2] = 0;
  E_Network_Manage_stru_temp.Centeripaddress[3] = 0;
  E_Network_Manage_stru_temp.CenterPort = 0;

  E_Network_Manage_stru_temp.Engineipaddress[0] = 0;
  E_Network_Manage_stru_temp.Engineipaddress[1] = 0;
  E_Network_Manage_stru_temp.Engineipaddress[2] = 0;
  E_Network_Manage_stru_temp.Engineipaddress[3] = 0;
  E_Network_Manage_stru_temp.EnginePort = 0;
	#if 1
	upgrateFlag = 0x0;
	E_DeviceType = 0x0;
	E_AnchorWorkMode = 0x0;
	E_PowerOnType = 0x0;
	EE_TimeStamp_Read();
	EE_Version_Read();
	EE_upgrateFlag_Read();
	EE_DeviceType_Read();
	EE_AnchorWorkMode_Read();
	EE_PowerOnType_Read(); 
	#endif
	EE_Network_Read();
	#if 1
	log_print(DEBUG,("timestamp = %x\r\n",  E_Staticgloabl_stru_temp.timestamp));
	for(int i = 0; i < 3; i++)
		  log_print(DEBUG,("HardwareVersion[%d] = %d\r\n",i, E_Version_stru_temp.HardwareVersion[i]));
	for(int i = 0; i < 3; i++)
		  log_print(DEBUG,("SoftwareVersion[%d] = %d\r\n",i, E_Version_stru_temp.SoftwareVersion[i]));
 #endif	
	
	for(int i = 0; i < 4; i++)
		  log_print(DEBUG,("ipaddress[%d] = %d\r\n",i, E_Network_Manage_stru_temp.Anchoripaddress[i]));
	log_print(DEBUG,("AnchorPort = %d\r\n", E_Network_Manage_stru_temp.AnchorPort));	
	for(int i = 0; i < 4; i++)
			log_print(DEBUG,("Subnetmask[%d] = %d\r\n",i, E_Network_Manage_stru_temp.Subnetmask[i]));
	for(int i = 0; i < 4; i++)
		log_print(DEBUG,("Gateway[%d] = %d\r\n",i, E_Network_Manage_stru_temp.Gateway[i]));
	for(int i = 0; i < 4; i++)
				log_print(DEBUG,("Centeripaddress[%d] = %d\r\n",i, E_Network_Manage_stru_temp.Centeripaddress[i]));
	log_print(DEBUG,("CenterPort = %d\r\n", E_Network_Manage_stru_temp.CenterPort));
		for(int i = 0; i < 4; i++)
				log_print(DEBUG,("Engineipaddress[%d] = %d\r\n",i, E_Network_Manage_stru_temp.Engineipaddress[i]));
	log_print(DEBUG,("EnginePort = %d\r\n", E_Network_Manage_stru_temp.EnginePort));
	#if 1
	log_print(DEBUG, ("upgrateFlag = %x\r\n",  upgrateFlag));
	log_print(DEBUG, ("E_DeviceType = %x\r\n",  E_DeviceType));
	log_print(DEBUG, ("E_AnchorWorkMode = %x\r\n",  E_AnchorWorkMode));
	log_print(DEBUG, ("E_PowerOnType = %x\r\n",  E_PowerOnType));
	#endif
	return 1;
}
#endif

//时戳的读写
void EE_TimeStamp_Write(void)
{
	I2C_EE_BufferWrite((uint8_t *)&E_Staticgloabl_stru_temp, EE_TimeStamp_Addr, 4);
}

void EE_TimeStamp_Read(void)
{
	I2C_EE_BufferRead((uint8_t *)&E_Staticgloabl_stru_temp, EE_TimeStamp_Addr, 4);
}

void EE_Version_Write(void)
{
	I2C_EE_BufferWrite((uint8_t *)&E_Version_stru_temp, EE_Version_Addr, sizeof(E_Version_stru));
}

void EE_Version_Read(void)
{
	I2C_EE_BufferRead((uint8_t *)&E_Version_stru_temp, EE_Version_Addr, sizeof(E_Version_stru));
}

void EE_upgrateFlag_Write(void)
{
	I2C_EE_BufferWrite((uint8_t *)&upgrateFlag, EE_UpgarteFlag_Addr, 1);
}

void EE_upgrateFlag_Read(void)
{
	I2C_EE_BufferRead((uint8_t *)&upgrateFlag, EE_UpgarteFlag_Addr, 1);
}

void EE_DeviceType_Write(void)
{
	I2C_EE_BufferWrite((uint8_t *)&E_DeviceType, EE_DeviceType_Addr, 1);
}

void EE_DeviceType_Read(void)
{
	I2C_EE_BufferRead((uint8_t *)&E_DeviceType, EE_DeviceType_Addr, 1);
}

void EE_AnchorWorkMode_Write(void)
{
	I2C_EE_BufferWrite((uint8_t *)&E_AnchorWorkMode, EE_AnchorWorkMode_Addr, 1);
}

void EE_AnchorWorkMode_Read(void)
{
	I2C_EE_BufferRead((uint8_t *)&E_AnchorWorkMode, EE_AnchorWorkMode_Addr, 1);
}

void EE_PowerOnType_Write(void)
{
	I2C_EE_BufferWrite((uint8_t *)&E_PowerOnType, EE_PowerOnType_Addr, 1);
}

void EE_PowerOnType_Read(void)
{
	I2C_EE_BufferRead((uint8_t *)&E_PowerOnType, EE_PowerOnType_Addr, 1);
}

void EE_Network_Write(void)
{
	I2C_EE_BufferWrite((uint8_t *)&E_Network_Manage_stru_temp, EE_Network_Addr, sizeof(E_Network_Manage_stru));
}

void EE_Network_Read(void)
{
	I2C_EE_BufferRead((uint8_t *)&E_Network_Manage_stru_temp, EE_Network_Addr, sizeof(E_Network_Manage_stru));
}





/*********************************************END OF FILE**********************/
