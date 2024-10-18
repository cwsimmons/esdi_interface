
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

#include "adaptec_acb2322.h"
#include "crc.h"
#include "util.h"
#include "types.h"

#include <string.h>
#include <stdio.h>

struct esdi_controller adaptec_acb2322 = {
    "ADAPTEC_ACB2322",
    4, 1972, 2270, 2270+42936,
    0xFE, 0xFE,
    8, 519, 512,
    adaptec_acb2322_get_expected_lbas,
    adaptec_acb2322_process_sector
};

int adaptec_acb2322_get_expected_lbas(
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

    if (cylinder >= (drive_params->cylinders - 2))
        return 0;

    int sectors_per_cylinder = drive_params->heads * drive_params->sectors;

    for (int i = 0; i < drive_params->sectors; i++) {
        expected_lbas[i] = (sectors_per_cylinder * cylinder) +
                           (drive_params->sectors * head) +
                           i;
    }

    return drive_params->sectors;
}

int adaptec_acb2322_process_sector(
    struct drive_parameters* drive_params,
    struct raw_sector* raw,
    struct processed_sector* processed
) {

    uint32_t header_check = crc16_msb(
        raw->address_area,
        adaptec_acb2322.addr_area_length,
        0x1021,
        0x0000
    );

    if (header_check)
        return -1;  // Bad addr area CRC

    struct chs chs = {
        .c = (raw->address_area[1] <<  8) | (raw->address_area[2]),
        .h = raw->address_area[3],
        .s = raw->address_area[4]
    };

    uint32_t data_check = crc48_msb(
        raw->data_area,
        adaptec_acb2322.data_area_length,
        0x181814503011,
        0x000000000000
    );

    if (data_check)
        return -3; // Bad data area CRC

    processed->lba = (chs.c * (drive_params->heads * drive_params->sectors)) +
                     (chs.h * (drive_params->sectors)) + 
                     (chs.s - 1);

    processed->length = adaptec_acb2322.sector_size;
    memcpy(processed->data, &(raw->data_area[1]), adaptec_acb2322.sector_size);

    return 0;
}
