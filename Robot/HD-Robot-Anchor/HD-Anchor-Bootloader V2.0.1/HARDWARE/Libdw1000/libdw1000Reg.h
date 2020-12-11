#ifndef _LIBDW1000REG_H_
#define _LIBDW1000REG_H_

#include "common.h"


// OTP addresses definitions
#define LDOTUNE_ADDRESS (0x04)
#define PARTID_ADDRESS (0x06)
#define LOTID_ADDRESS  (0x07)
#define VBAT_ADDRESS   (0x08)
#define VTEMP_ADDRESS  (0x09)
#define XTRIM_ADDRESS  (0x1E)

#define OTP_IF_ID               0x2D   
#define OTP_IF_LEN              (18)

#define OTP_IF_ID               0x2D            /* One Time Programmable Memory Interface */
#define OTP_IF_LEN              (18)
/* offset from OTP_IF_ID in bytes */
#define OTP_WDAT                0x00            /* 32-bit register. The data value to be programmed into an OTP location  */
#define OTP_WDAT_LEN            (4)
/* offset from OTP_IF_ID in bytes */
#define OTP_ADDR                0x04            /* 16-bit register used to select the address within the OTP memory block */
#define OTP_ADDR_LEN            (2)
#define OTP_ADDR_MASK           0x07FF          /* This 11-bit field specifies the address within OTP memory that will be accessed read or written. */
/* offset from OTP_IF_ID in bytes */
#define OTP_CTRL                0x06            /* used to control the operation of the OTP memory */
#define OTP_CTRL_LEN            (2)
#define OTP_CTRL_MASK           0x8002
#define OTP_CTRL_OTPRDEN		0x0001			/* This bit forces the OTP into manual read mode */
#define OTP_CTRL_OTPREAD        0x0002          /* This bit commands a read operation from the address specified in the OTP_ADDR register */
#define OTP_CTRL_LDELOAD        0x8000          /* This bit forces a load of LDE microcode */
#define OTP_CTRL_OTPPROG        0x0040          /* Setting this bit will cause the contents of OTP_WDAT to be written to OTP_ADDR. */
/* offset from OTP_IF_ID in bytes */
#define OTP_STAT                0x08
#define OTP_STAT_LEN            (2)
#define OTP_STAT_MASK           0x0003
#define OTP_STAT_OTPPRGD        0x0001          /* OTP Programming Done */
//#define OTP_STAT_OTPVLTOK     0x0002          /* OTP Programming Voltage OK */   !!!!!!!!!!!!
/* offset from OTP_IF_ID in bytes */
#define OTP_RDAT                0x0A            /* 32-bit register. The data value read from an OTP location will appear here */
#define OTP_RDAT_LEN            (4)
/* offset from OTP_IF_ID in bytes */
#define OTP_SRDAT               0x0E            /* 32-bit register. The data value stored in the OTP SR (0x400) location will appear here after power up */
#define OTP_SRDAT_LEN           (4)
/* offset from OTP_IF_ID in bytes */
#define OTP_SF                  0x12            /*8-bit special function register used to select and load special receiver operational parameter */
#define OTP_SF_LEN              (1)
#define OTP_SF_MASK             0x63
#define OTP_SF_OPS_KICK         0x01            /* This bit when set initiates a load of the operating parameter set selected by the OPS_SEL */
#define OTP_SF_LDO_KICK         0x02            /* This bit when set initiates a load of the LDO tune code */
#define OTP_SF_OPS_SEL_L64      0x00            /* Operating parameter set selection: Length64 */
#define OTP_SF_OPS_SEL_TIGHT    0x40            /* Operating parameter set selection: Tight */

/* 外部放大器的GPIO引脚控制 */

// Defined constants for "lna_pa" bit field parameter passed to dwt_setlnapamode() function
#define DWT_LNA_PA_DISABLE     0x00
#define DWT_LNA_ENABLE         0x01
#define DWT_PA_ENABLE          0x02

#define GPIO_MSGP4_MASK         0x0000C000UL    /* Mode Selection for GPIO4/EXTPA */
#define GPIO_MSGP5_MASK         0x00030000UL    /* Mode Selection for GPIO5/EXTTXE */
#define GPIO_MSGP6_MASK         0x000C0000UL    /* Mode Selection for GPIO6/EXTRXE */

#define GPIO_PIN4_EXTPA         0x00004000UL    /* The pin operates as the EXTPA output */
#define GPIO_PIN5_EXTTXE        0x00010000UL    /* The pin operates as the EXTTXE output */
#define GPIO_PIN6_EXTRXE        0x00040000UL    /* The pin operates as the EXTRXE output */

uint32_t dwGetDeviceEUI(dwDevice_t* dev, uint32_t address);
void dwSetLNA_PA_Mode(dwDevice_t* dev, int lna_pa);

#endif