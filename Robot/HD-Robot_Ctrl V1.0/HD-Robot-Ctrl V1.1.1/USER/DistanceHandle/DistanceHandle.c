/*
 * @:*********************************************************************************************************: 
 * @Date: 2020-10-10 16:25:22
 * @LastEditTime: 2020-10-10 16:32:35
 * @**********************************************************************************************************: 
 */
#include "DistanceHandle.h"
#include "math.h"

const u8 udp_HandleErr[12] = "HandleErr\r\n";
uint8_t tim6_irq_flag = 0;
Queue DistanceData_queue;
uint32_t AB_Distance = 0;
uint32_t Pos_ToAAndBMid = 1212;
uint32_t Distance_AntDelay = 1000;
extern uint8_t ModeTemp;

extern distance_SendPackets distance_SendPackets_temp;
DistanceOffset_Stru DistanceOffset_Stru_temp;
double AnchorToAnchor_Distance[3] = { 1000 };
extern DistanceOffset_Stru DistanceOffset_Formflash;
extern AnchorFollowCmd_Stru AnchorFollowCmd_Stru_temp;
Distance_ToAngle_Stru Distance_ToAngle_Stru_temp;
CalibrationMode_Stru CalibrationMode_Stru_temp;

uint32_t A_distance_CheckSum = 0, B_distance_CheckSum = 0;

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
void UWB_DistanceToAngle(Distance_ToAngle_Stru *Distance_ToAngle_Stru_temp)
{
	distance_SendPackets distance_SendPackets_temp;
	static int DistanceCheck_Temp = 0;
	static int Filter_cnt = 0;
	uint8_t str_temp_buf[20] = { 0 };
	char str_temp[40] = { 0 };
	uint32_t Distance_CheckTemp = 0;
	static uint32_t A_distance_CheckTemp = 0, B_distance_CheckTemp = 0;
	double A_distance_OffsetValue = 0, B_distance_OffsetValue = 0;
	uint32_t A_distance_Filter_out = 0, B_distance_Filter_out = 0;
	int A_cnt_temp = 0, B_cnt_temp = 0;
	static uint32_t A_distance_average = 0, B_distance_average = 0;
	static uint32_t A_distance_average_buff = 0, B_distance_average_buff = 0;
	float A_DisToAngle_temp = 0, B_DisToAngle_temp = 0, AB_DisToAngle_temp = 0, A_BX_angle = 0, A_AngleOffset_temp = 0;
	# if 1
	if((A_distance_average > 0) && (B_distance_average > 0))
	{
		A_distance_average_buff = A_distance_average;
		B_distance_average_buff = B_distance_average;
	}
	A_distance_average = Distance_ToAngle_Stru_temp -> A_distance;
	B_distance_average = Distance_ToAngle_Stru_temp -> B_distance;
	if((abs(A_distance_average - A_distance_average_buff) > 1500) || (abs(B_distance_average - B_distance_average_buff) > 1500))
	{
		A_distance_average = 0;
		B_distance_average = 0;
	}
	#endif
	log_print(DEBUG,("A_dis = %d, B_dis = %d\r\n", A_distance_average, B_distance_average));	
	#if 1
	if(ModeTemp == 1)
	{
		log_print(DEBUG,("calibration mode\r\n"));
		if((A_distance_average > 0) && (B_distance_average > 0) && (A_distance_average < 100000) && (B_distance_average < 100000))
		{
			DistanceCheck_Temp++;
			A_distance_CheckSum = A_distance_CheckSum + A_distance_average;
			B_distance_CheckSum = B_distance_CheckSum + B_distance_average;
		}
		if(DistanceCheck_Temp == 20)
		{
			ModeTemp = 0;
			DistanceCheck_Temp = 0;
			A_distance_CheckTemp = A_distance_CheckSum/20;
			B_distance_CheckTemp = B_distance_CheckSum/20;
			A_distance_CheckSum = 0;
			B_distance_CheckSum = 0;
			log_print(DEBUG,("A_distance_CheckTemp = %d\r\n", A_distance_CheckTemp));	
			log_print(DEBUG,("B_distance_CheckTemp = %d\r\n", B_distance_CheckTemp));	
			A_distance_OffsetValue = CalibrationMode_Stru_temp.A_standard_distance - A_distance_CheckTemp;
			B_distance_OffsetValue = CalibrationMode_Stru_temp.B_standard_distance - B_distance_CheckTemp;
			
			if((A_distance_OffsetValue > 500) || (B_distance_OffsetValue > 500))
			{
				memset((uint8_t *)str_temp, 0 ,sizeof(str_temp));
				sprintf(str_temp,"Err,A_distance_OffsetValue > 500\r\n"); // udp_HandleErr
				udp_UWB_senddata(udppcb, (uint8_t*)str_temp, strlen(str_temp));
			}
			else
			{
				if(CalibrationMode_Stru_temp.FollowMode == 0x01)
				{
					log_print(DEBUG,("ahead mode\r\n"));
					DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[0].FollowOffset_Mark = 0x01;
					AB_DisToAngle_temp = AnchorToAnchor_Distance[0];
					DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[0].FollowMode = 0x01;  	
					DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[0].A_distance_offset =  A_distance_OffsetValue;
					DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[0].B_distance_offset =  B_distance_OffsetValue;
					STMFLASH_HeaderDistanceOffset_Write();
				}
				if(CalibrationMode_Stru_temp.FollowMode == 0x02)
				{
					log_print(DEBUG,("left mode\r\n"));
					DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[1].FollowOffset_Mark = 0x01;
					AB_DisToAngle_temp = AnchorToAnchor_Distance[1];
					DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[1].FollowMode = 0x02; 
					DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[1].A_distance_offset =  A_distance_OffsetValue;
					DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[1].B_distance_offset =  B_distance_OffsetValue;
					STMFLASH_LeftDistanceOffset_Write();
				}
				if(CalibrationMode_Stru_temp.FollowMode == 0x03)
				{
					log_print(DEBUG,("right mode\r\n"));
					DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[2].FollowOffset_Mark = 0x01;
					AB_DisToAngle_temp = AnchorToAnchor_Distance[2];
					DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[2].FollowMode = 0x03; 
					DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[2].A_distance_offset =  A_distance_OffsetValue;
					DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[2].B_distance_offset =  B_distance_OffsetValue;
					STMFLASH_RightDistanceOffset_Write();
				}
				//STMFLASH_DistanceOffset_Write();
				memset((uint8_t *)str_temp, 0 ,sizeof(str_temp));
				log_print(DEBUG,("A_offset = %f, B_offset = %f\r\n",A_distance_OffsetValue,B_distance_OffsetValue));
				sprintf(str_temp,"A_offset = %f, B_offset = %f\r\n",A_distance_OffsetValue,B_distance_OffsetValue);
				udp_UWB_senddata(udppcb, (uint8_t*)str_temp, strlen(str_temp));
			}
			//ModeTemp = Receiving_mode;
			A_distance_CheckTemp = 0;
			B_distance_CheckTemp = 0;
		}
	}
	#endif

	#if 1
	DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[0].DistanceToAngle_Mode = 0x00;
	DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[1].DistanceToAngle_Mode = 0x00;
	DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[2].DistanceToAngle_Mode = 0x00;
	if((AnchorFollowCmd_Stru_temp.Follwmode == AheadFollow) && (DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[0].DistanceToAngle_Mode == 0x00))
	{
		A_DisToAngle_temp = A_distance_average + DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[0].A_distance_offset;
		B_DisToAngle_temp = B_distance_average + DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[0].B_distance_offset;
		AB_DisToAngle_temp = AnchorToAnchor_Distance[0];
		A_AngleOffset_temp = DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[0].angle_offset;
	}
	else if((AnchorFollowCmd_Stru_temp.Follwmode == AheadFollow) && (DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[0].DistanceToAngle_Mode == 0x01))
	{
		A_DisToAngle_temp = B_distance_average + DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[0].B_distance_offset;
		B_DisToAngle_temp = A_distance_average + DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[0].A_distance_offset;
		AB_DisToAngle_temp = AnchorToAnchor_Distance[0];
		A_AngleOffset_temp = DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[0].angle_offset;
	}
	else if((AnchorFollowCmd_Stru_temp.Follwmode == LeftFollow) && (DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[1].DistanceToAngle_Mode == 0x00))
	{
		A_DisToAngle_temp = A_distance_average + DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[1].A_distance_offset;
		B_DisToAngle_temp = B_distance_average + DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[1].B_distance_offset;
		AB_DisToAngle_temp = AnchorToAnchor_Distance[1];
		A_AngleOffset_temp = DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[1].angle_offset;
	}
	else if((AnchorFollowCmd_Stru_temp.Follwmode == LeftFollow) && (DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[1].DistanceToAngle_Mode == 0x01))
	{
		A_DisToAngle_temp = B_distance_average + DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[1].B_distance_offset;
		B_DisToAngle_temp = A_distance_average + DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[1].A_distance_offset;
		AB_DisToAngle_temp = AnchorToAnchor_Distance[1];
		A_AngleOffset_temp = DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[1].angle_offset;
	}
	else if((AnchorFollowCmd_Stru_temp.Follwmode == RightFollow) && (DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[2].DistanceToAngle_Mode == 0x00))
	{
		A_DisToAngle_temp = A_distance_average + DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[2].A_distance_offset;
		B_DisToAngle_temp = B_distance_average + DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[2].B_distance_offset;
		AB_DisToAngle_temp = AnchorToAnchor_Distance[2];
		A_AngleOffset_temp = DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[2].angle_offset;
	}
	else if((AnchorFollowCmd_Stru_temp.Follwmode == RightFollow) && (DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[2].DistanceToAngle_Mode == 0x01))
	{
		A_DisToAngle_temp = B_distance_average + DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[2].B_distance_offset;
		B_DisToAngle_temp = A_distance_average + DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[2].A_distance_offset;
		AB_DisToAngle_temp = AnchorToAnchor_Distance[2];
		A_AngleOffset_temp = DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[2].angle_offset;
	}
	memset((uint8_t *)&distance_SendPackets_temp, 0, sizeof(distance_SendPackets));
  distance_SendPackets_temp.id_anchor = Distance_ToAngle_Stru_temp->id_anchor;
  distance_SendPackets_temp.A_id_tag = Distance_ToAngle_Stru_temp->A_id_tag;
  distance_SendPackets_temp.B_id_tag = Distance_ToAngle_Stru_temp->B_id_tag;
	distance_SendPackets_temp.FollowMode = AnchorFollowCmd_Stru_temp.Follwmode;
	distance_SendPackets_temp.BatteryVoltage = Distance_ToAngle_Stru_temp->BatteryVoltage;
	distance_SendPackets_temp.header = 0x4448;
	distance_SendPackets_temp.ender = 0x4225;
	#if 1
	if(((A_DisToAngle_temp + B_DisToAngle_temp) > AB_DisToAngle_temp) && ((A_DisToAngle_temp + AB_DisToAngle_temp) > B_DisToAngle_temp)
				&& ((B_DisToAngle_temp + AB_DisToAngle_temp) > A_DisToAngle_temp))
  {
		 //log_print(DEBUG,("Form a triangle is OK\r\n"));
		 A_BX_angle = (acos(((pow(A_DisToAngle_temp, 2)) + (pow(AB_DisToAngle_temp, 2)) - (pow(B_DisToAngle_temp, 2)))
														/(2*A_DisToAngle_temp*AB_DisToAngle_temp)))*(180.0/M_PI);
		 //A_BX_angle = A_BX_angle + A_AngleOffset_temp;
		 log_print(DEBUG,("Angle = %f, %f, %f\r\n", A_BX_angle,A_DisToAngle_temp, B_DisToAngle_temp));
		 distance_SendPackets_temp.reserved = (uint32_t)(A_BX_angle*1000);
		 //udp_UWB_senddata(udppcb, (uint8_t *)&distance_SendPackets_temp, sizeof(distance_SendPackets));
		 //USART1_For_Bytes_tx(sizeof(distance_SendPackets), (uint8_t *)&distance_SendPackets_temp);
  }
  else
  {
	  distance_SendPackets_temp.reserved = 0;
	  log_print(DEBUG,("Form a triangle is Fail\r\n"));
  }
	
	if((A_distance_average > 0) && (B_distance_average > 0) && (A_DisToAngle_temp > 0) && (A_DisToAngle_temp > 0))
	{
		distance_SendPackets_temp.A_distance = A_DisToAngle_temp;
		distance_SendPackets_temp.B_distance = B_DisToAngle_temp;
	}
	else
	{
		distance_SendPackets_temp.reserved = 0;
		distance_SendPackets_temp.A_distance = 0;
		distance_SendPackets_temp.B_distance = 0;
	}
	#endif
	memcpy(str_temp_buf, (uint8_t *)&distance_SendPackets_temp, sizeof(distance_SendPackets));
	for(int i = 0; i < sizeof(distance_SendPackets); i++)
	{
		sprintf(&str_temp[i*2], "%02x", str_temp_buf[i]);
	}
	USART1_For_Bytes_tx(sizeof(distance_SendPackets)*2, (uint8_t *)&str_temp);
  udp_UWB_senddata(udppcb, (uint8_t *)&str_temp, sizeof(distance_SendPackets)*2);
	#endif
}

// 系统默认参数设置
void Distance_DefaultPara_Config(void)
{
	log_print(DEBUG,("distance Default FollowModeOffset\r\n"));
	for(int i = 0; i < 4; i++)
	{
		DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[i].FollowOffset_Mark = 0x00;
		DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[i].DistanceToAngle_Mode = 0x00;	
		DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[i].angle_offset = 0; 
		DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[i].A_distance_offset = 0;
		DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[i].B_distance_offset = 0;
	}
}




