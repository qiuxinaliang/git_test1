/*
 * @:*********************************************************************************************************: 
 * @Date: 2020-11-03 18:07:17
 * @LastEditTime: 2020-12-04 14:15:03
 * @**********************************************************************************************************: 
 */
#include "board.h"
#include <cm_backtrace.h>

#define u32 uint32_t
#define HARDWARE_VERSION               "V1.0.0"
#define SOFTWARE_VERSION               "V0.1.0"

uint8_t DeviceSelfCheckInfo;

static void SystemClock_Config(void);
static void GPIO_CLK_Init(void);
static void HardwareConfigInfo_Handle(void);

/*
*********************************************************************************************************
*	函 数 名: BSP_Init
*	功能说明: 系统硬件初始化
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
extern Staticgloabl_stru Staticgloabl_stru_temp;
void BSP_Init(void)
{
	uint32_t timestamp = 0;
  HAL_Init();    
  GPIO_CLK_Init();
  SystemClock_Config();    /* 配置系统时钟为180 MHz */ 
	RTC_Init();
  MX_TIM6_Init();
  DEBUG_USART_Config();    /*初始化USART 配置模式为 115200 8-N-1，中断接收*/
  DeviceSelfCheckInfo = 1;

	Lora_SPI_Init();
  lora_init(470000000, 20, 9, 7, 1, 0x0005);
	LED_GPIO_Config();
 
	I2C_EE_Init();
	HardwareConfigInfo_Handle();
	//I2C_Test();
  my_mem_init(SRAMIN);
	rtc_timestamp = 1604489403;
	IWDG_Init(IWDG_PRESCALER_64,1000);  	//分频数为4,重载值为1000,溢出时间为2s	
  
	cm_backtrace_init("HD-Vessel-Anchor", HARDWARE_VERSION, SOFTWARE_VERSION);
  log_print(DEBUG,("name = HD-Vessel-Anchor, HARDWARE_VERSION  = %s, SOFTWARE_VERSION = %s", HARDWARE_VERSION, SOFTWARE_VERSION));
}

/*
*********************************************************************************************************
*	函 数 名:  HardWareConfigInfo_Handle
*	功能说明: 硬件配置信息处理
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void HardwareConfigInfo_Handle(void)
{
  uint32_t temp = 0;
	
	EE_TimeStamp_Read();
	if((E_Staticgloabl_stru_temp.timestamp < 1604489403) || (E_Staticgloabl_stru_temp.timestamp > 4004489403))
	{
		E_Staticgloabl_stru_temp.timestamp = 1604489403;
	}
	EE_Version_Read();
	if((E_Version_stru_temp.HardwareVersion[0] > 10) || (E_Version_stru_temp.SoftwareVersion[0] > 10))
	{
		E_Version_stru_temp.HardwareVersion[0] = 1;
		E_Version_stru_temp.HardwareVersion[1] = 0;
		E_Version_stru_temp.HardwareVersion[2] = 1;
		E_Version_stru_temp.SoftwareVersion[0] = 1;
		E_Version_stru_temp.SoftwareVersion[1] = 4;
		E_Version_stru_temp.SoftwareVersion[2] = 2;
	}
	EE_Network_Read();
	if((E_Network_Manage_stru_temp.Anchoripaddress[0] != E_Network_Manage_stru_temp.Gateway[0]) 
		 || (E_Network_Manage_stru_temp.Anchoripaddress[1] != E_Network_Manage_stru_temp.Gateway[1]) 
	   || (E_Network_Manage_stru_temp.Anchoripaddress[0] == 0xFF) || (E_Network_Manage_stru_temp.Anchoripaddress[1] == 0xFF))
	{
		log_print(DEBUG,("network default config...\r\n"));
		E_Network_Manage_stru_temp.Anchoripaddress[0] = 192;
		E_Network_Manage_stru_temp.Anchoripaddress[1] = 168;
		E_Network_Manage_stru_temp.Anchoripaddress[2] = 1;
		E_Network_Manage_stru_temp.Anchoripaddress[3] = 33;
		E_Network_Manage_stru_temp.AnchorPort = 6008;
		
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
		E_Network_Manage_stru_temp.Centeripaddress[3] = 248;
		E_Network_Manage_stru_temp.CenterPort = 6008;

		E_Network_Manage_stru_temp.Engineipaddress[0] = 192;
		E_Network_Manage_stru_temp.Engineipaddress[1] = 168;
		E_Network_Manage_stru_temp.Engineipaddress[2] = 1;
		E_Network_Manage_stru_temp.Engineipaddress[3] = 248;
		E_Network_Manage_stru_temp.EnginePort = 6008;
	}
	EE_upgrateFlag_Read();
	if(upgrateFlag == 1)
	{
		upgrateFlag = 0;
		EE_upgrateFlag_Write();
	}
	EE_PowerOnType_Read();
	if((E_PowerOnType != PowerOnOpen) ||(E_PowerOnType != PowerOnOpen))
	{
		E_PowerOnType = PowerOnOpen;
	}
	EE_AnchorWorkMode_Read();
	if((E_AnchorWorkMode != UWB_Postion_Mode)||(E_AnchorWorkMode != Lora_Commun_Mode) ||(E_AnchorWorkMode != UWB_Lora_Mode))
	{
		E_AnchorWorkMode = Lora_Commun_Mode;
	}
	log_print(DEBUG,("Static IP........................%d.%d.%d.%d\r\n",E_Network_Manage_stru_temp.Anchoripaddress[0],
	E_Network_Manage_stru_temp.Anchoripaddress[1],E_Network_Manage_stru_temp.Anchoripaddress[2],E_Network_Manage_stru_temp.Anchoripaddress[3]));
	log_print(DEBUG,("netmask..........................%d.%d.%d.%d\r\n",E_Network_Manage_stru_temp.Subnetmask[0],
	E_Network_Manage_stru_temp.Subnetmask[1],E_Network_Manage_stru_temp.Subnetmask[2],E_Network_Manage_stru_temp.Subnetmask[3]));
						
	log_print(DEBUG,("gateway..........................%d.%d.%d.%d\r\n",E_Network_Manage_stru_temp.Gateway[0],
	E_Network_Manage_stru_temp.Gateway[1],E_Network_Manage_stru_temp.Gateway[2],E_Network_Manage_stru_temp.Gateway[3]));
	
	log_print(DEBUG,("Center IP..........................%d.%d.%d.%d\r\n",E_Network_Manage_stru_temp.Centeripaddress[0],
	E_Network_Manage_stru_temp.Centeripaddress[1],E_Network_Manage_stru_temp.Centeripaddress[2],E_Network_Manage_stru_temp.Centeripaddress[3]));
	log_print(DEBUG,("Engine IP..........................%d.%d.%d.%d\r\n",E_Network_Manage_stru_temp.Engineipaddress[0],
	E_Network_Manage_stru_temp.Engineipaddress[1],E_Network_Manage_stru_temp.Engineipaddress[2],E_Network_Manage_stru_temp.Engineipaddress[3]));
  log_print(DEBUG,("LocalPort = %d, CenterPort = %d, EnginePort = %d", E_Network_Manage_stru_temp.AnchorPort, E_Network_Manage_stru_temp.CenterPort,
						E_Network_Manage_stru_temp.EnginePort));	
}


/*
*********************************************************************************************************
*	函 数 名:SystemClock_Config
*	功能说明: 系统时钟配置， 180M  
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  
  __HAL_RCC_PWR_CLK_ENABLE();   /* Enable Power Control clock */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 360;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 8;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    while(1) {}
  }

  HAL_PWREx_EnableOverDrive();     /* 激活 OverDrive 模式 */
  /* 选择PLLCLK作为SYSCLK，并配置 HCLK, PCLK1 and PCLK2 的时钟分频因子 
	 * SYSCLK = PLLCLK     = 180M
	 * HCLK   = SYSCLK / 1 = 180M
	 * PCLK2  = SYSCLK / 2 = 90M
	 * PCLK1  = SYSCLK / 4 = 45M
	 */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    while(1) {}
  }
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/configTICK_RATE_HZ);
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);   /* SysTick_IRQn interrupt configuration */
}
/*
*********************************************************************************************************
*	函 数 名:GPIO_CLK_Init
*	功能说明: GPIO时钟初始化配置 
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void GPIO_CLK_Init(void)
{
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
}

//THUMB指令不支持汇编内联
//采用如下方法实现执行汇编指令WFI  
__asm void WFI_SET(void)
{
	WFI;		  
}
//关闭所有中断(但是不包括fault和NMI中断)
__asm void INTX_DISABLE(void)
{
	CPSID   I
	BX      LR	  
}
//开启所有中断
__asm void INTX_ENABLE(void)
{
	CPSIE   I
	BX      LR  
}
//设置栈顶地址
//addr:栈顶地址
__asm void MSR_MSP(u32 addr) 
{
	MSR MSP, r0 			//set Main Stack value
	BX r14
}

void assert_failed(uint8_t* file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* Infinite loop */
    cm_backtrace_assert(cmb_get_sp());
    printf("assert failed at %s:%d \n", file, line);
    while (1) {
    }
}

