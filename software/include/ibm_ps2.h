
// IBM ESDI controller for Microchannel PS/2 machines
// P/N = 90X8063

#include "controller.h"

#include <stdint.h>
#include <stdbool.h>

extern struct esdi_controller ibm_ps2;

int ibm_ps2_get_expected_lbas(
    struct drive_parameters* drive_params,
    int cylinder,
    int head,
    int* expected_lbas
);

int ibm_ps2_process_sector(
    struct drive_parameters* drive_params,
    struct raw_sector* raw,
    struct processed_sector* processed
);
