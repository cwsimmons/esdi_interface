
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

#include "ibm_5364.h"
#include "crc.h"
#include "util.h"

#include <string.h>
#include <stdio.h>

struct esdi_controller ibm_5364 = {
    "IBM_5364",
    242, 242+1940, 2270, 2270+21918+160+60,
    0xFE, 0xF8,
    8, 263, 256,
    ibm_5364_get_expected_lbas,
    ibm_5364_process_sector
};

int ibm_5364_get_expected_lbas(
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

    int sectors_per_cylinder = (drive_params->heads * (drive_params->sectors - 1));

    for (int i = 0; i < drive_params->sectors - 1; i++) {
        expected_lbas[i] = (sectors_per_cylinder * cylinder) +
                           ((drive_params->sectors - 1) * head) +
                           i;
    }

    return drive_params->sectors - 1;
}

int ibm_5364_process_sector(
    struct drive_parameters* drive_params,
    struct raw_sector* raw,
    struct processed_sector* processed
) {

    uint32_t header_check = crc16_msb(
        &raw->address_area[1],
        ibm_5364.addr_area_length-1,
        0x1021,
        0xffff
    );

    if (header_check)
        return -1;  // Bad addr area CRC

    if (raw->address_area[4] == 0xFF) {
        processed->marked_spare = true;
        return 0;
    } else {
        processed->marked_spare = false;
    }

    struct chs chs = {
        .c = (raw->address_area[1] <<  8) | (raw->address_area[2]),
        .h = raw->address_area[3],
        .s = raw->address_area[4]
    };

    uint32_t data_check = crc48_msb(
        &raw->data_area[1],
        ibm_5364.data_area_length-1,
        0x100b0001100b,
        0xffffffffffff
    );

    if (data_check)
        return -3; // Bad data area CRC

    processed->lba = (chs.c * (drive_params->heads * (drive_params->sectors - 1))) +
                     (chs.h * (drive_params->sectors - 1)) + 
                     (chs.s);       // These sector numbers from the header are zero based

    processed->length = ibm_5364.sector_size;
    memcpy(processed->data, &(raw->data_area[1]), ibm_5364.sector_size);

    return 0;
}
