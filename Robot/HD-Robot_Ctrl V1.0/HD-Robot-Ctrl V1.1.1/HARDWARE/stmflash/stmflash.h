/*
 * @:*********************************************************************************************************: 
 * @Date: 2020-05-25 16:58:51
 * @LastEditTime: 2020-10-12 09:50:04
 * @**********************************************************************************************************: 
 */
#ifndef __STMFLASH_H
#define __STMFLASH_H
#include "common.h"

extern uint32_t UpgradeFlag;
//FLASH��ʼ��ַ
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH����ʼ��ַ
#define FLASH_WAITETIME  50000          //FLASH�ȴ���ʱʱ��

//FLASH ��������ʼ��ַ
#define ADDR_FLASH_SECTOR_0     ((u32)0x08000000) 	//����0��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_1     ((u32)0x08004000) 	//����1��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_2     ((u32)0x08008000) 	//����2��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_3     ((u32)0x0800C000) 	//����3��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_4     ((u32)0x08010000) 	//����4��ʼ��ַ, 64 Kbytes  
#define ADDR_FLASH_SECTOR_5     ((u32)0x08020000) 	//����5��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_6     ((u32)0x08040000) 	//����6��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_7     ((u32)0x08060000) 	//����7��ʼ��ַ, 128 Kbytes  

#define ADDR_FLASH_SECTOR_8     ((u32)0x08080000) 	//����8��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_9     ((u32)0x080A0000) 	//����9��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_10    ((u32)0x080C0000) 	//����10��ʼ��ַ,128 Kbytes  
#define ADDR_FLASH_SECTOR_11    ((u32)0x080E0000) 	//����11��ʼ��ַ,128 Kbytes 

#define ADDR_FLASH_SECTOR_12	((u32)0x08100000) 	//����12��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_13	((u32)0x08104000) 	//����13��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_14    ((u32)0x08108000) 	//����14��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_15	((u32)0x0810C000) 	//����15��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_16    ((u32)0x08110000) 	//����16��ʼ��ַ, 64 Kbytes  
#define ADDR_FLASH_SECTOR_17	((u32)0x08120000) 	//����17��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_18	((u32)0x08140000) 	//����18��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_19	((u32)0x08160000) 	//����19��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_20    ((u32)0x08180000) 	//����20��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_21	((u32)0x081A0000) 	//����21��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_22    ((u32)0x081C0000) 	//����22��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_23    ((u32)0x081E0000) 	//����23��ʼ��ַ, 128 Kbytes   

//extern DistanceOffset_Stru DistanceOffset_Stru_temp; 
#pragma pack(8)
typedef struct
{
	uint8_t FollowOffset_Mark;
	uint8_t FollowMode;
	uint8_t DistanceToAngle_Mode;
	double A_distance_offset;
	double B_distance_offset;
	double angle_offset;
}FollowModeOffset_Stru;
#pragma pack()


#pragma pack(8)
typedef struct DistanceOffset_Stru
{
	uint8_t tag_id;
	uint8_t A_anchor_id;
	uint8_t B_anchor_id;
	FollowModeOffset_Stru FollowModeOffset_Stru_temp[3];
	uint8_t Anchors_Distance_Mark;
	double AnchorToAnchor_Distance[3];  //��������࣬�Ҳ��վ֮�����ֵ
}DistanceOffset_Stru;
#pragma pack()

#pragma pack(8)  
typedef struct 
{
	uint8_t net_mark;
	uint8_t remoteip[4];	//Զ������IP��ַ 
	uint16_t remote_port;
	uint8_t ip[4];       //����IP��ַ
	uint16_t local_port;
}Network_Manage;
#pragma pack()

#pragma pack(8)
typedef struct STMFlashReadWrite_Stru
{
	uint8_t UpgradeFlag; // ����״̬��־
  DistanceOffset_Stru DistanceOffset_Stru_temp; //��ಹ��ֵ����
	Network_Manage Network_Manage_temp; // �����������
}STMFlashReadWrite_Stru;
#pragma pack()
 
//extern Network_Manage Network_Manage_buf;
u32 STMFLASH_ReadWord(u32 faddr);		  	//������  
void STMFLASH_Write(u32 WriteAddr,u32 *pBuffer,u32 NumToWrite);		//��ָ����ַ��ʼд��ָ�����ȵ�����
void STMFLASH_Read(u32 ReadAddr,u32 *pBuffer,u32 NumToRead);   		//��ָ����ַ��ʼ����ָ�����ȵ�����
void Test_Write(u32 WriteAddr,u32 WriteData); 
void Upgrade_StatusWrite(void);
void Upgrade_CleanWrite(void);
void STMFLASH_HeaderDistanceOffset_Write(void);
void STMFLASH_LeftDistanceOffset_Write(void);
void STMFLASH_RightDistanceOffset_Write(void); 
//void STMFLASH_DistanceOffset_Write(void);
int Upgrade_Query(void);
void STMFLASH_DistanceOffset_Read(void);   
void STMFLASH_Anchors_Distance_Write(uint8_t Anchors_Distance_Mark);
uint8_t STMFLASH_Anchors_Distance_Read(void);
void STMFLASH_NetworkPara_Config_Write(Network_Manage Network_Manage_pbuf);
uint8_t STMFLASH_NetworkPara_Config_Read(void);
void STMFLASH_Write_test(void);
#endif
