
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

#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include <stdint.h>

struct chs {
    int c,h,s;
};

struct drive_parameters {

    int heads;
    int cylinders;
    int sectors;

    bool is_soft_sectored;
    int sectors_hard;

};

struct __attribute__((packed)) drive_configuration {
    uint16_t general_configuration[20];
    uint16_t specific_configuration[15];
};

struct __attribute__((packed)) emulation_header {
    uint16_t file_version;
    uint32_t drive_configuration_offset;
    uint32_t data_offset;
    uint16_t cylinders;
    uint16_t heads;
    uint16_t sectors_per_track;
    uint16_t sector_size_in_image;
};

struct raw_sector {
    // Address
    int cylinder;
    int head;
    int physical_sector;

    // Info
    int status;

    bool address_read_ok;
    uint8_t* address_area;
    int address_start_location;

    bool data_read_ok;
    uint8_t* data_area;
    int data_start_location;
};

struct processed_sector {
    int lba;
    
    bool marked_bad;
    bool marked_spare;
    bool relocated;

    int length;
    uint8_t* data;
};

#endif // TYPES_H