#ifndef __LIBDW1000_SPI_H__
#define __LIBDW1000_SPI_H__

#include "common.h"
#include "libdw1000Types.h"

/* SPI读写接口 */
void dwSpiRead(dwDevice_t *dev, uint8_t regid, uint32_t address, void* data, size_t length);
uint16_t dwSpiRead16(dwDevice_t *dev, uint8_t regid, uint32_t address);
uint32_t dwSpiRead32(dwDevice_t *dev, uint8_t regid, uint32_t address);

void dwSpiWrite(dwDevice_t *dev, uint8_t regid, uint32_t address,const void* data, size_t length);

void dwSpiWrite32(dwDevice_t *dev, uint8_t regid, uint32_t address,uint32_t data);

#endif //__LIBDW1000_SPI_H__
