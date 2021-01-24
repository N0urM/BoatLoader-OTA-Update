#include <string>
#include "BLCommunication.h"
#define HEADER_SIZE     6 
#define HANDSHAKE_MSG   "HS"

SoftwareSerial STM_Serial(STM_SERIAL_RX, STM_SERIAL_TX); // RX , TX


void BL_Init()
{
  STM_Serial.begin(115200);
}

void SendNewUpdateSignal(void)
{
  const uint8_t updateSignal[] = {'a' ,'s' ,'d' ,'f'};
  uint8_t i;
  for (i=0 ; i<4 ; i++)
  {
    STM_Serial.write(updateSignal[i]);
  }
  Serial.println("Signal sent.");
}

void WaitforHandshake(void)
{
  String HandShake = "";
  while (HandShake == "")
  {
    if (STM_Serial.available())
      HandShake = STM_Serial.readStringUntil('\n');
  }
  if (HandShake == HANDSHAKE_MSG)
    Serial.println("Hndshake recieved.");
  else 
  {
    Serial.print("Recived: ");
    Serial.println(HandShake);
  }
}

void SendToBootLoader(String FileName , int32_t CRC_Calc)
{
  fs::File file1 = SPIFFS.open(FileName, "r");
  if (file1)
  {
    // Sed data line by line and wait for Data check up
    uint8_t readChar1 , readchar2;
    String OKString;
    uint16_t DataSentLen = 0; 
    uint16_t ContentLen;
    int32_t CRCValue;
    uint8_t header [HEADER_SIZE];
    
    ContentLen = (file1.size()/2);
    CRCValue = CRC_Calc;
    header[5] = 0xFF & (uint8_t)CRCValue;
    header[4] = 0xFF & (uint8_t)(CRCValue>>8);
    header[3] = 0xFF & (uint8_t)(CRCValue>>16);
    header[2] = 0xFF & (uint8_t)(CRCValue>>24);
    header[1] = 0xFF & (uint8_t)ContentLen;
    header[0] = 0xFF & (uint8_t)(ContentLen>>8);
   
    Serial.print("Content Len: ");
    Serial.println(ContentLen , HEX);
    Serial.print("CRC: ");
    Serial.println(CRCValue , HEX);
    Serial.println("Attempt to send to STM.. ");

    uint8_t i=0;
    for (i=0 ; i<HEADER_SIZE ; i++)
    {
      STM_Serial.write(header[i]);
    }
    uint16 k =0;
    while (file1.available())   // Read from begining
    {
      readChar1 = file1.read();
      readchar2 = file1.read();
      uint8_t HEX_Byte = AsciiToHex(readChar1 , readchar2);
    // Serial.print(HEX_Byte , HEX);
    // Serial.print(" ");
      STM_Serial.write(HEX_Byte);
      DataSentLen +=1;
      if (DataSentLen % 1024 == 0 )
      {
        k++;
        OKString = STM_Serial.readString();  
        if ( OKString != "ok\n" && OKString != "ok" )
        {
          Serial.printf("Uploading Firmware Failed in chunck: %d\n" , k);
          Serial.printf("Data Sent Len: %d \n" , DataSentLen);
          Serial.print("BootLoader replied: ");
          Serial.println (OKString);
          break;
        }
      }

      if (!file1.available())   // Reached EOF
      {
        Serial.println("Firmware uploaded succesfully");
        // wait for BL confirmation
        OKString = "";
        while (OKString == "")
        {
          OKString = STM_Serial.readStringUntil('\n');
        } 
        if ( OKString == "ok")
        {
          EEPROM.write(UPDATE_UPLOAD_READY, 0);
          EEPROM.commit(); //Store data to EEPROM
          Serial.println("BL Verified succeessfully");
        }else {
          Serial.println("BL Verification Faild");
          Serial.printf("BL replied: ");
          Serial.print(OKString);
        }
      }
    }
    file1.close();
  } 
}
