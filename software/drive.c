
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

#include "drive.h"
#include "serial_command.h"

void set_drive_select(uint8_t ds) {
    if (serial_command_base == 0) {
        return;
    }

    serial_command_base[2] = (uint32_t) ds & 0xF;
}

void set_head_select(uint8_t hs) {
    if (serial_command_base == 0) {
        return;
    }

    serial_command_base[3] = (uint32_t) hs & 0xF;
}

void drive_reset() {
    serial_command(CMD_CONTROL | CMD_CONTROL_MOD_RESET);
}

void drive_seek(int cyl) {
    serial_command(cyl & 0x0FFF);
}
