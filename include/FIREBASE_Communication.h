#ifndef FB_COM
#define FB_COM

#include "Arduino.h"
#include <FirebaseESP8266.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <FS.h>
#include <LittleFS.h>
#include <EEPROM.h>
#include "config.h"

/******** Public Functions ********/
/*
    Init FIREBASE 
    Must be used in Setup before any attempt to download
*/
void FireBaseConfig();

/*
    Return 1 if new version found 
    Return 0 If no update found 
*/
bool FireBaseCheckForUpdate ();

/*
    initiate secure connection , 
    download file , 
    Validate data using CRC32 
    return Version Filename if success empty string on failure  
*/
String handleFireBaseConnection(int32_t * CRC_CALC);


/******** Private Functions ********/
void downloadFirmware();
void updateEEPROM(uint32 address , uint8 value);
bool CRC32File(String localFileName , int32_t * CRC_CALC);
int32_t GET_STM_CRC();

#endif