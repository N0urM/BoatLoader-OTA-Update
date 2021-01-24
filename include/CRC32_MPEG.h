#ifndef CRC32_MPEG_H
#define CRC32_MPEG_H


/*  Initialize CRC Value 
    Must be used before calculating CRC
*/
void CRC32_MPEG_reset();

/*
    Accumilate the previous CRC with the new data added,
    until a reset occurs.  
*/
uint32_t Compute_CRC32_HEX(uint8_t HEX_byte);

#endif