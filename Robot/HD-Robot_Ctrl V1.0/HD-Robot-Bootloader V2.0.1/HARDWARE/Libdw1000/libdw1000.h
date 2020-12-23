#ifndef __LIBDW1000_H__
#define __LIBDW1000_H__

#include "common.h"

extern const uint8_t MODE_LONGDATA_RANGE_LOWPOWER[];
extern const uint8_t MODE_SHORTDATA_FAST_LOWPOWER[];
extern const uint8_t MODE_LONGDATA_FAST_LOWPOWER[];
extern const uint8_t MODE_SHORTDATA_FAST_ACCURACY[];
extern const uint8_t MODE_LONGDATA_FAST_ACCURACY[];
extern const uint8_t MODE_LONGDATA_RANGE_ACCURACY[];

void dwInit(dwDevice_t* dev, dwOps_t* ops);
void dwSetUserdata(dwDevice_t* dev, void* userdata);
void* dwGetUserdata(dwDevice_t* dev);
int dwConfigure(dwDevice_t* dev);
uint32_t dwGetDeviceId(dwDevice_t* dev);
void dwEnableAllLeds(dwDevice_t* dev);
void dwEnableClock(dwDevice_t* dev, dwClock_t clock);
void dwSoftReset(dwDevice_t* dev);
void dwManageLDE(dwDevice_t* dev);

/* ###########################################################################
 * #### DW1000 register read/write ###########################################
 * ######################################################################### */

void dwReadSystemConfigurationRegister(dwDevice_t* dev);
void dwWriteSystemConfigurationRegister(dwDevice_t* dev);
void dwReadSystemEventStatusRegister(dwDevice_t* dev);
void dwReadNetworkIdAndDeviceAddress(dwDevice_t* dev);
void dwWriteNetworkIdAndDeviceAddress(dwDevice_t* dev);
void dwReadSystemEventMaskRegister(dwDevice_t* dev);
void dwWriteSystemEventMaskRegister(dwDevice_t* dev);
void dwReadChannelControlRegister(dwDevice_t* dev);
void dwWriteChannelControlRegister(dwDevice_t* dev);
void dwReadTransmitFrameControlRegister(dwDevice_t* dev);
void dwWriteTransmitFrameControlRegister(dwDevice_t* dev);

/****************************************************************/

void dwSetReceiveWaitTimeout(dwDevice_t *dev, uint16_t timeout);

void dwSetFrameFilter(dwDevice_t* dev, bool val);
void dwSetFrameFilterBehaveCoordinator(dwDevice_t* dev, bool val);
void dwSetFrameFilterAllowBeacon(dwDevice_t* dev, bool val);
void dwSetFrameFilterAllowData(dwDevice_t* dev, bool val);
void dwSetFrameFilterAllowAcknowledgement(dwDevice_t* dev, bool val);
void dwSetFrameFilterAllowMAC(dwDevice_t* dev, bool val);
void dwSetFrameFilterAllowReserved(dwDevice_t* dev, bool val);
void dwSetDoubleBuffering(dwDevice_t* dev, bool val);
void dwSetInterruptPolarity(dwDevice_t* dev, bool val);
void dwSetReceiverAutoReenable(dwDevice_t* dev, bool val);
void dwInterruptOnSent(dwDevice_t* dev, bool val);
void dwInterruptOnReceived(dwDevice_t* dev, bool val);
void dwInterruptOnReceiveFailed(dwDevice_t* dev, bool val);
void dwInterruptOnReceiveTimeout(dwDevice_t* dev, bool val);
void dwInterruptOnReceiveTimestampAvailable(dwDevice_t* dev, bool val);
void dwInterruptOnAutomaticAcknowledgeTrigger(dwDevice_t* dev, bool val);
void dwClearInterrupts(dwDevice_t* dev);

void dwIdle(dwDevice_t* dev);
void dwNewReceive(dwDevice_t* dev);
void dwStartReceive(dwDevice_t* dev);
void dwStopReceive(dwDevice_t* dev);
void dwNewTransmit(dwDevice_t* dev);
void dwStartTransmit(dwDevice_t* dev);
void dwNewConfiguration(dwDevice_t* dev);
void dwCommitConfiguration(dwDevice_t* dev);
void dwWaitForResponse(dwDevice_t* dev, bool val);
void dwSuppressFrameCheck(dwDevice_t* dev, bool val);
void dwUseSmartPower(dwDevice_t* dev, bool smartPower);
dwTime_t dwSetDelay(dwDevice_t* dev, const dwTime_t* delay);
void dwSetDataRate(dwDevice_t* dev, uint8_t rate);
void dwSetPulseFrequency(dwDevice_t* dev, uint8_t freq);
uint8_t dwGetPulseFrequency(dwDevice_t* dev);
void dwSetPreambleLength(dwDevice_t* dev, uint8_t prealen);
void dwUseExtendedFrameLength(dwDevice_t* dev, bool val);
void dwReceivePermanently(dwDevice_t* dev, bool val);
void dwSetChannel(dwDevice_t* dev, uint8_t channel);
void dwSetPreambleCode(dwDevice_t* dev, uint8_t preacode);
void dwSetDefaults(dwDevice_t* dev);
void dwSetData(dwDevice_t* dev, uint8_t data[], unsigned int n);
unsigned int dwGetDataLength(dwDevice_t* dev);
void dwGetData(dwDevice_t* dev, uint8_t data[], unsigned int n);
void dwGetTransmitTimestamp(dwDevice_t* dev, dwTime_t* time);
void dwGetReceiveTimestamp(dwDevice_t* dev, dwTime_t* time);
void dwCorrectTimestamp(dwDevice_t* dev, dwTime_t* timestamp);
void dwGetSystemTimestamp(dwDevice_t* dev, dwTime_t* time);
bool dwIsTransmitDone(dwDevice_t* dev);
bool dwIsReceiveTimestampAvailable(dwDevice_t* dev);
bool dwIsReceiveDone(dwDevice_t* dev);
bool dwIsReceiveFailed(dwDevice_t *dev);
bool dwIsReceiveTimeout(dwDevice_t* dev);
bool dwIsClockProblem(dwDevice_t* dev);
void dwClearAllStatus(dwDevice_t* dev);
void dwClearReceiveTimestampAvailableStatus(dwDevice_t* dev);
void dwClearReceiveStatus(dwDevice_t* dev);
void dwClearTransmitStatus(dwDevice_t* dev);
float dwGetReceiveQuality(dwDevice_t* dev);
float dwGetFirstPathPower(dwDevice_t* dev);
float dwGetReceivePower(dwDevice_t* dev);
void dwEnableMode(dwDevice_t *dev, const uint8_t mode[]);
void dwTune(dwDevice_t *dev);
void dwHandleInterrupt(dwDevice_t *dev);

void dwAttachSentHandler(dwDevice_t *dev, dwHandler_t handler);
void dwAttachReceivedHandler(dwDevice_t *dev, dwHandler_t handler);
void dwAttachReceiveTimeoutHandler(dwDevice_t *dev, dwHandler_t handler);

void dwSetAntenaDelay(dwDevice_t *dev, dwTime_t delay);

/* Tune the DWM radio parameters */
void dwTune(dwDevice_t *dev);

void dwIdle(dwDevice_t* dev);

char* dwStrError(int error);

/* Error codes */
#define DW_ERROR_OK 0
#define DW_ERROR_WRONG_ID 1

#endif


