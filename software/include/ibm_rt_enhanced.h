
// IBM Enhanced ESDI controller for RT
// P/N = ?

#ifndef IBM_RT_ENHANCED_H
#define IBM_RT_ENHANCED_H

#include "controller.h"

#include <stdint.h>
#include <stdbool.h>

extern struct esdi_controller ibm_rt_enhanced;

int ibm_rt_get_expected_lbas(
    struct drive_parameters* drive_params,
    int cylinder,
    int head,
    int* expected_lbas
);

int ibm_rt_process_sector(
    struct drive_parameters* drive_params,
    struct raw_sector* raw,
    struct processed_sector* processed
);

#endif // IBM_RT_ENHANCED_H