#ifndef _SDCard_h
#define _SDCard_h


//#include "SolarEV.h"  // Có define debug để bật/tắt các debug ra Serial.

// Thư viện cho thẻ nhớ SD Card
//#include "FS.h"
#include <SD.h>
//-----------------------------------------------------------------
// Giao tiếp giữa ESP32 với SD_Card là giao thức SPI.
//-----------------------------------------------------------------
#include <SPI.h>

#define CS_PIN   22
#define MOSI_PIN 23
#define SCK_PIN  18
#define MISO_PIN 19
//-----------------------------------------------------------------


class SD_Card {
private:
  File myFile;
public:
  void KhoiTaoSD_Card(void);
  void WriteToFile(String Data);
  void GetDataFromFile(String* arr, int arrSize);
  unsigned long SoDongTrongFile(void);
  void DeleteFile(void);
};


#endif