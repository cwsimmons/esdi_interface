
#include "datapath.h"
#include "util.h"

#include <unistd.h>
#include <time.h>
#include <stdio.h>

#define DMA_CTRL (0x30 >> 2)
#define DMA_STAT (0x34 >> 2)
#define DMA_ADDR (0x48 >> 2)
#define DMA_LEN  (0x58 >> 2)

volatile uint32_t* sector_timer_base = 0;
volatile uint32_t* datapath_base = 0;
volatile uint8_t* bram_base = 0;
volatile uint32_t* dma_base = 0;

struct drive_parameters* dp_drive_params = 0;
struct esdi_controller* dp_controller_info = 0;

void datapath_sector_timer_set_enable(bool enable) {
    if (enable)
        sector_timer_base[0] |= 0x1;
    else
        sector_timer_base[0] &= ~0x1;
}

void datapath_sector_timer_reset() {
    uint32_t tmp = sector_timer_base[0];
    tmp |= 0x02;
    sector_timer_base[0] = tmp;
    sector_timer_base[0] = tmp & ~0x02;
}

void datapath_sector_timer_set_address_area_enable(bool enable) {
    if (enable)
        sector_timer_base[0] |= 0x4;
    else
        sector_timer_base[0] &= ~0x4;
}

void datapath_sector_timer_set_data_area_enable(bool enable) {
    if (enable)
        sector_timer_base[0] |= 0x8;
    else
        sector_timer_base[0] &= ~0x8;
}

void datapath_sector_timer_set_soft_sector(bool is_soft) {
    if (is_soft)
        sector_timer_base[0] |= 0x20;
    else
        sector_timer_base[0] &= ~0x20;
}

void reset_dma() {
    dma_base[DMA_CTRL] = 0x2;
    // while(dma[DMA_CTRL] & 0x02) {}
    dma_base[DMA_CTRL] &= ~0x2;
}

void dma_set_enable(bool enable) {
    uint32_t tmp = dma_base[DMA_CTRL];
    tmp &= ~0x01;
    if (enable)
        tmp |= 0x01;
    dma_base[DMA_CTRL] = tmp;
}

void datapath_configure(
    struct drive_parameters* drive_params,
    struct esdi_controller* controller_info
) {
    dp_drive_params = drive_params;
    dp_controller_info = controller_info;

    sector_timer_base[0] = 0;

    datapath_sector_timer_set_soft_sector(drive_params->is_soft_sectored);

    sector_timer_base[2] = controller_info->addr_area_assert;
    sector_timer_base[3] = controller_info->addr_area_deassert;
    sector_timer_base[4] = controller_info->data_area_assert;
    sector_timer_base[5] = controller_info->data_area_deassert;

}

void datapath_start() {
    reset_dma();
    dma_set_enable(true);
    while (dma_base[DMA_STAT] & 0x01) {}
    datapath_base[0] = 0x3;
    datapath_sector_timer_reset();
    datapath_sector_timer_set_enable(true);

}

void datapath_stop() {
    datapath_base[0] = 0x2;
    datapath_sector_timer_set_enable(false);
    dma_set_enable(false);
}

bool findbyte(volatile uint8_t* buffer, int length, uint8_t pattern, int* offset, int* bit) {

    for (int i = 0; i < length; i++) {

        for (int j = 0; j < 8; j++) {


            if (j == 0) {
                if (buffer[i] == pattern) {
                    *offset = i;
                    *bit = 0;
                    return true;
                }
            } else if (i != (length - 1)) {

                uint8_t front = buffer[i] << j;
                uint8_t back = buffer[i+1] >> (8 - j);
                if ((front | back) == pattern) {
                    *offset = i;
                    *bit = j;
                    return true;
                }

            }

        }

    }

    return false;
}

void copy_buff_start_at(uint8_t* buffer_dest, volatile uint8_t* buffer_src, int length, int offset, int bit) {

    uint8_t front, back, x;

    for (int i = offset; i < length; i++) {
        if (bit == 0) {
            x = buffer_src[i];
        } else {
            front = buffer_src[i] << bit;
            if (i != (length - 1)) {
                back = buffer_src[i+1] >> (8 - bit);
            } else {
                back = 0;
            }
            x = front | back;
        }

        buffer_dest[i - offset] = x;
    }

}

int read_area(int physical_sector, int area, uint8_t* data) {

    int min_length = -1;
    int max_length = -1;
    uint8_t sync_byte;

    datapath_sector_timer_set_address_area_enable(false);
    datapath_sector_timer_set_data_area_enable(false);
    if (area == ADDRESS_AREA) {
        datapath_sector_timer_set_address_area_enable(true);
        min_length = dp_controller_info->addr_area_length;
        max_length = min_length + 100;
        sync_byte = dp_controller_info->addr_area_sync_byte;
    }
    if (area == DATA_AREA) {
        datapath_sector_timer_set_data_area_enable(true);
        min_length = dp_controller_info->data_area_length;
        max_length = min_length + 100;
        sync_byte = dp_controller_info->data_area_sync_byte;
    }

    dma_base[DMA_ADDR] = 0xa0004000;
    dma_base[DMA_LEN] = 2048;

    usleep(2000);
    sector_timer_base[6] = physical_sector;

    // Wait to encounter sector
    clock_t start = clock();
    while (((double)(clock() - start) / CLOCKS_PER_SEC) < 0.1) {
        if (!(sector_timer_base[1] & 0x2))
            break;
    }
    if (sector_timer_base[1] & 0x2) {
        datapath_sector_timer_set_enable(false);
        datapath_sector_timer_reset();
        datapath_sector_timer_set_enable(true);
        reset_dma();
        dma_set_enable(true);
        while (dma_base[DMA_STAT] & 0x01) {}
        return -1;
    }

    // Wait for DMA to finish
    start = clock();
    while (((double)(clock() - start) / CLOCKS_PER_SEC) < 0.1) {
        if ((dma_base[DMA_STAT] & 0x2))
            break;
    }

    if (!(dma_base[DMA_STAT] & 0x2)) {
        reset_dma();
        dma_set_enable(true);
        while (dma_base[DMA_STAT] & 0x01) {}
        return -2;
    }

    int length = dma_base[DMA_LEN] & 0x03FFFFFF;

    int offset, bit;
    bool no_data = false;


    if (length < min_length || length > max_length) {
        return -3;
    } else if (findbyte(&bram_base[0], length, sync_byte, &offset, &bit)) {

        // hex_print(&bram_base[0], length);

        if ((offset + (bit ? 1 : 0) + min_length) > length) {
            // printf("Not enough data (offset=%d | bit = %d | length = %d)\n", offset, bit, length);
            return -5;
        }

        copy_buff_start_at(data, &bram_base[0], offset + (bit ? 1 : 0) + min_length, offset, bit);

        return 0;
    } else {
        return -4;
    }
}

int read_track(int num_sectors, int* physical_sectors, struct raw_sector* raw_sectors) {

    int s1, s2;

    for (int i = 0; i < num_sectors; i++) {
        raw_sectors[i].physical_sector = physical_sectors[i];
        s1 = read_area(physical_sectors[i], ADDRESS_AREA, raw_sectors[i].address_area);
        s2 = read_area(physical_sectors[i], DATA_AREA, raw_sectors[i].data_area);
        raw_sectors[i].status = (!s1 && !s2) ? 0 : -1;
    }

    return num_sectors;
}
