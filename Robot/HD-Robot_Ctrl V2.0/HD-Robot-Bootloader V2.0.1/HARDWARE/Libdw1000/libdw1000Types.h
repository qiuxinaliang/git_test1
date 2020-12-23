#ifndef __LIBDW1000_TYPES_H__
#define __LIBDW1000_TYPES_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "dw1000.h"

struct dwOps_s;
struct dwDevice_s;

/*keil默认不支持匿名结构体，需要指明*/
#pragma anon_unions 
typedef union dwTime_u
{
    uint8_t raw[5];
    uint64_t full;
    struct{
        uint32_t low32;
        uint8_t high8;
    } __attribute__((packed));

    struct{
        uint8_t low8;
        uint32_t high32;
    } __attribute__((packed));
    
} __attribute__((packed)) dwTime_t;

typedef void (*dwHandler_t) (struct dwDevice_s *dev);

/* 设备参数配置结构体*/
typedef struct dwDevice_s
{
    struct dwOps_s *ops;
    void *userdata;

    /* 参数配置*/
    uint8_t sysctrl[LEN_SYS_CTRL];
    uint8_t deviceMode;
    uint8_t networkAndAddress[LEN_PANADR];
    uint8_t syscfg[LEN_SYS_CFG];
    uint8_t sysmask[LEN_SYS_MASK];
    uint8_t chanctrl[LEN_CHAN_CTRL];
    uint8_t sysstatus[LEN_SYS_STATUS];
    uint8_t txfctrl[LEN_TX_FCTRL];

    uint8_t extendedFrameLength;
    uint8_t pacSize;
    uint8_t pulseFrequency;
    uint8_t dataRate;
    uint8_t preambleLength;
    uint8_t preambleCode;
    uint8_t channel;

    bool smartPower;
    bool frameCheck;
    bool permanentReceive;
    bool wait4resp;

    dwTime_t    antennaDelay;

    dwHandler_t handleSent;
    dwHandler_t handleReceived;
    dwHandler_t handleReceiveTimeout;
}dwDevice_t;

typedef void (*dwHandler_t) (dwDevice_t *dev);

/* 设置枚举*/
typedef enum {dwSpiSpeedLow, dwSpiSpeedHigh} dwSpiSpeed_t;

typedef enum {dwClockAuto = 0x00, dwClockXti = 0x01, dwClockPll = 0x02} dwClock_t;


/* DW_SPI操作结构体函数*/
typedef struct dwOps_s
{
    void (*spiRead) (dwDevice_t* dev, const void *header, size_t headerLength, 
    void* data, size_t dataLength);

    void (*spiWrite) (dwDevice_t* dev, const void *header, size_t headerLength, 
    const void* data, size_t dataLength);

    void (*spiSetSpeed) (dwDevice_t* dev, dwSpiSpeed_t speed);
    
    void (*delayms) (dwDevice_t* dev, unsigned int delay);

    void (*reset) (dwDevice_t *dev);

}dwOps_t;

#endif //__LIBDW1000_TYPES_H__
