#include <stdio.h>
#include "TroykaGPS.h"

GPS::GPS(Stream& serial)
{
  uartDeviceInit(&serial);
}

int8_t GPS::getData() {
    int i = 0;
    int j = 0;
    int t = 0;
    char gpsConnectSat[4];
    char time[16];
    char date[16];
    char latitude[16];
    char longitude[16];
    char parsingLatitude[16];
    char parsingLongitude[16];    
    char speed[8];
    char altitude[8];
    char sat[8];

    // проверка и чтение данных с gps-модуля
    if (!readData(gpsConnectSat, time, date, latitude, longitude, speed, altitude, sat)){
        return GPS_ERROR_DATA;
    }
    // есть ли связь со спутниками
    if (gpsConnectSat[0] != 'A') {
        return GPS_ERROR_SAT;
    }

    // запись данных времени в виде массива символов
    _time[0] = time[0];
    _time[1] = time[1];
    _time[2] = ':';
    _time[3] = time[2];
    _time[4] = time[3];
    _time[5] = ':';
    _time[6] = time[4];
    _time[7] = time[5];
    _time[8] = '\0';
    // запись данных времени в целочисленные переменные
    _hour = (time[0] - '0') * 10 + (time[1] - '0') + 3;
    _minute = (time[2] - '0') * 10 + (time[3] - '0');
    _second = (time[4] - '0') * 10 + (time[5] - '0');

    // запись данных даты в виде массива символов
    _date[0] = date[0];
    _date[1] = date[1];
    _date[2] = '.';
    _date[3] = date[2];
    _date[4] = date[3];
    _date[5] = '.';
    _date[6] = '2';
    _date[7] = '0';
    _date[8] = date[4];
    _date[9] = date[5];
    _date[10] = '\0';
    // запись данных времени в целочисленные переменные
    _day = (date[0] - '0') * 10 + (date[1] - '0');
    _month = (date[2] - '0') * 10 + (date[3] - '0');
    _year = 2000 + (date[4] - '0') * 10 + (date[5] - '0');

    // запись данных количества видимых спутников
    _sat = atoi(sat);
    // запись данных текущей скорости в узлах
    _speed = atof(speed);
    // запись данных высоты над уровнем моря
    _altitude = atof(altitude);

    // запись данных широты в ° градусах в виде десятичной дроби
    i = 2;
    j = 0;
    while (latitude[i] != '\0') {
        parsingLatitude[j++] = latitude[i++];
    }
    parsingLatitude[j] = '\0';
    _latitudeBase10 = atof(parsingLatitude) * 100 / 60 / 100;
    _latitudeBase10 = _latitudeBase10 + (latitude[0] - '0') * 10 + (latitude[1] - '0');
    if (latitude[strlen(latitude) - 1] == 'S') {
        _latitudeBase10 = -_latitudeBase10;
    }

    // запись данных долготы в ° градусах в виде десятичной дроби
    i = 3;
    j = 0;
    while (longitude[i] != '\0') {
        parsingLongitude[j++] = longitude[i++];
    }
    parsingLongitude[j] = '\0';
    _longitudeBase10 = atof(parsingLongitude) * 100 / 60 / 100;
    _longitudeBase10 = _longitudeBase10 + (longitude[0] - '0') * 100 + (longitude[1] - '0') * 10 + (longitude[2] - '0');
    if (longitude[strlen(longitude) - 1] == 'W') {
        _longitudeBase10 = -_longitudeBase10;
    }

    // запись данных широты в ° градусах, ′ минутах и ″ секундах с десятичной дробью
    float lalitudeSec;    
    char latitudeSecStr[12];
    i = 0;
    t = 0;
    _latitudeBase60[t++] = latitude[i++];
    _latitudeBase60[t++] = latitude[i++];
    _latitudeBase60[t++] = '*';
    _latitudeBase60[t++] = latitude[i++];
    _latitudeBase60[t++] = latitude[i++];
    _latitudeBase60[t++] = '\'';
    i++;
    j = 0;
    while (latitude[i] != '\0') {
        latitudeSecStr[j++] = latitude[i++];
    }
    latitudeSecStr[j] = '\0';
    lalitudeSec = atof(latitudeSecStr) * 60 / 100 / 10000;
    dtostrf(lalitudeSec, 6, 4, latitudeSecStr);
    i = 0;
    while (latitudeSecStr[i] != '\0') {
        _latitudeBase60[t++] = latitudeSecStr[i++];
    }
    _latitudeBase60[t++] = '"';
    _latitudeBase60[t++] = latitude[strlen(latitude) - 1];
    _latitudeBase60[t] = '\0';

    // запись данных долготы в ° градусах, ′ минутах и ″ секундах с десятичной дробью
    float longitudeSec;    
    char longitudeSecStr[12];
    i = 0;
    t = 0;
    if (longitude[0] == '1') {
        _longitudeBase60[t++] = longitude[i++];
    } else {
        i++;
    }
    _longitudeBase60[t++] = longitude[i++];
    _longitudeBase60[t++] = longitude[i++];
    _longitudeBase60[t++] = '*';
    _longitudeBase60[t++] = longitude[i++];
    _longitudeBase60[t++] = longitude[i++];
    _longitudeBase60[t++] = '\'';

    i++;
    j = 0;
    while (longitude[i] != '\0') {
        longitudeSecStr[j++] = longitude[i++];
    }
    longitudeSecStr[j] = '\0';
    longitudeSec = atof(longitudeSecStr) * 60 / 100 / 10000;
    dtostrf(longitudeSec, 6, 4, longitudeSecStr);
    
    i = 0;
    while (longitudeSecStr[i] != '\0') {
        _longitudeBase60[t++] = longitudeSecStr[i++];
    }
    _longitudeBase60[t++] = '"';
    _longitudeBase60[t++] = longitude[strlen(longitude) - 1];
    _longitudeBase60[t] = '\0';

    return GPS_OK;
}

bool GPS::readData(char* gpsConnectSat, char* time, char* date, char* latitude, char* longitude, char* speed, char* altitude, char* sat) {
    // $GNGGA,165708.000,5544.648951,N,03739.472758,E,2,5,4.62,143.098,M,14.426,M,,*49
    // $GNRMC,165708.000,A,5544.648951,N,03739.472758,E,4.00,63.20,070717,,,D*4F
    int i = 0;
    int j = 0;
    char *strGNGGA;
    char *strGNRMC;
    char gpsBuffer[1023];
    uartDeviceFlushSerial();
    uartDeviceCleanBuffer(gpsBuffer, sizeof(gpsBuffer));
    uartDeviceReadBuffer(gpsBuffer, sizeof(gpsBuffer), DEFAULT_TIMEOUT);
//    Serial.write(gpsBuffer, sizeof(gpsBuffer));
//    Serial.println();

    strGNRMC = strstr(gpsBuffer,"$GNRMC");

    if (strlen(strGNRMC) < 100) {
        return false;
    }
    if (strGNRMC[i] == '$') {
        i++;
        while (strGNRMC[i] != ',') {
            i++;
        }
    }

    if (strGNRMC[i] == ',') {
        i++;
        j = 0;
        while (strGNRMC[i] != '.') {
            time[j++] = strGNRMC[i++];
        }
        i++;
        time[j] = '\0';
        while (strGNRMC[i] != ',') {
            i++;
        }
    }
    // состояние GPS
    if (strGNRMC[i] == ',') {
        i++;
        gpsConnectSat[0] = strGNRMC[i];
        i++;
    }

    // запись данных широты
    if (strGNRMC[i] == ',') {
        i++;
        j = 0;
        while (strGNRMC[i] != ',') {
            latitude[j++] = strGNRMC[i++];
        }
        i++;
        while (strGNRMC[i] != ',') {
            latitude[j++] = strGNRMC[i++];
        }
        latitude[j] = '\0';  	
    }

    // запись данных долготы
    if (strGNRMC[i] == ',') {
        i++;
        j = 0;
        while (strGNRMC[i] != ',') {
            longitude[j++] = strGNRMC[i++];
        }
        i++;
        while (strGNRMC[i] != ',') {
            longitude[j++] = strGNRMC[i++];
        }
        longitude[j] = '\0';
    }

    // запись данных скорости
    if (strGNRMC[i] == ',') {
        i++;
        j = 0;
        while (strGNRMC[i] != ',') {
        speed[j++] = strGNRMC[i++];
        }
        speed[j] = '\0';
    }

    // пропуск данных высоты
    if (strGNRMC[i] == ',') {
        i++;
        j = 0;
        while (strGNRMC[i] != ',') {
            i++;
        }
    }

    // запись даты
    if (strGNRMC[i] == ',') {
        i++;
        j = 0;
        while (strGNRMC[i] != ',') {
            date[j++] = strGNRMC[i++];
        }
        date[j] = '\0';
    }

    // поиск сообщения
    strGNGGA = strstr(gpsBuffer,"$GNGGA");
    if (strlen(strGNGGA) < 100) {
        return false;
    }
    i = 0;
    // пропуск типа сообщения
    if (strGNGGA[i] == '$') {
        i++;
        while (strGNGGA[i] != ',') {
            i++;
        }
    }

    // пропуск записи данных о времени
    if (strGNGGA[i] == ',') {
        i++;
        j = 0;
        while (strGNGGA[i] != '.') {
            i++;
        }
        i++;
        while (strGNGGA[i] != ',') {
            i++;
        }
    }

    // пропуск данных широты
    if (strGNGGA[i] == ',') {
        i++;
        j = 0;
        while (strGNGGA[i] != ',') {
            i++;
        }   
    }
    // пропуск буквы широты «N»
    i = i + 2;

    // пропуск данных долготы
    if (strGNGGA[i] == ',') {
        i++;
        j = 0;
        while (strGNGGA[i] != ',') {
            i++;
        }
    }

    // пропуск буквы долготы «E»
    i = i + 2;
    // пропуск типа решения
    i = i + 2;

    // запись данных кол-ва спутников
    if (strGNGGA[i] == ',') {
        i++;
        j = 0;
        while (strGNGGA[i] != ',') {
            sat[j++] = strGNGGA[i++];
        }
        sat[j] = '\0';
    }

    // пропуск геометрического фактора
    if (strGNGGA[i] == ',') {
        i++;
        j = 0;
        while (strGNGGA[i] != ',') {
            i++;
        }   
    }
    // запись данных высоты
    if (strGNGGA[i] == ',') {
        i++;
        j = 0;
        while (strGNGGA[i] != ',') {
            altitude[j++] = strGNGGA[i++];
        }
        altitude[j] = '\0';
    }
    return true;
}

void GPS::getLatitudeBase60(char* latitudeBase60, size_t maxLength) const {
    strncpy(latitudeBase60, _latitudeBase60, maxLength);
}

void GPS::getLongitudeBase60(char* longitudeBase60, size_t maxLength) const {
    strncpy(longitudeBase60, _longitudeBase60, maxLength);
}

void GPS::getTime(char* time, size_t maxLength) const {
    strncpy(time, _time, maxLength);
}

void GPS::getDate(char* date, size_t maxLength) const {
    strncpy(date, _date, maxLength);
}

int GPS::available() {
    return uartDeviceAvailable();
}

int GPS::waitAvailable(int time) {
    return uartDeviceWaitAvailable(time);
}

int GPS::read() {
    return uartDeviceReadSerial();
}