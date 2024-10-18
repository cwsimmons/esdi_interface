
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

#include "ibm_ps2.h"
#include "crc.h"

#include <string.h>
#include <stdio.h>

struct esdi_controller ibm_ps2 = {
    "IBM_PS2",
    408, 408+1872, 2540, 2540+42636,
    0xA1, 0xA1,
    10, 517, 512,
    ibm_ps2_get_expected_lbas,
    ibm_ps2_process_sector
};

struct chs ibm_ps2_lba_to_chs(struct drive_parameters* drive_params, int lba) {
    struct chs retval;
    int sectors_per_cylinder = (drive_params->heads * drive_params->sectors) - 4;

    retval.c = lba / sectors_per_cylinder; // Integer division

    int sectors_into_cylinder = lba % sectors_per_cylinder;

    retval.h = sectors_into_cylinder / drive_params->sectors; // Integer division
    retval.s = sectors_into_cylinder % drive_params->sectors;

    return retval;
}

int ibm_ps2_get_expected_lbas(
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

    // Fourth to last cylinder is all CE blocks
    // Third to last cylinder is all secondary map blocks
    // Second to last cylinder is all primary map blocks
    // Last cylinder is unused?

    if (cylinder >= (drive_params->cylinders - 4))
        return 0;

    // For some reason this controller does not completely
    // fill up the highest platter.
    // Need to try different disk sizes to see how this value is actually computed.
    int sectors_per_cylinder = (drive_params->heads * drive_params->sectors) - 4;
    int num_sectors = (head < 6) ? drive_params->sectors : drive_params->sectors - 4;


    for (int i = 0; i < num_sectors; i++) {
        expected_lbas[i] = (sectors_per_cylinder * cylinder) +
                           (drive_params->sectors * head) +
                           i;
    }

    return num_sectors;
}

int ibm_ps2_process_sector(
    struct drive_parameters* drive_params,
    struct raw_sector* raw,
    struct processed_sector* processed
) {

    uint32_t header_check = crc32_msb(
        raw->address_area,
        ibm_ps2.addr_area_length,
        0x41044185,
        0x00000000
    );

    if (header_check)
        return -1;  // Bad addr area CRC

    int lba = (raw->address_area[1] <<  0) |
              (raw->address_area[2] <<  8) |
              (raw->address_area[3] << 16) |
              ((raw->address_area[4] & 0x0F) << 24);


    if (lba == 0x0F7F7F7F) {
        processed->marked_spare = true;
        return 0;
    } else {
        processed->marked_spare = false;
    }

    processed->marked_bad = (raw->address_area[4] & 0x80) ? true : false;

    if (processed->marked_bad) {
        return 0;
    }

    struct chs chs = ibm_ps2_lba_to_chs(drive_params, lba);


    uint32_t data_check = crc32_msb(
        raw->data_area,
        ibm_ps2.data_area_length,
        0x41044185,
        0x00000000
    );

    if (data_check)
        return -3; // Bad data area CRC

    processed->lba = lba;
    processed->length = ibm_ps2.sector_size;
    memcpy(processed->data, &(raw->data_area[1]), ibm_ps2.sector_size);

    return 0;
}
