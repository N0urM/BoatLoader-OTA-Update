#ifndef BL_COM
#define BL_COM

#include "Arduino.h"
#include <SoftwareSerial.h>
#include <FS.h>
#include <LittleFS.h>
#include <EEPROM.h>
#include "config.h"

/************* Public Functions *****************/

/* initiate UART communication */
void BL_Init();

/* Send file: FileName to STM via UART SW channel
   Defined in Config File 
        STM_SERIAL_RX              D1   
        STM_SERIAL_TX              D2
 */
void SendToBootLoader(String FileName , int32_t CRC_Calc);

/*
   Function: Notify application running on STM 
             that a new update is available.  
*/
void SendNewUpdateSignal(void);


/*
   Make sure BL is ready and listening before sending the actual data
   Note: This function has an infinite loop.
*/
void WaitforHandshake(void);

/*************** Private functions ****************/
uint8_t AsciiToHex(uint8_t readchar1 ,uint8_t readchar2);

#endif      // EOF