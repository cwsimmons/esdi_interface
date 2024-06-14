
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

#ifndef DATAPATH_H
#define DATAPATH_H

#include "types.h"
#include "controller.h"

#include <stdint.h>
#include <stdbool.h>

extern volatile uint32_t* sector_timer_base;
extern volatile uint32_t* datapath_base;
extern volatile uint8_t* bram_base;
extern volatile uint32_t* dma_base;

enum area_type {
    ADDRESS_AREA,
    DATA_AREA
};

void datapath_configure(
    struct drive_parameters* drive_params,
    struct esdi_controller* controller_info
);

void datapath_sector_timer_set_enable(bool enable);
void datapath_sector_timer_reset();
void datapath_sector_timer_set_address_area_enable(bool enable);
void datapath_sector_timer_set_data_area_enable(bool enable);
void datapath_sector_timer_set_soft_sector(bool is_soft);
void reset_dma();
void dma_set_enable(bool enable);
void datapath_start();
void datapath_stop();
bool findbyte(volatile uint8_t* buffer, int length, uint8_t pattern, int* offset, int* bit);
void copy_buff_start_at(uint8_t* buffer_dest, volatile uint8_t* buffer_src, int length, int offset, int bit);
int read_area(int physical_sector, int area, uint8_t* data);
int read_track(int num_sectors, int* physical_sectors, struct raw_sector* raw_sectors);

#endif // DATAPATH_H