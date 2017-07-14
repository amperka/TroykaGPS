#include "uartDevice.h"

Stream* serialUartDevice;// = NULL;

void  uartDeviceInit(void* uartDevice) {
    serialUartDevice = (Stream*)uartDevice;
}

int uartDeviceAvailable() {
    return serialUartDevice->available();
}

int uartDeviceWaitAvailable (int waitTime) {
    unsigned long timerStart;
    int dataLen = 0;
    timerStart = millis();
    while((unsigned long) (millis() - timerStart) > waitTime) {
//        delay(500);
        dataLen = uartDeviceAvailable();
        if(dataLen > 0) {
            break;
        }
    }
    return dataLen;
}

void uartDeviceFlushSerial() {
    while (uartDeviceAvailable()) {
        char c = serialUartDevice -> read();
    }
}

int uartDeviceReadSerial() {
    return serialUartDevice -> read();
}

void uartDeviceReadBuffer(char* buffer, int count, unsigned int timeout, unsigned int charTimeout) {
    int i = 0;
    unsigned long timerStart, prevChar;
    timerStart = millis();
    prevChar = 0;
    while(1) {
        while (uartDeviceAvailable()) {
            char c = serialUartDevice -> read();
            prevChar = millis();
            buffer[i++] = c;
            if(i >= count)
                break;
        }
        if(i >= count)
            break;
        if ((unsigned long) (millis() - timerStart) > timeout) {
            break;
        }
        //If interchar Timeout => return FALSE. So we can return sooner from this function. Not DO it if we dont recieve at least one char (prevChar <> 0)
        if (((unsigned long) (millis() - prevChar) > charTimeout) && (prevChar != 0)) {
            break;
        }
    }
}

void uartDeviceCleanBuffer(char* buffer, int count) {
    for(int i = 0; i < count; i++) {
        buffer[i] = '\0';
    }
}

void uartDeviceSendByte(uint8_t data) {
	serialUartDevice->write(data);
}

void uartDeviceSendChar(const char c) {
	serialUartDevice->write(c);
}

void uartDeviceSendCMD(const char* cmd) {
    for(int i = 0; i < strlen(cmd); i++) {
        uartDeviceSendByte(cmd[i]);
    }
}

void uartDeviceSendCMD(const __FlashStringHelper* cmd) {
    int i = 0;
    const char *ptr = (const char *) cmd;
    while (pgm_read_byte(ptr + i) != 0x00) {
        uartDeviceSendByte(pgm_read_byte(ptr + i++));  
    }
}

void uartDeviceSendCMDP(const char* cmd) {
    // pgm_read_byte(address_short) - Read a byte from the program space with a 16-bit (near) address
    while (pgm_read_byte(cmd) != 0x00)
    uartDeviceSendByte(pgm_read_byte(cmd++));  
}

void uartDeviceSendAT(void) {
    uartDeviceCheckWithCMD(F("AT\r\n"),"OK", CMD);
}

void uartDeviceSendEndMark(void) {
    uartDeviceSendByte((char)26);
}

bool uartDeviceWaitForResp(const char* resp, dataType type, unsigned int timeout, unsigned int charTimeout) {
    int len = strlen(resp);
    int sum = 0;
    // prevChar is the time when the previous Char has been read
    unsigned long timerStart, prevChar;
    timerStart = millis();
    prevChar = 0;
    while(1) {
        if (uartDeviceAvailable()) {
            char c = serialUartDevice -> read();
            prevChar = millis();
            sum = (c == resp[sum]) ? sum + 1 : 0;
            if (sum == len) {
                break;
            }
        }
        if ((unsigned long) (millis() - timerStart) > timeout) {
            return false;
        }
        // if interchar Timeout => return FALSE. So we can return sooner from this function
        if (((unsigned long) (millis() - prevChar) > charTimeout) && (prevChar != 0)) {
            return false;
        }
        
    }
    // if is a CMD, we will finish to read buffer
    if(type == CMD)
        uartDeviceFlushSerial();
    return true;   
}


bool uartDeviceCheckWithCMD(const char* cmd, const char *resp, dataType type, unsigned int timeout, unsigned int charTimeout) {
    uartDeviceSendCMD(cmd);
    return uartDeviceWaitForResp(resp, type, timeout, charTimeout);
}

// HACERR que tambien la respuesta pueda ser FLASH STRING
bool uartDeviceCheckWithCMD(const __FlashStringHelper* cmd, const char *resp, dataType type, unsigned int timeout, unsigned int charTimeout) {
    uartDeviceSendCMD(cmd);
    return uartDeviceWaitForResp(resp, type, timeout, charTimeout);
}
