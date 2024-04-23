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