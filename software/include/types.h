
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

struct raw_sector {
    // Address
    int cylinder;
    int head;
    int physical_sector;

    // Info
    int status;

    bool address_read_ok;
    uint8_t* address_area;

    bool data_read_ok;
    uint8_t* data_area;
};

struct processed_sector {
    int lba;
    
    int length;
    uint8_t* data;
};

#endif // TYPES_H