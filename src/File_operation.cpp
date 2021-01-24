#include "Arduino.h"
#include "File_operation.h"

fs::File file1;
String FileName1;


void PrintFile(String localFileName)
{
  file1 = SPIFFS.open(localFileName, "r");
  char i = 0;
  unsigned char readChar;
  while (file1.available())
  {
    readChar = file1.read();    // Read Next Byte
    /* Format Style and print to HW Serial */
    if (readChar < 0x10)
    {
      Serial.print('0');      
    }
    Serial.print(readChar, HEX);
    Serial.print(' ');
    if (i++ > 15)
    {
      i = 0;
      Serial.println();
    }
  }
}

void listDeleteAllFiles()
{

  Dir dir = SPIFFS.openDir("/");    // Open root Directory
  while (dir.next())
  {

    Serial.print("FILE: ");
    Serial.println(dir.fileName());   // List
    SPIFFS.remove(dir.fileName());    // & Delete  
  }
}

void listAllFiles()
{

  Dir dir = SPIFFS.openDir("/");
  while (dir.next())
  {
    Serial.print("FILE: ");
    Serial.println(dir.fileName());   //List
  }
}

String LastFileName()
{ 
  Dir dir = SPIFFS.openDir("/");
  String FileName ;
  while (dir.next()){
    FileName = dir.fileName();
  }  
  return FileName;      // Last found File path (Name)
}


uint8_t AsciiToHex(uint8_t readchar1 ,uint8_t readchar2)
{

  uint8_t HEX_Val = 0 ;
  if (readchar1 >= '0' && readchar1 <= '9')
  {
    HEX_Val = ((readchar1 - '0')<<4);
  }else 
  {
    HEX_Val = ((readchar1 - 'A' + 10)<<4);
  }

  if(readchar2 >= '0' && readchar2 <= '9')
  {
    HEX_Val+= (readchar2-'0') ;
  }else 
  {
    HEX_Val += (readchar2 -'A' +10) ;
  }

  return HEX_Val;

}

