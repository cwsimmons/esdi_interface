
#include "crc.h"


uint32_t crc32_msb(uint8_t* data, int length, uint32_t poly, uint32_t init) {
    
    uint32_t remainder = init;

    for (int i = 0; i < length; i++) {
        remainder ^= ((uint32_t) data[i]) << 24;

        for (int j = 0; j < 8; j++) {
            if (remainder & 0x80000000) {
                remainder = (remainder << 1) ^ poly;
            } else {
                remainder = (remainder << 1);
            }
        }
    }

    return remainder;
}

uint32_t crc32_lsb(uint8_t* data, int length, uint32_t poly, uint32_t init) {

    uint32_t remainder = init;

    for (int i = 0; i < length; i++) {
        remainder ^= data[i];

        for (int j = 0; j < 8; j++) {
            if (remainder & 0x01) {
                remainder = (remainder >> 1) ^ poly;
            } else {
                remainder = (remainder >> 1);
            }
        }
    }

    return remainder;
}

uint16_t crc16_msb(uint8_t* data, int length, uint16_t poly, uint16_t init) {
    
    uint16_t remainder = init;

    for (int i = 0; i < length; i++) {
        remainder ^= ((uint16_t) data[i]) << 8;

        for (int j = 0; j < 8; j++) {
            if (remainder & 0x8000) {
                remainder = (remainder << 1) ^ poly;
            } else {
                remainder = (remainder << 1);
            }
        }
    }

    return remainder;
}

uint16_t crc16_lsb(uint8_t* data, int length, uint16_t poly, uint16_t init) {

    uint16_t remainder = init;

    for (int i = 0; i < length; i++) {
        remainder ^= data[i];

        for (int j = 0; j < 8; j++) {
            if (remainder & 0x01) {
                remainder = (remainder >> 1) ^ poly;
            } else {
                remainder = (remainder >> 1);
            }
        }
    }

    return remainder;
}
