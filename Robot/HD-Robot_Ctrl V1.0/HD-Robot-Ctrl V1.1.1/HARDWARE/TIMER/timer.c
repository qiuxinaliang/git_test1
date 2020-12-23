/*
 * @:*********************************************************************************************************: 
 * @Date: 2020-09-29 16:10:54
 * @LastEditTime: 2020-09-30 13:52:12
 * @**********************************************************************************************************: 
 */
#include "timer.h"
 	
TIM_HandleTypeDef TIM3_Handler;      //定时器句柄 
TIM_HandleTypeDef htim6;
extern u32 lwip_localtime;	         //lwip本地时间计数器,单位:ms
//通用定时器3中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!(定时器3挂在APB1上，时钟为HCLK/2)
void TIM3_Init(u16 arr,u16 psc)
{  
	TIM3_Handler.Instance = TIM3;                          //通用定时器3
	TIM3_Handler.Init.Prescaler = psc;                     //分频
	TIM3_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;    //向上计数器
	TIM3_Handler.Init.Period = arr;                        //自动装载值
	TIM3_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_Base_Init(&TIM3_Handler);
	
	HAL_TIM_Base_Start_IT(&TIM3_Handler); //使能定时器3和定时器3中断   
}

/* TIM6 init function */
void MX_TIM6_Init(void)
{
  TIM_MasterConfigTypeDef sMasterConfig = {0};
	

  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 36000 -1;  // 180M / 2 / 36000 = 2500,表示计数器1s可计数2500次
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 500 -1;  // 计数值为500，2500 / 500 = 5 表示每1s进5次中断
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
void IWDG_Init(u8 prer,u16 rlr)
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
	__HAL_RCC_TIM3_CLK_ENABLE();            //使能TIM3时钟
	HAL_NVIC_SetPriority(TIM3_IRQn,1,0);    //设置中断优先级，抢占优先级1，子优先级3
	HAL_NVIC_EnableIRQ(TIM3_IRQn);          //开启ITM3中断   
	
	__HAL_RCC_TIM6_CLK_ENABLE();

	/* TIM6 interrupt Init */
	HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 2, 1);
	HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
}

//定时器3中断服务函数
void TIM3_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&TIM3_Handler);
}

void TIM6_DAC_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim6);
}

//定时器3中断服务函数调用
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim ==(&TIM3_Handler))
	{
		 lwip_localtime +=10; //加10
	}
	else if(htim ==(&htim6))
	{
		lwip_periodic_handle();
		tim6_irq_flag = true;
		//log_print(DEBUG, ("tim6_irq\r\n"));
	}
}

////////////////////////////////////////////////////////////////////////////////////////

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{
	__HAL_RCC_TIM3_CLK_DISABLE();
	HAL_NVIC_DisableIRQ(TIM3_IRQn);
	__HAL_RCC_TIM6_CLK_DISABLE();
	HAL_NVIC_DisableIRQ(TIM6_DAC_IRQn);
} 

