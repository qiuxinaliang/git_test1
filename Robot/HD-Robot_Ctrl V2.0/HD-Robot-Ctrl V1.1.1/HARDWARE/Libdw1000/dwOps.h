#ifndef __DWOPS_H__
#define __DWOPS_H__

#include "common.h"

extern SPI_HandleTypeDef hspi4;

void dwOpsInit(dwDevice_t *device);
extern dwOps_t dwOps;
extern void delayms(dwDevice_t* dev, unsigned int delay);
#endif
