#ifndef FOP
#define FOP

#include "Arduino.h"
#include <FS.h>
#include <LittleFS.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>

/************* Public Functions *****************/

// List all files in SPIFFS on HW Serial
void listAllFiles();

// List and Delete All files in SPIFFS
void listDeleteAllFiles();

// Print File to HW Serial 
void PrintFile(String localFileName);

// Get the latest downloaded File
String LastFileName();

uint8_t AsciiToHex(uint8_t readchar1 ,uint8_t readchar2);

#endif      // EOF