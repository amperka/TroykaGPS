#ifndef __UART_DEVICE_H__
#define __UART_DEVICE_H__
#include <Arduino.h>

// miliseconds
#define DEFAULT_TIMEOUT     		1000
// miliseconds
#define DEFAULT_INTERCHAR_TIMEOUT	1000

enum dataType {
    CMD     = 0,
    DATA    = 1,
};

void  uartDeviceInit(void* uartDevice);
int   uartDeviceAvailable();
int   uartDeviceAvailable(int waitTime);
void  uartDeviceFlushSerial();
int   uartDeviceReadSerial();
void  uartDeviceReadBuffer(char* buffer,int count,  unsigned int timeout = DEFAULT_TIMEOUT, unsigned int charTimeout = DEFAULT_INTERCHAR_TIMEOUT);
void  uartDeviceCleanBuffer(char* buffer, int count);
void  uartDeviceSendByte(uint8_t data);
void  uartDeviceSendChar(const char c);
void  uartDeviceSendCMD(const char* cmd);
void  uartDeviceSendCMD(const __FlashStringHelper* cmd);
void  uartDeviceSendCMDP(const char* cmd);   
void  uartDeviceSendAT(void);
void  uartDeviceSendEndMark(void);
bool  uartDeviceWaitForResp(const char* resp, dataType type, unsigned int timeout = DEFAULT_TIMEOUT, unsigned int charTimeout = DEFAULT_INTERCHAR_TIMEOUT);
bool  uartDeviceCheckWithCMD(const char* cmd, const char *resp, dataType type, unsigned int timeout = DEFAULT_TIMEOUT, unsigned int charTimeout = DEFAULT_INTERCHAR_TIMEOUT);
bool  uartDeviceCheckWithCMD(const __FlashStringHelper* cmd, const char *resp, dataType type, unsigned int timeout = DEFAULT_TIMEOUT, unsigned int charTimeout = DEFAULT_INTERCHAR_TIMEOUT);

#endif
