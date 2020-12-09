#include "stmflash.h"

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
	u32 i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadWord(ReadAddr);//��ȡ4���ֽ�.
		ReadAddr+=4;//ƫ��4���ֽ�.	
	}
}

upgrateInfo_stru upgrateInfo_stru_temp;
HardwareInfo_Stru HardwareInfo_stru_temp;
Network_Manage Network_Manage_stru_temp; // �����������

void upgrateHardSoft_Write(void)   	
{
	STMFlashReadWrite_Stru STMFlashReadWrite_Stru_temp;
	memset((uint8_t *)&STMFlashReadWrite_Stru_temp, 0, sizeof(STMFlashReadWrite_Stru));
	STMFLASH_Read(ADDR_FLASH_SECTOR_7,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru_temp)/4); 
	STMFlashReadWrite_Stru_temp.upgrateInfo_stru_temp.upgrate_mark = 0x01;
	memcpy((uint8_t *)&STMFlashReadWrite_Stru_temp.upgrateInfo_stru_temp.HardwareVersion, (uint8_t *)&upgrateInfo_stru_temp.HardwareVersion, 3);
	memcpy((uint8_t *)&STMFlashReadWrite_Stru_temp.upgrateInfo_stru_temp.SoftwareVersion, (uint8_t *)&upgrateInfo_stru_temp.SoftwareVersion, 3);
  //memcpy((uint8_t *)&STMFlashReadWrite_Stru_temp.upgrateInfo_stru_temp, (uint8_t *)&upgrateInfo_stru_temp, sizeof(upgrateInfo_stru_temp));
	STMFLASH_Write(ADDR_FLASH_SECTOR_7 ,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru));//д��һ���� 
	log_print( DEBUG, ("upgrateInfo write to flash \r\n"));
}

void upgrateInfo_Write(void)   	
{
	STMFlashReadWrite_Stru STMFlashReadWrite_Stru_temp;
	memset((uint8_t *)&STMFlashReadWrite_Stru_temp, 0, sizeof(STMFlashReadWrite_Stru));
	STMFLASH_Read(ADDR_FLASH_SECTOR_7,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru_temp)/4); 
  memcpy((uint8_t *)&STMFlashReadWrite_Stru_temp.upgrateInfo_stru_temp, (uint8_t *)&upgrateInfo_stru_temp, sizeof(upgrateInfo_stru_temp));
	STMFLASH_Write(ADDR_FLASH_SECTOR_7 ,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru));//д��һ���� 
	log_print( DEBUG, ("upgrateInfo write to flash \r\n"));
}

void upgrateInfo_Read(void)   	
{
	STMFlashReadWrite_Stru STMFlashReadWrite_Stru_temp;
	memset((uint8_t *)&STMFlashReadWrite_Stru_temp, 0, sizeof(STMFlashReadWrite_Stru));
	STMFLASH_Read(ADDR_FLASH_SECTOR_7,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru_temp)/4); 
  memcpy((uint8_t *)&upgrateInfo_stru_temp, (uint8_t *)&STMFlashReadWrite_Stru_temp.upgrateInfo_stru_temp, sizeof(upgrateInfo_stru_temp));
}

void AnchorWorkMode_Write(void)   	
{
	STMFlashReadWrite_Stru STMFlashReadWrite_Stru_temp;
	memset((uint32_t *)&STMFlashReadWrite_Stru_temp, 0, sizeof(STMFlashReadWrite_Stru));
	STMFLASH_Read(ADDR_FLASH_SECTOR_7,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru_temp)/4); 
  STMFlashReadWrite_Stru_temp.HardwareInfo_stru_temp.HardwareInfo_mark = 0x01;
	STMFlashReadWrite_Stru_temp.HardwareInfo_stru_temp.PowerOnType = HardwareInfo_stru_temp.PowerOnType;
  memcpy((uint8_t *)&STMFlashReadWrite_Stru_temp.HardwareInfo_stru_temp.AnchorWorkMode, (uint8_t *)&HardwareInfo_stru_temp.AnchorWorkMode, 1);
	STMFLASH_Write(ADDR_FLASH_SECTOR_7 ,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru));//д��һ���� 
	log_print( DEBUG, ("HardwareInfo write to flash \r\n"));
}

void PowerOnType_Write(void)   	
{
	STMFlashReadWrite_Stru STMFlashReadWrite_Stru_temp;
	memset((uint8_t *)&STMFlashReadWrite_Stru_temp, 0, sizeof(STMFlashReadWrite_Stru));
	STMFLASH_Read(ADDR_FLASH_SECTOR_7,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru_temp)/4); 
  memcpy((uint8_t *)&STMFlashReadWrite_Stru_temp.HardwareInfo_stru_temp.PowerOnType, (uint8_t *)&HardwareInfo_stru_temp.PowerOnType, 1);
	STMFlashReadWrite_Stru_temp.HardwareInfo_stru_temp.AnchorWorkMode = HardwareInfo_stru_temp.AnchorWorkMode;
	STMFlashReadWrite_Stru_temp.HardwareInfo_stru_temp.HardwareInfo_mark = 0x01;
	STMFLASH_Write(ADDR_FLASH_SECTOR_7 ,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru));//д��һ���� 
	log_print( DEBUG, ("HardwareInfo write to flash \r\n"));
}

void HardwareInfo_Read(void)   	
{
	STMFlashReadWrite_Stru STMFlashReadWrite_Stru_temp;
	memset((uint8_t *)&STMFlashReadWrite_Stru_temp, 0, sizeof(STMFlashReadWrite_Stru));
	STMFLASH_Read(ADDR_FLASH_SECTOR_7,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru_temp)/4); 
  memcpy((uint8_t *)&HardwareInfo_stru_temp, (uint8_t *)&STMFlashReadWrite_Stru_temp.HardwareInfo_stru_temp, sizeof(HardwareInfo_stru_temp));
}

void Network_Manage_Write(void)   	
{
	STMFlashReadWrite_Stru STMFlashReadWrite_Stru_temp;
	memset((uint8_t *)&STMFlashReadWrite_Stru_temp, 0, sizeof(STMFlashReadWrite_Stru));
	STMFLASH_Read(ADDR_FLASH_SECTOR_7,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru_temp)/4); 
  memcpy((uint8_t *)&STMFlashReadWrite_Stru_temp.Network_Manage_stru_temp, (uint8_t *)&Network_Manage_stru_temp, sizeof(Network_Manage_stru_temp));
	STMFLASH_Write(ADDR_FLASH_SECTOR_7 ,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru));//д��һ���� 
	log_print( DEBUG, ("Network_Manage write to flash \r\n"));
}

void Network_Manage_Read(void)   	
{
	STMFlashReadWrite_Stru STMFlashReadWrite_Stru_temp;
	memset((uint8_t *)&STMFlashReadWrite_Stru_temp, 0, sizeof(STMFlashReadWrite_Stru));
	STMFLASH_Read(ADDR_FLASH_SECTOR_7,(uint32_t *)&STMFlashReadWrite_Stru_temp,sizeof(STMFlashReadWrite_Stru_temp)/4); 
  memcpy((uint8_t *)&Network_Manage_stru_temp, (uint8_t *)&STMFlashReadWrite_Stru_temp.Network_Manage_stru_temp, sizeof(Network_Manage_stru_temp));
}

