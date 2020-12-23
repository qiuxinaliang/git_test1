#ifndef _DWPARAMETER_H_
#define _DWPARAMETER_H_


#include "libdw1000Types.h"
#include "mac.h"
#include "common.h"

/* 天线延时设置 */
#define ANTENNA_OFFSET 154.6   // In meter
#define ANTENNA_DELAY  (ANTENNA_OFFSET*499.2e6*128)/299792458.0 // In radio tick

/* 四个测距数据包 */
#define POLL 0x01   
#define ANSWER 0x02
#define FINAL 0x03
#define REPORT 0x04 

extern uint8_t address[8];
extern uint8_t base_address[8];
extern volatile uint8_t curr_seq;

extern uint32_t A_distance_temp[20];
extern uint32_t B_distance_temp[20];
extern int A_distance_cnt;
extern int B_distance_cnt;

/* 报告距离 */
typedef struct	
{
	uint8_t tagAddress[8];
	uint8_t anchorAddress[8]; 
	uint32_t distance;
} __attribute__ ((packed)) reportDistance_t;

/* 报告负载信息 */
typedef struct 
{
	uint8_t pollRx[5];
    uint8_t answerTx[5];
    uint8_t finalRx[5];
} __attribute__ ((packed)) reportPayload_t ;


/* 时间戳信息 */
typedef union timestamp_u 
{
	uint8_t raw[5];
	uint64_t full;
	struct 
	{
		uint32_t low32;
		uint8_t high8;
	} __attribute__ ((packed));
	struct 
	{
		uint8_t low8;
		uint32_t high32;
	} __attribute__ ((packed));
}__attribute__ ((packed)) timestamp_t;

extern uint8_t distance_packet_flag;
/* 自定义变量 */
extern double distance;
extern uint32_t local_Anchor_Distance;

/* 测距时间戳 */
extern dwTime_t poll_tx;
extern dwTime_t poll_rx;
extern dwTime_t answer_tx;
extern dwTime_t answer_rx;
extern dwTime_t final_tx;
extern dwTime_t final_rx;

extern packet_t rxPacket;
extern packet_t txPacket;

/* 自定义 */
extern uint32_t timeRangingComplete;
extern uint8_t rangingComplete;

extern void txcallback(dwDevice_t *dev);
extern void rxcallback(dwDevice_t *dev); 
#endif

