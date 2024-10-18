
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

#include "ultrastor_12f.h"
#include "crc.h"
#include "util.h"

#include <string.h>
#include <stdio.h>

struct esdi_controller ultrastor_12f = {
    "ULTRASTOR_12F",
    161+500, 161+1293+500, 1513+500, 1513+28510+500+160,
    0xa1, 0xa1,
    9, 521, 512,
    ultrastor_12f_get_expected_lbas,
    ultrastor_12f_process_sector
};

int ultrastor_12f_get_expected_lbas(
    struct drive_parameters* drive_params,
    int cylinder,
    int head,
    int* expected_lbas
) {

    if (expected_lbas == NULL) {
        return -1;
    }

    if ((cylinder >= drive_params->cylinders) || (head >= drive_params->heads))
        return -1;

    int sectors_per_cylinder = drive_params->heads * drive_params->sectors;

    for (int i = 0; i < drive_params->sectors; i++) {
        expected_lbas[i] = (sectors_per_cylinder * cylinder) +
                           (drive_params->sectors * head) +
                           i;
    }

    return drive_params->sectors;
}

int ultrastor_12f_process_sector(
    struct drive_parameters* drive_params,
    struct raw_sector* raw,
    struct processed_sector* processed
) {

    uint32_t header_check = crc16_msb(
        raw->address_area,
        ultrastor_12f.addr_area_length,
        0x1021,
        0xFFFF
    );

    if (header_check)
        return -1;  // Bad addr area CRC

    uint8_t cyl_high_raw = raw->address_area[1];
    int clyinder_high_byte = -1;

    // The high order byte of the cylinder is weird. Here's a lookup table
    uint8_t cylinder_high_byte_map[7] = {0xFE, 0xFF, 0xFC, 0xFD, 0xF6, 0xF7, 0xF4};
    for (int i = 0; i < 7; i++) {
        if (cylinder_high_byte_map[i] == cyl_high_raw)
            clyinder_high_byte = i;
    }
    if (clyinder_high_byte == -1) {
        return -4;
    }

    struct chs chs = {
        .c = (clyinder_high_byte <<  8) | (raw->address_area[2]),
        .h = (raw->address_area[3] & 0x1F),
        .s = raw->address_area[4]
    };

    if ((raw->cylinder != chs.c) || (raw->head != chs.h)) {
        return -2;  // We're didn't read this from where we though we did
    }

    uint32_t data_check = crc56_msb(
        raw->data_area,
        ultrastor_12f.data_area_length,
        0x140a0445000101,
        0xffffffffffffff
    );

    if (data_check)
        return -3; // Bad data area CRC

    processed->lba = (chs.c * (drive_params->heads * drive_params->sectors)) +
                     (chs.h * (drive_params->sectors)) + 
                     (chs.s - 1);

    processed->length = ultrastor_12f.sector_size;
    memcpy(processed->data, &(raw->data_area[2]), ultrastor_12f.sector_size);

    return 0;
}
