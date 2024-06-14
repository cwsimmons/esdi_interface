
#include "ibm_rt_enhanced.h"
#include "crc.h"

#include <string.h>
#include <stdio.h>

struct esdi_controller ibm_rt_enhanced = {
    "IBM_RT_ENHANCED",
    242, 242+1940, 2270, 2270+42766,
    0xFE, 0xFE,
    9, 517,
    ibm_rt_get_expected_lbas,
    ibm_rt_process_sector
};

int ibm_rt_get_expected_lbas(
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

int ibm_rt_process_sector(
    struct drive_parameters* drive_params,
    struct raw_sector* raw,
    struct processed_sector* processed
) {

    uint32_t header_check = crc32_msb(
        raw->address_area,
        ibm_rt_enhanced.addr_area_length,
        0x41044185,
        0x00000000
    );

    if (header_check)
        return -1;  // Bad addr area CRC

    struct chs chs = {
        .c = (raw->address_area[1] <<  8) | (raw->address_area[2]),
        .h = raw->address_area[3],
        .s = raw->address_area[4]
    };

        // printf("CHS is [%d,%d,%d]\n", chs.c, chs.h, chs.s);
    if ((raw->cylinder != chs.c) || (raw->head != chs.h)) {
        return -2;  // We're didn't read this from where we though we did
    }

    uint32_t data_check = crc32_msb(
        raw->data_area,
        ibm_rt_enhanced.data_area_length,
        0x41044185,
        0x00000000
    );

    if (data_check)
        return -3; // Bad data area CRC

    processed->lba = (chs.c * (drive_params->heads * drive_params->sectors)) +
                     (chs.h * (drive_params->sectors)) + 
                     (chs.s - 1);       // Or maybe we should subtract 1 earlier

    processed->length = ibm_rt_enhanced.data_area_length - 5;
    memcpy(processed->data, &(raw->data_area[1]), ibm_rt_enhanced.data_area_length - 5);

    return 0;
}
