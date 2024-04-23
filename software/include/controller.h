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

    get_expected_lbas_funcptr get_expected_lbas;
    process_sector_funcptr process_sector;
};

#endif // CONTROLLER_H