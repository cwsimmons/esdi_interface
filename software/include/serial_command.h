
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

#ifndef SERIAL_COMMAND_H
#define SERIAL_COMMAND_H

#include <stdint.h>
#include "types.h"
#include "serial_command_values.h"

extern volatile uint32_t* serial_command_base;
extern int serial_retries;

int serial_query(uint16_t command, uint16_t* response);
int serial_command(uint16_t command);
int serial_query_with_retries(uint16_t command, uint16_t* response);

int serial_query_drive_parameters(struct drive_parameters* parameters);

#endif // SERIAL_COMMAND_H