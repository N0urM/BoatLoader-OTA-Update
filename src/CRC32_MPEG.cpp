#include "Arduino.h"
#include "CRC32_MPEG.h"

uint32_t crc = 0xFFFFFFFF;

// 2 bytes at a time
uint32_t Compute_CRC32_HEX(uint8_t HEX_byte)
{
    const uint32_t polynomial = 0x04C11DB7; // divisor 32bit //
    uint8_t b = HEX_byte;
    crc ^= (uint)(b << 24); /* move byte into MSB of 32bit CRC */

    for (int i = 0; i < 8; i++)
    {
        if ((crc & 0x80000000) != 0) /* test for MSB = bit 31 */
        {
            crc = (uint)((crc << 1) ^ polynomial);
        }
        else
        {
            crc <<= 1;
        }
    }
    return crc;
}

void CRC32_MPEG_reset()
{
    crc = 0xFFFFFFFF;
}