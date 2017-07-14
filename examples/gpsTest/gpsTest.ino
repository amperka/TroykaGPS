// библиотека для работы с GPS устройством
#include <TroykaGPS.h>
// создаём объект класса GPS и передаём в него объект Serial1 
GPS gps(Serial1);

// массив для хранения текущего времени
char time[16];
// массив для хранения текущей даты
char date[16];
// массив для хранения широты в градусах, минутах и секундах
char latitudeBase60[16];
// массив для хранения долготы в градусах, минутах и секундах
char longitudeBase60[16];

void setup()
{ 
  // открываем последовательный порт для мониторинга действий в программе
  Serial.begin(115200);
  // ждём, пока не откроется монитор последовательного порта
  // для того, чтобы отследить все события в программе
  while (!Serial) {
  }
  Serial.print("Serial init OK\r\n");
  // открываем Serial-соединение с GPS-модулем
  Serial1.begin(115200);
}
 
void loop()
{
  // считываем состояние GPS-модуля
  switch(gps.getData()) {
    // всё OK
    case GPS_OK:
      Serial.println("GPS is OK");
      // выводим координаты широты и долготы
      // 1. в градусах, минутах и секундах
      // 2. градусах в виде десятичной дроби
      Serial.println("GPS Coordinates: ");
      gps.getLatitudeBase60(latitudeBase60);
      gps.getLongitudeBase60(longitudeBase60);
      Serial.print("Latitude\t");
      Serial.print(latitudeBase60);
      Serial.print("\t\t");
      Serial.println(gps.getLatitudeBase10(), 6);
      Serial.print("Longitude\t");
      Serial.print(longitudeBase60);
      Serial.print("\t\t");
      Serial.println(gps.getLongitudeBase10(), 6);
      // выводим количество видимых спутников
      Serial.print("Sat: ");
      Serial.println(gps.getSat());
      // выводим текущую скорость
      Serial.print("Speed: ");
      Serial.println(gps.getSpeed());
      // выводим высоту над уровнем моря
      Serial.print("Altitude: ");
      Serial.println(gps.getAltitude());
      // выводим текущее время
      Serial.print("Time: ");
      gps.getTime(time);
      gps.getDate(date);
      Serial.write(time);
      Serial.println();
      // выводим текущую дату
      Serial.print("Date: ");
      Serial.write(date);
      Serial.println("\r\n");
      // каждую переменную дату и времени можно выводить отдельно
/*    Serial.print(gps.getHour());
      Serial.print(gps.getMinute());
      Serial.print(gps.getSecond());
      Serial.print(gps.getDay());
      Serial.print(gps.getMonth());
      Serial.print(gps.getYear());
*/   
      break;
    // ошибка данных
    case GPS_ERROR_DATA:
      Serial.println("GPS error data");
      break;
    // нет соединение со спутниками
    case GPS_ERROR_SAT:
      Serial.println("GPS no connect to satellites");
      break;
  }
}
