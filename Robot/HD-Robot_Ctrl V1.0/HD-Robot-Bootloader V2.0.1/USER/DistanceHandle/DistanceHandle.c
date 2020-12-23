/*
 * @:*********************************************************************************************************: 
 * @Date: 2020-10-10 16:25:22
 * @LastEditTime: 2020-10-10 16:32:35
 * @**********************************************************************************************************: 
 */
#include "DistanceHandle.h"

uint8_t tim6_irq_flag = 0;
Queue DistanceData_queue;

#define DistanceData_queue_INIT(queue_size)\
{\
	DistanceData_queue.item = NULL;\
	init_queue(&DistanceData_queue,queue_size);\
}

//2. 以高度为例 定义卡尔曼结构体并初始化参数
KFP A_KFP_height={0.02,0,0,0,0.002,0.043};
KFP B_KFP_height={0.02,0,0,0,0.002,0.043};

/**
 * @:*********************************************************************************************************: 
 * @函 数 名: kalmanFilter
 * @功能说明: 卡尔曼滤波器
 * @形    参: KFP *kfp 卡尔曼结构体参数
 * @返 回 值: 滤波后的参数（最优值）
 * @**********************************************************************************************************: 
 */
static double kalmanFilter(KFP *kfp,double input)
{
	 //预测协方差方程：k时刻系统估算协方差 = k-1时刻的系统协方差 + 过程噪声协方差
	 kfp->Now_P = kfp->LastP + kfp->Q;
	 //卡尔曼增益方程：卡尔曼增益 = k时刻系统估算协方差 / （k时刻系统估算协方差 + 观测噪声协方差）
	 kfp->Kg = kfp->Now_P / (kfp->Now_P + kfp->R);
	 //更新最优值方程：k时刻状态变量的最优值 = 状态变量的预测值 + 卡尔曼增益 * （测量值 - 状态变量的预测值）
	 kfp->out = kfp->out + kfp->Kg * (input -kfp->out);//因为这一次的预测值就是上一次的输出值
	 //更新协方差方程: 本次的系统协方差付给 kfp->LastP 威下一次运算准备。
	 kfp->LastP = (1-kfp->Kg) * kfp->Now_P;
	 return kfp->out;
}
/**
 * @:*********************************************************************************************************: 
 * @函 数 名: UWB_DistanceToAngle
 * @功能说明: 测距数据处理，生成角度值
 * @形    参: 
 * @返 回 值: 
 * @**********************************************************************************************************: 
 */
void UWB_DistanceToAngle(void)
{
	uint32_t A_distance_buf[20] = { 0 };
	uint32_t B_distance_buf[20] = { 0 };
	uint32_t A_distance_max = 0, A_distance_min = 0;
	uint32_t B_distance_max = 0, B_distance_min = 0;
	uint32_t A_distance_sum = 0, B_distance_sum = 0;
	uint32_t A_distance_average = 0, B_distance_average = 0;
	uint32_t A_distance_Filter_out = 0, B_distance_Filter_out = 0;
	int A_cnt_temp = 0, B_cnt_temp = 0;
	
	if(tim6_irq_flag == true)
	{
		tim6_irq_flag = false;
		memcpy((uint32_t *)A_distance_buf, (uint32_t *)A_distance_temp, 20*4);
		memcpy((uint32_t *)A_distance_buf, (uint32_t *)A_distance_temp, 20*4);
		memset((uint32_t *)A_distance_temp, 0, 20*4);
		memset((uint32_t *)B_distance_temp, 0, 20*4);
		A_cnt_temp = A_distance_cnt;
		B_cnt_temp = B_distance_cnt;
		A_distance_cnt = 0;
		B_distance_cnt = 0;
		// 先去掉一个最大值和最小值
		A_distance_max = A_distance_buf[0];
		A_distance_min = A_distance_buf[0];
		B_distance_max = B_distance_buf[0];
		B_distance_min = B_distance_buf[0];
		if(A_cnt_temp > 2)
		{
		
			for(int i = 0; i < A_cnt_temp; i++)
			{
				if(A_distance_max < A_distance_buf[i])
				{
					A_distance_max = A_distance_buf[i];
				}
				if(A_distance_min > A_distance_buf[i])
				{
					A_distance_min = A_distance_buf[i];
				}
				A_distance_sum = A_distance_sum + A_distance_buf[i];
			}
			A_distance_average = (A_distance_sum - A_distance_max - A_distance_min)/(A_cnt_temp - 2);
		}
		else if(A_cnt_temp == 1)
		{
			A_distance_average = A_distance_buf[0];
		}
		else
		{
			A_distance_average = 0;
		}
		///////////////////////////////////////////////////////////////////////////////////////////
		if(B_cnt_temp > 2)
		{
		
			for(int i = 0; i < B_cnt_temp; i++)
			{
				if(B_distance_max < B_distance_buf[i])
				{
					B_distance_max = B_distance_buf[i];
				}
				if(B_distance_min > B_distance_buf[i])
				{
					B_distance_min = B_distance_buf[i];
				}
				B_distance_sum = B_distance_sum + B_distance_buf[i];
			}
			B_distance_average = (B_distance_sum - B_distance_max - B_distance_min)/(B_cnt_temp - 2);
		}
		else if(B_cnt_temp == 1)
		{
			B_distance_average = B_distance_buf[0];
		}
		else
		{
			B_distance_average = 0;
		}
		A_distance_Filter_out = (uint32_t)kalmanFilter(&A_KFP_height,(double)A_distance_average);
		B_distance_Filter_out = (uint32_t)kalmanFilter(&B_KFP_height,(double)B_distance_average);
		printf("A_distance = %d\r\n", A_distance_Filter_out);
		//printf("B_distance = %d\r\n", B_distance_Filter_out);
	}
}

