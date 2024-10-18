
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

// Adaptec ACB-2322

#ifndef ADAPTEC_ACB2322_H
#define ADAPTEC_ACB2322_H

#include "controller.h"

#include <stdint.h>
#include <stdbool.h>

extern struct esdi_controller adaptec_acb2322;

int adaptec_acb2322_get_expected_lbas(
    struct drive_parameters* drive_params,
    int cylinder,
    int head,
    int* expected_lbas
);

int adaptec_acb2322_process_sector(
    struct drive_parameters* drive_params,
    struct raw_sector* raw,
    struct processed_sector* processed
);

#endif // ADAPTEC_ACB2322_H