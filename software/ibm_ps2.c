
#include "ibm_ps2.h"
#include "crc.h"

#include <string.h>
#include <stdio.h>

struct esdi_controller ibm_ps2 = {
    "IBM_PS2",
    408, 408+1872, 2540, 2540+42636,
    0xA1, 0xA1,
    10, 517,
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
              (raw->address_area[4] << 24);

    struct chs chs = ibm_ps2_lba_to_chs(drive_params, lba);

    if ((raw->cylinder != chs.c) || (raw->head != chs.h)) {
        printf("LBA looks like [%d,%d,%d]\n", chs.c, chs.h, chs.s);
        return -2;  // We're didn't read this from where we though we did
    }

    uint32_t data_check = crc32_msb(
        raw->data_area,
        ibm_ps2.data_area_length,
        0x41044185,
        0x00000000
    );

    if (data_check)
        return -3; // Bad data area CRC

    processed->lba = lba;
    processed->length = ibm_ps2.data_area_length - 5;
    memcpy(processed->data, &(raw->data_area[1]), ibm_ps2.data_area_length - 5);

    return 0;
}
