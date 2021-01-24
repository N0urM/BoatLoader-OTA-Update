#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>

#include "FIREBASE_Communication.h"
#include "CRC32_MPEG.h"
#include "File_operation.h"
#include "BLCommunication.h"
#include "config.h"

void WIFIConfigure();
void ArduinoOTA_Config();

void setup()
{
  
  Serial.begin(115200);

  ArduinoOTA_Config();
  ArduinoOTA.begin();

  BL_Init();
  SPIFFS.begin();
  EEPROM.begin(512);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  WIFIConfigure();
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  FireBaseConfig();  
  
  /*************************************/
  //listDeleteAllFiles();
  //updateEEPROM(VERSION_ADDRESS , 0);
  /*************************************/
  String FileName;
  int32_t CRC_Cal ; 
  int32_t * CRC_ptr = & CRC_Cal;
  // If update availabe, get it. OtherWise, Get the latest version downloaded.
  if (FireBaseCheckForUpdate() == 1)
  {
    FileName = handleFireBaseConnection(CRC_ptr);
  }
  else
  {
    Serial.println("No Available Update on DB");
    FileName = LastFileName();
  }
  
  // If there is an update downloaded from DB and not successfully uploaded to STM, attempt to send
  if(EEPROM.read (UPDATE_UPLOAD_READY) != 0  && FileName != "")
  {

    Serial.println("Update Ready.. ");
    Serial.print("Version File: ");
    Serial.println(FileName);
   // PrintFile(FileName);
    CRC_Cal = GET_STM_CRC();
    Serial.print("DB CRC: ");
    Serial.println(CRC_Cal , HEX);
    SendNewUpdateSignal();
    WaitforHandshake();
    SendToBootLoader(FileName  , CRC_Cal);
  }
}

void loop() {
  ArduinoOTA.handle();
}

void WIFIConfigure()
{
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}

void ArduinoOTA_Config(){
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });

}