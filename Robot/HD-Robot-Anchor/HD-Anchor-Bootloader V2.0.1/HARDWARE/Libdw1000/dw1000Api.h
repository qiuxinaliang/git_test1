#ifndef _DW1000API_H_
#define _DW1000API_H_

#include "common.h"

#define rangingOverTime 20 //超过 500ms,normally measure ranging time about 5ms

typedef enum TagWorkingMode
{
	Receiving_mode = 0,
	Monitoring_mode,
	Sleeping_mode,
	Idle_mode
}TagWorkingMode;

extern dwDevice_t dwm_device;
extern dwDevice_t *dwm;
extern packet_t rxPacket;
extern packet_t txPacket;
extern int ModeTemp;

#pragma pack(1)
typedef struct distance_SendPackets
{
  uint16_t header;
  uint8_t id_anchor; 
  uint8_t A_id_tag;  
  uint32_t A_distance;  
	uint8_t B_id_tag;  
  uint32_t B_distance;
  uint32_t reserved; 
  uint16_t ender; 
}distance_SendPackets;
#pragma pack()
extern distance_SendPackets distance_SendPackets_temp;

extern void txcallback(dwDevice_t *dev);
extern void rxcallback(dwDevice_t *dev);
void DW_Parameter_Config(dwDevice_t * dev);
void DW_User_Justable_Config(dwDevice_t * dev);
void DW_AnchorToTag_Distance(dwDevice_t * dev);
void DW_TagEnableReceive_Distance(dwDevice_t* dev);
void DW_TagStateModify_Distance(dwDevice_t* dev);

#endif
