/*
 * @:*********************************************************************************************************: 
 * @Date: 2020-11-12 17:04:06
 * @LastEditTime: 2020-11-12 18:14:58
 * @**********************************************************************************************************: 
 */
#include "time.h"
TIM_HandleTypeDef htim6;

/* TIM6 init function */
void MX_TIM6_Init(void)
{
  TIM_MasterConfigTypeDef sMasterConfig = {0};
	

  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 36000 -1;  // 180M / 2 / 36000 = 2500,表示计数器1s可计数2500次
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 2500 -1;  // 计数值为500，2500 / 2500 = 1 表示每1s进1次中断
  //htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    //Error_Handler();
  }
	HAL_TIM_Base_Start_IT(&htim6); //使能定时器3和定时器3中断 
}

/*
*******************************************************************************
*	函 数 名:  IWDG_Init
*	功能说明: 初始化独立看门狗
*	形	  参: prer:分频数:IWDG_PRESCALER_4~IWDG_PRESCALER_256,自动重装载值,0~0XFFF
*	返 回 值: 无(时间计算(大概):Tout=((4*2^prer)*rlr)/32 (ms))
*******************************************************************************
*/
IWDG_HandleTypeDef IWDG_Handler; //独立看门狗句柄
void IWDG_Init(uint8_t prer,uint16_t rlr)
{
	IWDG_Handler.Instance=IWDG;
	IWDG_Handler.Init.Prescaler=prer;	//设置IWDG分频系数
	IWDG_Handler.Init.Reload=rlr;		//重装载值
	HAL_IWDG_Init(&IWDG_Handler);		//初始化IWDG  

	HAL_IWDG_Start(&IWDG_Handler);		//开启独立看门狗
}
    
//喂独立看门狗
void IWDG_Feed(void)
{   
	HAL_IWDG_Refresh(&IWDG_Handler); 	//喂狗
}

//定时器底册驱动，开启时钟，设置中断优先级
//此函数会被HAL_TIM_Base_Init()函数调用
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
	__HAL_RCC_TIM6_CLK_ENABLE();

	/* TIM6 interrupt Init */
	HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 2, 1);
	HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
}

void TIM6_DAC_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim6);
}

//定时器3中断服务函数调用
uint8_t tim6_irq_flag = 0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

  if(htim ==(&htim6))
	{
		tim6_irq_flag = 1;
		//log_print(DEBUG, ("tim6_irq\r\n"));
	}
}

////////////////////////////////////////////////////////////////////////////////////////

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{
	__HAL_RCC_TIM6_CLK_DISABLE();
	HAL_NVIC_DisableIRQ(TIM6_DAC_IRQn);
} 

