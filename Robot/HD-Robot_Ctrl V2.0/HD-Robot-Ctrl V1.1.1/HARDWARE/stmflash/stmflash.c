#include "stmflash.h"

uint32_t UpgradeFlag;
/*
*******************************************************************************
*	�� �� ��:  STMFLASH_ReadWord
*	����˵��: ��ȡָ����ַ����(32λ����)
*	��	  ��: faddr:����ַ
*	�� �� ֵ: ��Ӧ����
*******************************************************************************
*/
u32 STMFLASH_ReadWord(u32 faddr)
{
	return *(__IO uint32_t *)faddr; 
}

/*
*******************************************************************************
*	�� �� ��:  STMFLASH_GetFlashSector
*	����˵��: ��ȡĳ����ַ���ڵ�flash����
*	��	  ��: addr:flash��ַ
*	�� �� ֵ: 0~11,��addr���ڵ�����
*******************************************************************************
*/
u8 STMFLASH_GetFlashSector(u32 addr)
{
	if(addr<ADDR_FLASH_SECTOR_1)return FLASH_SECTOR_0;
	else if(addr<ADDR_FLASH_SECTOR_2)return FLASH_SECTOR_1;
	else if(addr<ADDR_FLASH_SECTOR_3)return FLASH_SECTOR_2;
	else if(addr<ADDR_FLASH_SECTOR_4)return FLASH_SECTOR_3;
	else if(addr<ADDR_FLASH_SECTOR_5)return FLASH_SECTOR_4;
	else if(addr<ADDR_FLASH_SECTOR_6)return FLASH_SECTOR_5;
	else if(addr<ADDR_FLASH_SECTOR_7)return FLASH_SECTOR_6;
	else if(addr<ADDR_FLASH_SECTOR_8)return FLASH_SECTOR_7;
	else if(addr<ADDR_FLASH_SECTOR_9)return FLASH_SECTOR_8;
	else if(addr<ADDR_FLASH_SECTOR_10)return FLASH_SECTOR_9;
	else if(addr<ADDR_FLASH_SECTOR_11)return FLASH_SECTOR_10; 
	else if(addr<ADDR_FLASH_SECTOR_12)return FLASH_SECTOR_11; 
	else if(addr<ADDR_FLASH_SECTOR_13)return FLASH_SECTOR_12; 
	else if(addr<ADDR_FLASH_SECTOR_14)return FLASH_SECTOR_13; 
	else if(addr<ADDR_FLASH_SECTOR_15)return FLASH_SECTOR_14; 
	else if(addr<ADDR_FLASH_SECTOR_16)return FLASH_SECTOR_15; 
	else if(addr<ADDR_FLASH_SECTOR_17)return FLASH_SECTOR_16; 
	else if(addr<ADDR_FLASH_SECTOR_18)return FLASH_SECTOR_17; 
	else if(addr<ADDR_FLASH_SECTOR_19)return FLASH_SECTOR_18; 
	else if(addr<ADDR_FLASH_SECTOR_20)return FLASH_SECTOR_19; 
	else if(addr<ADDR_FLASH_SECTOR_21)return FLASH_SECTOR_20; 
	else if(addr<ADDR_FLASH_SECTOR_22)return FLASH_SECTOR_21; 
	else if(addr<ADDR_FLASH_SECTOR_23)return FLASH_SECTOR_22;  
	return FLASH_SECTOR_23;	
}

/*
*******************************************************************************
*	�� �� ��:  STMFLASH_Write
*	����˵��: ��ָ����ַ��ʼд��ָ�����ȵ�����
* �ر�ע��:��ΪSTM32F4������ʵ��̫��,û�취���ر�����������,���Ա�����
*         д��ַ�����0XFF,��ô���Ȳ������������Ҳ�������������.����
*         д��0XFF�ĵ�ַ,�����������������ݶ�ʧ.����д֮ǰȷ��������
*         û����Ҫ����,��������������Ȳ�����,Ȼ����������д. 
*         �ú�����OTP����Ҳ��Ч!��������дOTP��!
*         OTP�����ַ��Χ:0X1FFF7800~0X1FFF7A0F(ע�⣺���16�ֽڣ�����OTP���ݿ�����������д����)
*	��	  ��: WriteAddr:��ʼ��ַ(�˵�ַ����Ϊ4�ı���!!),pBuffer:����ָ��,NumToWrite:��(32λ)
*           ��(����Ҫд���32λ���ݵĸ���.) 
*	�� �� ֵ: ��
*******************************************************************************
*/
void STMFLASH_Write(u32 WriteAddr,u32 *pBuffer,u32 NumToWrite)	
{ 
	__set_PRIMASK(1);	
	FLASH_EraseInitTypeDef FlashEraseInit;
	HAL_StatusTypeDef FlashStatus=HAL_OK;
	u32 SectorError=0;
	u32 addrx=0;
	u32 endaddr=0;	
  if(WriteAddr<STM32_FLASH_BASE||WriteAddr%4)return;	//�Ƿ���ַ
    
 	HAL_FLASH_Unlock();             //����	
	addrx=WriteAddr;				//д�����ʼ��ַ
	endaddr=WriteAddr+NumToWrite*4;	//д��Ľ�����ַ
    
	if(addrx<0X1FFF0000)
	{
		while(addrx<endaddr)		//ɨ��һ���ϰ�.(�Է�FFFFFFFF�ĵط�,�Ȳ���)
		{
			if(STMFLASH_ReadWord(addrx)!=0XFFFFFFFF)//�з�0XFFFFFFFF�ĵط�,Ҫ�����������
			{   
				FlashEraseInit.TypeErase=FLASH_TYPEERASE_SECTORS;       //�������ͣ��������� 
				FlashEraseInit.Sector=STMFLASH_GetFlashSector(addrx);   //Ҫ����������
				FlashEraseInit.NbSectors=1;                             //һ��ֻ����һ������
				FlashEraseInit.VoltageRange=FLASH_VOLTAGE_RANGE_3;      //��ѹ��Χ��VCC=2.7~3.6V֮��!!
				if(HAL_FLASHEx_Erase(&FlashEraseInit,&SectorError)!=HAL_OK) 
				{
					break;//����������	
				}
			}
			else addrx+=4;
			FLASH_WaitForLastOperation(FLASH_WAITETIME);                //�ȴ��ϴβ������
		}
	}
	FlashStatus=FLASH_WaitForLastOperation(FLASH_WAITETIME);            //�ȴ��ϴβ������
	if(FlashStatus==HAL_OK)
	{
		while(WriteAddr<endaddr)//д����
		{
			if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,WriteAddr,*pBuffer)!=HAL_OK)//д������
			{ 
				break;	//д���쳣
			}
			WriteAddr+=4;
			pBuffer++;
		} 
	}
	HAL_FLASH_Lock();           //����
	__set_PRIMASK(0);	
} 

/*
*******************************************************************************
*	�� �� ��:  STMFLASH_Read
*	����˵��: ��ָ����ַ��ʼ����ָ�����ȵ�����
*	��	  ��: ReadAddr:��ʼ��ַ,pBuffer:����ָ��,NumToRead:��(32λ)��
*	�� �� ֵ: ��
*******************************************************************************
*/
void STMFLASH_Read(u32 ReadAddr,u32 *pBuffer,u32 NumToRead)   	
{
	__set_PRIMASK(1);	
	u32 i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadWord(ReadAddr);//��ȡ4���ֽ�.
		ReadAddr+=4;//ƫ��4���ֽ�.	
	}
	__set_PRIMASK(0);	
}

// ���Ժ���
void Test_Write(u32 WriteAddr,u32 WriteData)   	
{
	char test[50] = "stmflash test read and write\n";
	char test1[50] = { 0 };
	//char *testdata = NULL;
	//testdata = test;
	//*testdata = "stmflash test read and write\n";
	STMFLASH_Write(ADDR_FLASH_SECTOR_7 ,(uint32_t *)test,sizeof(test)/4);//д��һ���� 
	delay_ms(2000);
	memset((char *)test1, 0, 50);
	STMFLASH_Read(ADDR_FLASH_SECTOR_7,(uint32_t *)test1,sizeof(test)/4); 
	log_print( DEBUG, ("test  = %s\r\n", (char *)test1));
	//STMFLASH_Write(WriteAddr,&WriteData,1);//д��һ���� 
}

extern DistanceOffset_Stru DistanceOffset_Stru_temp;
void Upgrade_StatusWrite(void)   	
{
	STMFlashReadWrite_Stru STMFlashReadWrite_Stru_temp;
	memset((uint32_t *)&STMFlashReadWrite_Stru_temp, 0, sizeof(STMFlashReadWrite_Stru));
	STMFLASH_Read(ADDR_FLASH_SECTOR_7,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru_temp)/4); 
	STMFlashReadWrite_Stru_temp.UpgradeFlag = 0x24;
	STMFLASH_Write(ADDR_FLASH_SECTOR_7 ,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru)/4);//д��һ���� 
	log_print( DEBUG, ("UpgradeFlag write to flash \r\n"));
}

void Upgrade_CleanWrite(void)   	
{
	STMFlashReadWrite_Stru STMFlashReadWrite_Stru_temp;
	memset((uint32_t *)&STMFlashReadWrite_Stru_temp, 0, sizeof(STMFlashReadWrite_Stru));
	STMFLASH_Read(ADDR_FLASH_SECTOR_7,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru_temp)/4); 
	log_print( DEBUG, ("UpgradeFlag = %x \r\n", STMFlashReadWrite_Stru_temp.UpgradeFlag));
	STMFlashReadWrite_Stru_temp.UpgradeFlag = 0x00;
	STMFLASH_Write(ADDR_FLASH_SECTOR_7 ,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru)/4);//д��һ���� 
	
}

int Upgrade_Query(void)   	
{
	uint32_t read_temp = 0;
	STMFlashReadWrite_Stru STMFlashReadWrite_Stru_temp;
	memset((uint32_t *)&STMFlashReadWrite_Stru_temp, 0, sizeof(STMFlashReadWrite_Stru));
	STMFLASH_Read(ADDR_FLASH_SECTOR_7,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru)/4); 
	STMFlashReadWrite_Stru_temp.UpgradeFlag = 0;
	STMFLASH_Write(ADDR_FLASH_SECTOR_7 ,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru)/4);//д��һ���� 
	log_print( DEBUG, ("read UpgradeFlag\r\n"));
	return read_temp;
}


/*
*******************************************************************************
*	�� �� ��:  STMFLASH_HeaderDistanceOffset_Write
*	����˵��: ǰ����ģʽ������������
*	��	  ��: ��
*	�� �� ֵ: ��
*******************************************************************************
*/
void STMFLASH_HeaderDistanceOffset_Write(void)   	
{
	STMFlashReadWrite_Stru STMFlashReadWrite_Stru_temp;
	memset((uint32_t *)&STMFlashReadWrite_Stru_temp, 0, sizeof(STMFlashReadWrite_Stru));
	STMFLASH_Read(ADDR_FLASH_SECTOR_7,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru)/4); 
	memcpy((uint8_t *)&STMFlashReadWrite_Stru_temp.DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[0],(uint8_t *)&DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[0], 
	                                                                        sizeof(FollowModeOffset_Stru));
	STMFLASH_Write(ADDR_FLASH_SECTOR_7 ,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru)/4);//д��һ���� 
}

/* �������벹���������� */
void STMFLASH_LeftDistanceOffset_Write(void)   	
{
	STMFlashReadWrite_Stru STMFlashReadWrite_Stru_temp;
	memset((uint32_t *)&STMFlashReadWrite_Stru_temp, 0, sizeof(STMFlashReadWrite_Stru));
	STMFLASH_Read(ADDR_FLASH_SECTOR_7,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru)/4); 
	memcpy((uint8_t *)&STMFlashReadWrite_Stru_temp.DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[1],(uint8_t *)&DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[1], 
	                                                                        sizeof(FollowModeOffset_Stru));
	STMFLASH_Write(ADDR_FLASH_SECTOR_7 ,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru)/4);//д��һ���� 
}
/* �Ҹ�����벹���������� */
void STMFLASH_RightDistanceOffset_Write(void)   	
{
	STMFlashReadWrite_Stru STMFlashReadWrite_Stru_temp;
	memset((uint32_t *)&STMFlashReadWrite_Stru_temp, 0, sizeof(STMFlashReadWrite_Stru));
	STMFLASH_Read(ADDR_FLASH_SECTOR_7,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru)/4); 
	memcpy((uint8_t *)&STMFlashReadWrite_Stru_temp.DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[2],(uint8_t *)&DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[2], 
	                                                                        sizeof(FollowModeOffset_Stru));
	STMFLASH_Write(ADDR_FLASH_SECTOR_7 ,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru)/4);//д��һ���� 
}

#if 0
void STMFLASH_DistanceOffset_Write(void)   	
{
	STMFlashReadWrite_Stru STMFlashReadWrite_Stru_temp;
	memset((uint32_t *)&STMFlashReadWrite_Stru_temp, 0, sizeof(STMFlashReadWrite_Stru));
	STMFLASH_Read(ADDR_FLASH_SECTOR_7,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru)/4); 
	memcpy((DistanceOffset_Stru *)&STMFlashReadWrite_Stru_temp.DistanceOffset_Stru_temp,(DistanceOffset_Stru *)&DistanceOffset_Stru_temp, sizeof(DistanceOffset_Stru));
	STMFLASH_Write(ADDR_FLASH_SECTOR_7 ,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru)/4);//д��һ���� 
}
#endif

void STMFLASH_DistanceOffset_Read(void)   	
{
	STMFlashReadWrite_Stru STMFlashReadWrite_Stru_temp;
	memset((uint32_t *)&STMFlashReadWrite_Stru_temp, 0, sizeof(STMFlashReadWrite_Stru));
	STMFLASH_Read(ADDR_FLASH_SECTOR_7,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru)/4); 
	memcpy((uint8_t *)&DistanceOffset_Stru_temp, (uint8_t *)&STMFlashReadWrite_Stru_temp.DistanceOffset_Stru_temp, sizeof(DistanceOffset_Stru));
	log_print( DEBUG, ("A_HOff = %f, B = %f\r\n",DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[0].A_distance_offset,
	                                                     DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[0].B_distance_offset));
	log_print( DEBUG, ("A_LOff = %f, B = %f\r\n",DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[1].A_distance_offset,
	                                                     DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[1].B_distance_offset));
	log_print( DEBUG, ("A_ROff = %f, B = %f\r\n",DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[2].A_distance_offset,
	                                                     DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[2].B_distance_offset));
}

/* һ����д���������� */
void STMFLASH_Anchors_Distance_Write(uint8_t Anchors_Distance_Mark)   	
{
	STMFlashReadWrite_Stru STMFlashReadWrite_Stru_temp;
	memset((uint32_t *)&STMFlashReadWrite_Stru_temp, 0, sizeof(STMFlashReadWrite_Stru));
	STMFLASH_Read(ADDR_FLASH_SECTOR_7,(uint32_t *)&STMFlashReadWrite_Stru_temp, sizeof(STMFlashReadWrite_Stru)/4); 
	//DistanceOffset_Stru_temp.AnchorToAnchor_Distance[0] = AnchorToAnchor_Distance[0];
	//DistanceOffset_Stru_temp.AnchorToAnchor_Distance[1] = AnchorToAnchor_Distance[1];
	//DistanceOffset_Stru_temp.AnchorToAnchor_Distance[2] = AnchorToAnchor_Distance[2];
	DistanceOffset_Stru_temp.Anchors_Distance_Mark = Anchors_Distance_Mark;
	STMFlashReadWrite_Stru_temp.DistanceOffset_Stru_temp.AnchorToAnchor_Distance[0] = AnchorToAnchor_Distance[0];
	STMFlashReadWrite_Stru_temp.DistanceOffset_Stru_temp.AnchorToAnchor_Distance[1] = AnchorToAnchor_Distance[1];
	STMFlashReadWrite_Stru_temp.DistanceOffset_Stru_temp.AnchorToAnchor_Distance[2] = AnchorToAnchor_Distance[2];
	//memcpy((DistanceOffset_Stru *)&STMFlashReadWrite_Stru_temp.DistanceOffset_Stru_temp, (DistanceOffset_Stru *)&DistanceOffset_Stru_temp, sizeof(DistanceOffset_Stru));
	STMFLASH_Write(ADDR_FLASH_SECTOR_7 ,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru)/4);//д��һ���� 
}

uint8_t STMFLASH_Anchors_Distance_Read(void)   	
{
	uint8_t Anchors_Distance_Mark;
	STMFlashReadWrite_Stru STMFlashReadWrite_Stru_temp;
	memset((uint32_t *)&STMFlashReadWrite_Stru_temp, 0, sizeof(STMFlashReadWrite_Stru));
	STMFLASH_Read(ADDR_FLASH_SECTOR_7,(uint32_t *)&STMFlashReadWrite_Stru_temp, sizeof(STMFlashReadWrite_Stru)/4); 
	AnchorToAnchor_Distance[0] = STMFlashReadWrite_Stru_temp.DistanceOffset_Stru_temp.AnchorToAnchor_Distance[0];
	AnchorToAnchor_Distance[1] = STMFlashReadWrite_Stru_temp.DistanceOffset_Stru_temp.AnchorToAnchor_Distance[1];
	AnchorToAnchor_Distance[2] = STMFlashReadWrite_Stru_temp.DistanceOffset_Stru_temp.AnchorToAnchor_Distance[2];
	Anchors_Distance_Mark = STMFlashReadWrite_Stru_temp.DistanceOffset_Stru_temp.Anchors_Distance_Mark;
	return Anchors_Distance_Mark;
}

// ���������д��
extern Network_Manage *Network_Manage_pbuf;
void STMFLASH_NetworkPara_Config_Write(Network_Manage Network_Manage_pbuf)
{
	STMFlashReadWrite_Stru STMFlashReadWrite_Stru_temp;
	memset((uint32_t *)&STMFlashReadWrite_Stru_temp, 0, sizeof(STMFlashReadWrite_Stru));
	STMFLASH_Read(ADDR_FLASH_SECTOR_7,(uint32_t *)&STMFlashReadWrite_Stru_temp, sizeof(STMFlashReadWrite_Stru)/4); 
	memcpy((uint8_t *)&STMFlashReadWrite_Stru_temp.Network_Manage_temp,(uint8_t *)&Network_Manage_pbuf, sizeof(Network_Manage));
	STMFLASH_Write(ADDR_FLASH_SECTOR_7 ,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru)/4);//д��һ���� 
}

extern Network_Manage Network_Manage_t;
uint8_t STMFLASH_NetworkPara_Config_Read(void)   	
{
	STMFlashReadWrite_Stru STMFlashReadWrite_Stru_temp;
	memset((uint32_t *)&STMFlashReadWrite_Stru_temp, 0, sizeof(STMFlashReadWrite_Stru));
	STMFLASH_Read(ADDR_FLASH_SECTOR_7,(uint32_t *)&STMFlashReadWrite_Stru_temp, sizeof(STMFlashReadWrite_Stru)/4); 
	memcpy((uint8_t *)&Network_Manage_t, (uint8_t *)&STMFlashReadWrite_Stru_temp.Network_Manage_temp, sizeof(Network_Manage));
	return Network_Manage_t.net_mark;
}

void STMFLASH_Write_test(void)
{
	Upgrade_StatusWrite();
	Upgrade_CleanWrite();
	AnchorToAnchor_Distance[0] = 1200;
	AnchorToAnchor_Distance[1] = 1300;
	AnchorToAnchor_Distance[2] = 1400;
	STMFLASH_Anchors_Distance_Write(0x01);
	STMFLASH_Anchors_Distance_Read();
  log_print( DEBUG, ("H_dis = %f,L_dis = %f,R_dis = %f\r\n", AnchorToAnchor_Distance[0],AnchorToAnchor_Distance[1],AnchorToAnchor_Distance[2]));
	DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[0].FollowOffset_Mark = 0x01;
	DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[0].FollowMode = 0x01;  	
	DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[0].A_distance_offset =  567;
	DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[0].B_distance_offset =  789;
	STMFLASH_HeaderDistanceOffset_Write();
	DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[1].FollowOffset_Mark = 0x04;
	DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[1].FollowMode = 0x02;  	
	DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[1].A_distance_offset =  123;
	DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[1].B_distance_offset =  456;
	STMFLASH_LeftDistanceOffset_Write();
	DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[2].FollowOffset_Mark = 0x05;
	DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[2].FollowMode = 0x03;  	
	DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[2].A_distance_offset =  1576;
	DistanceOffset_Stru_temp.FollowModeOffset_Stru_temp[2].B_distance_offset =  489;
	STMFLASH_RightDistanceOffset_Write();
	STMFLASH_DistanceOffset_Read();
	
		//Ĭ��Զ��IPΪ:192.168.1.100
	Network_Manage_t.remoteip[0] = 110;	
	Network_Manage_t.remoteip[1] = 112;
	Network_Manage_t.remoteip[2] = 1;
	Network_Manage_t.remoteip[3] = 113;
	Network_Manage_t.remote_port = 6008;
	
	Network_Manage_t.ip[0] = 114;	
	Network_Manage_t.ip[1] = 115;
	Network_Manage_t.ip[2] = 7;
	Network_Manage_t.ip[3] = 102;
	Network_Manage_t.local_port = 6004;
	STMFLASH_NetworkPara_Config_Write(Network_Manage_t);
	Network_Manage_t.remoteip[0] = 0;	
	Network_Manage_t.remoteip[1] = 0;
	Network_Manage_t.remoteip[2] = 0;
	Network_Manage_t.remoteip[3] = 0;
	Network_Manage_t.remote_port = 0;
	
	Network_Manage_t.ip[0] = 0;	
	Network_Manage_t.ip[1] = 0;
	Network_Manage_t.ip[2] = 0;
	Network_Manage_t.ip[3] = 0;
	Network_Manage_t.local_port = 6009;
	STMFLASH_NetworkPara_Config_Read();
	printf("Static IP........................%d.%d.%d.%d\r\n",Network_Manage_t.ip[0],Network_Manage_t.ip[1],Network_Manage_t.ip[2],Network_Manage_t.ip[3]);
	printf("Remote IP..........................%d.%d.%d.%d\r\n",Network_Manage_t.remoteip[0],Network_Manage_t.remoteip[1],
	                                                            Network_Manage_t.remoteip[2],Network_Manage_t.remoteip[3]);
	printf("local_port = %d, remote_port = %d\r\n",Network_Manage_t.local_port,Network_Manage_t.remote_port);																												 
	while(1);
}


