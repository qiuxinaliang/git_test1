#include "libdw1000Reg.h"

/* EUI 读取函数 */
/* 从OTP中读取设备唯一ID */
uint32_t dwGetDeviceEUI(dwDevice_t* dev, uint32_t address)
{
    uint8_t buf[4];
    uint32_t ret_data;
	uint32_t reg;

    buf[1] = (address>>8) & 0xff;
    buf[0] = address & 0xff;

    dwSpiWrite(dev, OTP_IF_ID, OTP_ADDR, buf, 2);

    buf[0] = OTP_CTRL_OTPRDEN |OTP_CTRL_OTPREAD; 
    dwSpiWrite(dev, OTP_IF_ID, OTP_CTRL, buf, 1);
	buf[0] = 0x01;
	dwSpiWrite(dev, OTP_IF_ID, OTP_CTRL, buf, 1);
	ret_data = dwSpiRead32(dev, OTP_IF_ID, OTP_RDAT);
    buf[0] = 0x00; 
    dwSpiWrite(dev, OTP_IF_ID, OTP_CTRL, buf, 1);
	
	printf("dev_OTPEUI = 0x%x\n", ret_data);
    return (ret_data);
}

/* PA + LNA 的打开与关闭 */
void dwSetLNA_PA_Mode(dwDevice_t* dev, int lna_pa)
{
    uint32_t gpio_mode = dwSpiRead32(dev, GPIO_CTRL, GPIO_MODE_SUB);
    gpio_mode &= ~(GPIO_MSGP4_MASK | GPIO_MSGP5_MASK | GPIO_MSGP6_MASK);
    if (lna_pa & DWT_LNA_ENABLE)
    {
        gpio_mode |= GPIO_PIN6_EXTRXE;
    }
    if (lna_pa & DWT_PA_ENABLE)
    {
        gpio_mode |= (GPIO_PIN5_EXTTXE | GPIO_PIN4_EXTPA);
    }
    dwSpiWrite32(dev, GPIO_CTRL, GPIO_MODE_SUB, gpio_mode);
	/* Disable TXFSEQ */
	dwSpiWrite32(dev, PMSC, PMSC_CTRL0_TXFSEQ, 0);
}