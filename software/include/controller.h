
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

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "types.h"

#include <stdint.h>

typedef int (*get_expected_lbas_funcptr)(
    struct drive_parameters* drive_params,
    int cylinder,
    int head,
    int* expected_lbas
);

typedef int (*process_sector_funcptr)(
    struct drive_parameters* drive_params,
    struct raw_sector* raw,
    struct processed_sector* processed
); 

struct esdi_controller {
    char* name;

    int addr_area_assert;
    int addr_area_deassert;
    int data_area_assert;
    int data_area_deassert;
    
    uint8_t addr_area_sync_byte;
    uint8_t data_area_sync_byte;

    int addr_area_length;
    int data_area_length;
    int sector_size;

    get_expected_lbas_funcptr get_expected_lbas;
    process_sector_funcptr process_sector;
};

#endif // CONTROLLER_H