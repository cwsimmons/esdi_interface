
/* 

    Copyright 2024 Christopher Simmons

  This program is free software: you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the Free
  Software Foundation, either version 2 of the License, or (at your option)
  any later version.

  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along
  with this program. If not, see <https://www.gnu.org/licenses/>.

*/

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

uint64_t crc56_msb(uint8_t* data, int length, uint64_t poly, uint64_t init) {

    uint64_t remainder = init;

    for (int i = 0; i < length; i++) {
        remainder ^= ((uint64_t) data[i]) << 48;

        for (int j = 0; j < 8; j++) {
            if (remainder & 0x80000000000000) {
                remainder = (remainder << 1) ^ poly;
            } else {
                remainder = (remainder << 1);
            }
        }
    }

    return remainder & 0xffffffffffffff;
}
