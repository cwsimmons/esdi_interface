#ifndef CRC_H
#define CRC_H

#include <stdint.h>

uint32_t crc32_msb(uint8_t* data, int length, uint32_t poly, uint32_t init);
uint32_t crc32_lsb(uint8_t* data, int length, uint32_t poly, uint32_t init);
uint16_t crc16_msb(uint8_t* data, int length, uint16_t poly, uint16_t init);
uint16_t crc16_lsb(uint8_t* data, int length, uint16_t poly, uint16_t init);

#endif // CRC_H