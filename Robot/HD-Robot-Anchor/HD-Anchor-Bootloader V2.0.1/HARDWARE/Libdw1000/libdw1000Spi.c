#include "libdw1000Spi.h"

/*
*******************************************************************************
*	函 数 名: dwSpiRead
*	功能说明: DW1000读数据函数
*	形	  参: regid:地址address:读地址data:读取的数据length:读取数据的长度
*	返 回 值: 无
*******************************************************************************
*/
void dwSpiRead(dwDevice_t * dev, uint8_t regid, uint32_t address, void* data, size_t length)
{
    uint8_t header[3] = { 0 };
    size_t headerLength = 1;

    header[0] = regid & 0x3f;

    if(address != 0)
    {
        header[0] |= 0x40;
        
        header[1]  = address & 0x7f;
        address >>= 7;
        headerLength = 2;

        if(address != 0)
        {
            header[1] |= 0x80;
            header[2] |= address & 0xff;
            headerLength = 3;
        } 
    }

    dev->ops->spiRead(dev, header, headerLength, data, length);
}

/*
*******************************************************************************
*	函 数 名: dwSpiRead16
*	功能说明: DW1000读数据函数
*	形	  参: regid:地址address:读取地址
*	返 回 值: data: 读取的是数据
*******************************************************************************
*/
uint16_t dwSpiRead16(dwDevice_t * dev, uint8_t regid, uint32_t address)
{
    uint16_t data = 0;
    dwSpiRead(dev, regid, address, &data, sizeof(data));

    return data;
}

uint32_t dwSpiRead32(dwDevice_t * dev, uint8_t regid, uint32_t address)
{
    uint32_t data = 0;
    dwSpiRead(dev, regid, address, &data, sizeof(data));

    return data;
}

/*
*******************************************************************************
*	函 数 名: dwSpiWrite
*	功能说明: DW1000写数据
*	形	  参: regid:地址address:读取地址data:待写入数据length:待写入数据的长度
*	返 回 值: 无
*******************************************************************************
*/
void dwSpiWrite(dwDevice_t *dev, uint8_t regid, uint32_t address, const void* data, size_t length)
{
    uint8_t header[3] = { 0 };
    size_t headerLength = 1;

    header[0] = regid & 0x3f;
    header[0] |= 0x80;

    if(address != 0)
    {
        header[0] |= 0x40;
        header[1] = address & 0x7f;
        address >>= 7;
		headerLength = 2;

        if(address != 0)
        {
            header[1] |= 0x80;
            header[2] = address & 0xff;
            headerLength = 3;
        }
    }

    dev-> ops->spiWrite(dev, header, headerLength, (uint8_t *)data, length);
}

void dwSpiWrite32(dwDevice_t * dev, uint8_t regid, uint32_t address, uint32_t data)
{
    dwSpiWrite(dev, regid, address, &data, sizeof(data));

}
