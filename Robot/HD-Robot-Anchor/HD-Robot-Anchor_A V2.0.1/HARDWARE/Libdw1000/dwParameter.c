#include "dwParameter.h"

/* 在基站模式下，配置基站地址0x01-0xff */
/* 在标签模式下，地址为0x00 */
uint8_t UWB_address_temp = 2;
uint8_t address[8] = {1,0,0,0,0,0,0xcf,0xbc};
uint8_t base_address[8] = {0,0,0,0,0,0,0xcf,0xbc};

/* 0 - 1 bytes Header */
/* 2 bytes 设备ID */
/* 3 - 6 bytes距离值 高字节在前，低字节在后 */
/* 7 位Reserve */
/* 8 - 9 bytes ender*/
uint8_t Distance_Msg_buff[10] = {0xFA, 0xFB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF};
volatile uint8_t curr_seq = 0;
double distance;

/* 测距时间戳 */
dwTime_t poll_tx;
dwTime_t poll_rx;
dwTime_t answer_tx;
dwTime_t answer_rx;
dwTime_t final_tx;
dwTime_t final_rx; 

packet_t rxPacket;
packet_t txPacket;

/* 自定义 */
uint32_t timeRangingComplete = 0;
uint8_t rangingComplete = 0;
uint8_t distance_packet_flag = 0;
const double C = 299792458.0;       // Speed of light
const double tsfreq = 499.2e6 * 128;  // Timestamp counter frequency

/* 发送回调函数 */
void txcallback(dwDevice_t *dev)
{
	dwTime_t departure;
	dwGetTransmitTimestamp(dev, &departure);
	switch (txPacket.payload[0]) 
	{
		case POLL:
			//printf("poll_tx\n");
			poll_tx = departure;
			break;
		case ANSWER:
			answer_tx = departure;
			break;
		case FINAL:
			final_tx = departure;
			break;
		case REPORT:
			break;
	}
}

/* 接收回调函数 */

extern distance_SendPackets distance_SendPackets_temp;
extern DistanceOffset_Stru DistanceOffset_Stru_temp;
extern int UWB_DataHandle_Flag;
// 缓存测距数据
uint32_t A_distance_temp[20] = { 0 };
uint32_t B_distance_temp[20] = { 0 };
int A_distance_cnt = 0;
int B_distance_cnt = 0;
uint32_t local_Anchor_Distance = 0;
void rxcallback(dwDevice_t *dev) 
{
	uint8_t temp = 0xFE;
	dwTime_t arival;
	//log_print( DEBUG, ("rxcallback\r\n"));
	memset((uint8_t*)&distance_SendPackets_temp,0,sizeof(distance_SendPackets));
	int dataLength = dwGetDataLength(dev);
	if (dataLength == 0) return;
	memset(&rxPacket,0,MAC802154_HEADER_LENGTH);
	dwGetData(dev, (uint8_t*)&rxPacket, dataLength);
	if(rxPacket.destAddress[0] == temp)
	{  
		if (UWB_DataHandle_Flag == 1)
		{
			memcpy((uint8_t *)&distance_SendPackets_temp,(uint8_t *)&rxPacket.payload,sizeof(distance_SendPackets));
			if((distance_SendPackets_temp.A_distance > 0) && (distance_SendPackets_temp.A_distance < 100000))
				A_distance_temp[A_distance_cnt++] = distance_SendPackets_temp.A_distance;
			//log_print( DEBUG, ("B_distance = %d\r\n", distance_SendPackets_temp.B_distance));
			if((distance_SendPackets_temp.B_distance > 0) && (distance_SendPackets_temp.B_distance < 100000))
				B_distance_temp[B_distance_cnt++] = distance_SendPackets_temp.B_distance;
			//log_print( DEBUG, ("A=%d,B=%d\r\n", distance_SendPackets_temp.A_distance, distance_SendPackets_temp.B_distance));
			
			if(A_distance_cnt == 1)
			{
				DistanceOffset_Stru_temp.tag_id = distance_SendPackets_temp.id_anchor;
				DistanceOffset_Stru_temp.A_anchor_id = distance_SendPackets_temp.A_id_tag;
				DistanceOffset_Stru_temp.B_anchor_id = distance_SendPackets_temp.B_id_tag;
				DistanceOffset_Stru_temp.BatteryVoltage = distance_SendPackets_temp.BatteryVoltage;
			}
			if(ModeTemp == AntDelayCalibration_mode)
			{
				if(distance_SendPackets_temp.A_id_tag == address[0]) 
					local_Anchor_Distance = distance_SendPackets_temp.A_distance;
				if(distance_SendPackets_temp.B_id_tag == address[0]) 
					local_Anchor_Distance = distance_SendPackets_temp.B_distance;
			}
		}
		rangingComplete = 1;
		//udp_UWB_senddata(udppcb, (uint8_t*)&distance_SendPackets_temp, sizeof(distance_SendPackets));
		//USART1_For_Bytes_tx(sizeof(distance_SendPackets), (uint8_t *)&distance_SendPackets_temp);
		dwNewReceive(dev);
		dwStartReceive(dev);
		return;
	}
	else if (memcmp(rxPacket.destAddress, address, 8)) 
	{
		dwNewReceive(dev);
		dwStartReceive(dev);
		return;
	}

	dwGetReceiveTimestamp(dev, &arival);
	memcpy(txPacket.destAddress, rxPacket.sourceAddress, 8);
	memcpy(txPacket.sourceAddress, rxPacket.destAddress, 8);
	switch(rxPacket.payload[0]) 
	{
		//基站接受信息
		case POLL:
		{
			//printf("poll_rx\n");
			poll_rx = arival;
			txPacket.payload[0] = ANSWER;
			txPacket.payload[1] = rxPacket.payload[1];

			dwNewTransmit(dev);
			dwSetData(dev, (uint8_t*)&txPacket, MAC802154_HEADER_LENGTH+2);

			dwWaitForResponse(dev, true);
			dwStartTransmit(dev);
			break;
		}
		case FINAL:
		{
			reportPayload_t *report = (reportPayload_t *)(txPacket.payload+2);
			//printf("final_rx\n");
			final_rx = arival;
			txPacket.payload[0] = REPORT;
			txPacket.payload[1] = rxPacket.payload[1];
			memcpy(&report->pollRx, &poll_rx, 5);
			memcpy(&report->answerTx, &answer_tx, 5);
			memcpy(&report->finalRx, &final_rx, 5);

			dwNewTransmit(dev);
			dwSetData(dev, (uint8_t*)&txPacket, MAC802154_HEADER_LENGTH+2+sizeof(reportPayload_t));

			dwWaitForResponse(dev, true);
			dwStartTransmit(dev);
			break;
		}
		//标签接收信息
		case ANSWER:
		{
			if (rxPacket.payload[1] != curr_seq) 
			{
				return;
			}
			//printf("answer_rx\n");
			answer_rx = arival;
			txPacket.payload[0] = FINAL;
			txPacket.payload[1] = rxPacket.payload[1];
				
			dwNewTransmit(dev);
			dwSetData(dev, (uint8_t*)&txPacket, MAC802154_HEADER_LENGTH+2);

			dwWaitForResponse(dev, true);
			dwStartTransmit(dev);
			break;
		}
		case REPORT:
		{		
			reportPayload_t *report = (reportPayload_t *)(rxPacket.payload+2);
			double tround1, treply1, treply2, tround2, tprop_ctn, tprop;
			if (rxPacket.payload[1] != curr_seq) 
			{
				return;
			}
			memcpy(&poll_rx, &report->pollRx, 5);
			memcpy(&answer_tx, &report->answerTx, 5);
			memcpy(&final_rx, &report->finalRx, 5);
				
			tround1 = answer_rx.low32 - poll_tx.low32;
			treply1 = answer_tx.low32 - poll_rx.low32;
			tround2 = final_rx.low32 - answer_tx.low32;
			treply2 = final_tx.low32 - answer_rx.low32;

			tprop_ctn = ((tround1*tround2) - (treply1*treply2)) / (tround1 + tround2 + treply1 + treply2);

			tprop = tprop_ctn / tsfreq;
			distance = C * tprop;
			//printf("Zanchor %d: %5dmm,\r\n ", rxPacket.sourceAddress[0], (unsigned int)(distance*1000));
			rangingComplete = 1;
			break;
		}
	}
}
