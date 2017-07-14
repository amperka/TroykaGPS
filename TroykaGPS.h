#ifndef __AMPERKA_GPRS_H__
#define __AMPERKA_GPRS_H__

#include "uartDevice.h"
#include "Print.h"
#include "Stream.h"

#define GPS_OK            0
#define GPS_ERROR_DATA   -1
#define GPS_ERROR_SAT    -2

class GPS
{

public:
    GPS(Stream& serial);
    int8_t getData();
    int8_t getSat() const { return _sat; }
    float getSpeed() const { return _speed; }
    float getAltitude() const { return _altitude; }
    float getLatitudeBase10() const { return _latitudeBase10; }
    float getLongitudeBase10() const { return _longitudeBase10; }
    void getLatitudeBase60(char* latitudeBase60) const;
    void getLongitudeBase60(char* longitudeBase60) const;
    void getTime(char* time) const;
    void getDate(char* date) const;
    int8_t getSecond() const { return _second; }
    int8_t getMinute() const { return _minute; }
    int8_t getHour() const { return _hour; }
    int8_t getDay() const { return _day; }
    int8_t getMonth() const { return _month; }
    uint16_t getYear() const { return _year; }

 private:
    int available();
    int waitAvailable(int time);
    int read();
    bool readData(char* gpsConnectSat, char* time, char* date, char* latitude, char* longitude, char* speed, char* altitude, char* sat);
    bool _gpsConnectSat;
 	int8_t _sat;
 	int8_t _second;
    int8_t _minute;
    int8_t _hour;
    int8_t _day;
    int8_t _month;
    uint16_t _year;
    float _speed;
    float _altitude;
    float _latitudeBase10;
    float _longitudeBase10;
    char _latitudeBase60[16];
    char _longitudeBase60[16];
    char _time[16];
    char _date[16];
};
#endif
