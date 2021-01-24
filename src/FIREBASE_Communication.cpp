#include "Arduino.h"
#include "CRC32_MPEG.h"
#include "FIREBASE_Communication.h"
#include "File_operation.h"
#include "config.h"
//#include "time.h"


/******************* Publilc variables ***********************/
/*
static const char digicert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDujCCAqKgAwIBAgILBAAAAAABD4Ym5g0wDQYJKoZIhvcNAQEFBQAwTDEgMB4G
A1UECxMXR2xvYmFsU2lnbiBSb290IENBIC0gUjIxEzARBgNVBAoTCkdsb2JhbFNp
Z24xEzARBgNVBAMTCkdsb2JhbFNpZ24wHhcNMDYxMjE1MDgwMDAwWhcNMjExMjE1
MDgwMDAwWjBMMSAwHgYDVQQLExdHbG9iYWxTaWduIFJvb3QgQ0EgLSBSMjETMBEG
A1UEChMKR2xvYmFsU2lnbjETMBEGA1UEAxMKR2xvYmFsU2lnbjCCASIwDQYJKoZI
hvcNAQEBBQADggEPADCCAQoCggEBAKbPJA6+Lm8omUVCxKs+IVSbC9N/hHD6ErPL
v4dfxn+G07IwXNb9rfF73OX4YJYJkhD10FPe+3t+c4isUoh7SqbKSaZeqKeMWhG8
eoLrvozps6yWJQeXSpkqBy+0Hne/ig+1AnwblrjFuTosvNYSuetZfeLQBoZfXklq
tTleiDTsvHgMCJiEbKjNS7SgfQx5TfC4LcshytVsW33hoCmEofnTlEnLJGKRILzd
C9XZzPnqJworc5HGnRusyMvo4KD0L5CLTfuwNhv2GXqF4G3yYROIXJ/gkwpRl4pa
zq+r1feqCapgvdzZX99yqWATXgAByUr6P6TqBwMhAo6CygPCm48CAwEAAaOBnDCB
mTAOBgNVHQ8BAf8EBAMCAQYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUm+IH
V2ccHsBqBt5ZtJot39wZhi4wNgYDVR0fBC8wLTAroCmgJ4YlaHR0cDovL2NybC5n
bG9iYWxzaWduLm5ldC9yb290LXIyLmNybDAfBgNVHSMEGDAWgBSb4gdXZxwewGoG
3lm0mi3f3BmGLjANBgkqhkiG9w0BAQUFAAOCAQEAmYFThxxol4aR7OBKuEQLq4Gs
J0/WwbgcQ3izDJr86iw8bmEbTUsp9Z8FHSbBuOmDAGJFtqkIk7mpM0sYmsL4h4hO
291xNBrBVNpGP+DTKqttVCL1OmLNIG+6KYnX3ZHu01yiPqFbQfXf5WRDLenVOavS
ot+3i9DAgBkcRcAtjOj4LaR0VknFBbVPFd5uRHg5h6h+u/N5GJG79G+dwfCMNYxd
AfvDbbnvRG15RjF+Cv6pgsH/76tuIMRQyV+dTZsXjAzlAcmgQWpzU/qlULRuJQ/7
TBj0/VLZjmmx6BEP3ojY+x1J96relc8geMJgEtslQIxq/H5COEBkEveegeGTLg==
-----END CERTIFICATE-----
)EOF";
*/

// FireBase FingerPrint
//const uint8_t FingerPrint[] = {0x27 ,0x78 ,0x0c, 0x49, 0xc2, 0xe6, 0x4a, 0x08, 0x20, 0x8e, 0x1b, 0x12, 0x6e, 0x5e, 0x41, 0xd1, 0x69, 0xc3, 0x93, 0x6f};
// Mega FingerPrint
const uint8_t FingerPrint[]  = {0x4b, 0xee, 0x08, 0x8b, 0xa5, 0xfc, 0xea, 0xc5, 0x73, 0x17, 0xfe, 0x75, 0x43, 0x82, 0x33, 0xc1 ,0xc3 ,0xc9 ,0xcc ,0x26};
fs::File file;
FirebaseData firebaseDataUpdateValue;
FirebaseData firebaseDataFile;
uint8_t currentVersion = 0;
uint8_t dbVersion = 0;
String DBFilePath;
String FileName = "/version";

/******************* Publilc functions ***********************/
void FireBaseConfig()
{
  Firebase.reconnectWiFi(true);

  //Set the size of WiFi rx/tx buffers in the case where we want to work with large data.
  firebaseDataFile.setBSSLBufferSize(1024, 1024);

  //Set the size of https response buffers in the case where we want to work with large data.
  firebaseDataFile.setResponseSize(1024);

  //Set database read timeout to 1 minute (max 15 minutes)
  Firebase.setReadTimeout(firebaseDataFile, 1000 * 60);
  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Firebase.setwriteSizeLimit(firebaseDataFile, "large");
}

String handleFireBaseConnection(int32_t * CRC_CALC)
{
  FileName = FileName + (String)dbVersion + ".txt";
  Serial.print("FileName: ");
  Serial.println(FileName);

  if (Firebase.get(firebaseDataUpdateValue, "/UpdateLink"))
  {
    // Get File path
    DBFilePath = firebaseDataUpdateValue.stringData();
  //  Serial.print("File URL: ");
  //  Serial.println(DBFilePath);
  }  

  downloadFirmware();
  
  if (CRC32File(FileName , CRC_CALC)) // If CRC successfull 
  {
    Serial.println("Update Recieved successfully");
    updateEEPROM(VERSION_ADDRESS , dbVersion);
    updateEEPROM(UPDATE_UPLOAD_READY , 1);
    return FileName;
  }
  else
  {
    Serial.println("Update Failed");
    return "";  // Error sign
  }
  
}

bool FireBaseCheckForUpdate ()
{
  currentVersion = EEPROM.read(VERSION_ADDRESS);
  if (currentVersion == 0xFF)
  {  
    currentVersion = 0;
  }
  Serial.print("CurrentVersion: ");
  Serial.println(currentVersion);

  if (Firebase.get(firebaseDataUpdateValue, "/UpdateVersion"))
    dbVersion = firebaseDataUpdateValue.intData();
  Serial.print("dbVersion: ");
  Serial.println(dbVersion);

  if (dbVersion > currentVersion){
    return 1;
  }
  return 0;
}

/****************** Private Functions ****************/

void setTime(){

  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));

}

void downloadFirmware()
{
  // Init Secure connection
  std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
  client->setFingerprint(FingerPrint);
  //BearSSL::X509List cert(digicert);
  //setTime();
  //client->setTrustAnchors(&cert);
  HTTPClient https;
  // If version downloaded before, remove it
  if(SPIFFS.exists(FileName))
    SPIFFS.remove(FileName);
  file = SPIFFS.open(FileName, "w");

  // Start pulling down the firmware binary.
  if (file)   // File opened successfully 
  {

    https.begin(*client, DBFilePath);
    // Fetch File from secure connesction
    int httpsCode = https.GET();
    if (httpsCode <= 0)
    {
      Serial.printf("https failed, error: %s\n",
      https.errorToString(httpsCode).c_str());
      return;
    }
    // Check that we have enough space for the new binary.
    uint32_t contentLen = https.getSize();
    if (contentLen < 1){
      Serial.println("Download Failed");
      return;
    }
    else {
      Serial.printf("Content-Length: %d\n", contentLen);

    }
    // Store into SPIFFS
    https.writeToStream(&file);
    file.close();
    https.end();
  }
  else
  {
    Serial.println("couldn't open file");
  }
}

bool CRC32File(String localFileName , int32_t * CRC_Calculated)
{
  uint32_t crc = 0;
  file = SPIFFS.open(localFileName, "r");
  uint8_t readChar[2];
  uint8_t HEX_byte;
  // Clear previous CRC values
  CRC32_MPEG_reset();
  while (file.available())
  {
    readChar[0] = file.read();     // Read Next char
    readChar[1] = file.read();
    HEX_byte = AsciiToHex(readChar[0] , readChar[1]);
    crc = Compute_CRC32_HEX(HEX_byte);       // Add to CRC
  }

  String CRCDBString;
  int32 CRCDBVal = 0;
  if (Firebase.get(firebaseDataUpdateValue, "/CRC32Val")) // value in int32 big indian 
  {
    CRCDBString = firebaseDataUpdateValue.stringData();   // Get STored CRC from DB
  }
  CRCDBVal = atoi(CRCDBString.c_str());                   // Convert to integer
  *CRC_Calculated = crc;
  if (CRCDBVal == crc)                               // Compare stored and calculated
  {
    Serial.println("Data Validation Successfull");
    return 1;     // Success
  }
  else
  {
    Serial.println("Data Validation Error");
    Serial.print("Expected Value: ");
    Serial.println(CRCDBVal, HEX);
    Serial.print("Calculated Value: ");
    Serial.println(crc, HEX);
    return 0;   // Error Code
  }
}

void updateEEPROM(uint32 address , uint8 value)
{
  EEPROM.write(address, value);
  EEPROM.commit(); //Store data to EEPROM
//  Serial.print("EEPROM Value: ");
//  Serial.println(EEPROM.read(address));
}

int32_t GET_STM_CRC()
{
  int32_t STMFileCRCVal;
  String STMFileCRCString;
  if (Firebase.get(firebaseDataUpdateValue, "/CRC32Val"))
  {
    // Get STM CRC
    STMFileCRCString = firebaseDataUpdateValue.stringData();
  }
  STMFileCRCVal = atoi(STMFileCRCString.c_str()); 
  return STMFileCRCVal;

}
