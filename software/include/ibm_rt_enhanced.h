
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