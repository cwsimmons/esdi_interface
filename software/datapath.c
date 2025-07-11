
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

#include "datapath.h"
#include "util.h"

#include <unistd.h>
#include <time.h>
#include <stdio.h>

#define DMA_CTRL     (0x30 >> 2)
#define DMA_STAT     (0x34 >> 2)
#define DMA_CURRDESC (0x38 >> 2)
#define DMA_TAILDESC (0x40 >> 2)
#define DMA_ADDR     (0x48 >> 2)
#define DMA_LEN      (0x58 >> 2)

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
    dma_base[DMA_CTRL] = 0x4;
    clock_t start = clock();
    while(dma_base[DMA_CTRL] & 0x04) {
        if (((double)(clock() - start) / CLOCKS_PER_SEC) > 1) {
            printf("DMA did not complete reset.\n");
            break;
        }
    }
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
    // dma_set_enable(true);
    // while (dma_base[DMA_STAT] & 0x01) {}
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

// TODO: Make this readable
bool read_track_sg(int num_sectors, int* physical_sectors, struct raw_sector* raw_sectors) {

    datapath_sector_timer_set_address_area_enable(true);
    datapath_sector_timer_set_data_area_enable(true);

    datapath_sector_timer_set_enable(false);
    datapath_sector_timer_reset();
    datapath_sector_timer_set_enable(true);

    volatile uint32_t* descriptors = (volatile uint32_t*) bram_base;

    // Set current descriptor to the first
    dma_base[DMA_CURRDESC] = 0xa0040000;

    // Start DMA
    dma_set_enable(true);

    clock_t start = clock();
    while (dma_base[DMA_STAT] & 0x01) {
        if (((double)(clock() - start) / CLOCKS_PER_SEC) > 1) {
            printf("DMA did not leave halted state.\n");
            break;
        }
    }

    // Fill in the descriptor chain
    for (int i = 0; i < (num_sectors * 2); i++) {
        uint32_t next_desc_offset;
        if (i != ((num_sectors * 2) - 1)) {
            next_desc_offset = (i+1) * 0x40;
        }
        else {
            next_desc_offset = 0x00;
        }

        descriptors[((i * 0x40) + 0x00) >> 2] = 0xa0040000 + next_desc_offset;        // Next Descriptor
        descriptors[((i * 0x40) + 0x08) >> 2] = 0xa0040000 + 0x4000 + (i * 1024);     // Buffer Address
        descriptors[((i * 0x40) + 0x18) >> 2] = 1024;                                 // Control (Just Length)
        descriptors[((i * 0x40) + 0x1C) >> 2] = 0;                                    // Status
    }

    // Set the tail descriptor to the last
    dma_base[DMA_TAILDESC] = 0xa0040000 + (((num_sectors * 2) - 1) * 0x40);

    usleep(10000);

    for (int i = 0; i < num_sectors; i++) {
        if (sector_timer_base[1] & 0x01) {
            printf("Sector timer FIFO is full! (i=%d)\n", i);
            break;
        }
        sector_timer_base[6] = i;
    }

    // // Wait for all sectors to be reached
    start = clock();
    // while (((double)(clock() - start) / CLOCKS_PER_SEC) < 0.2) {
    //     if (!(sector_timer_base[1] & 0x2))
    //         break;
    // }
    // // If not every sector was reached, reset the sector timers task queue
    // if (sector_timer_base[1] & 0x2) {
    //     datapath_sector_timer_set_enable(false);
    //     datapath_sector_timer_reset();
    //     datapath_sector_timer_set_enable(true);
    // }

    // Wait for DMA to finish
    // start = clock();
    while (((double)(clock() - start) / CLOCKS_PER_SEC) < 3) {
        if ((dma_base[DMA_STAT] & 0x2)) {
            break;
        }
    }
    if (!(dma_base[DMA_STAT] & 0x2)) {
        printf("DMA Timeout\n");
        printf("sector_timing[1] = 0x%.8x\n", sector_timer_base[1]);
        printf("sector_timing[6] = 0x%.8x\n", sector_timer_base[6]);
    }

    datapath_sector_timer_set_enable(false);
    datapath_sector_timer_reset();
    datapath_sector_timer_set_enable(true);
    dma_set_enable(false);

    if (datapath_base[2] & 0x01) {
        printf("Datapath overflow detected\n");
        datapath_base[2] = 0;
    }

    for (int i = 0; i < num_sectors; i++) {
        raw_sectors[i].status = -1;
        raw_sectors[i].address_read_ok = false;
        raw_sectors[i].data_read_ok = false;
    }

    bool req_reset = false;

    for (int i = 0; i < num_sectors; i++) {
        if (descriptors[((i * (0x40 * 2)) + 0x1C) >> 2] & 0x80000000) {   // Is the address area descriptor complete?
            int offset, bit;
            int length = descriptors[((i * (0x40 * 2)) + 0x1C) >> 2] & 0x03FFFFFF;
            // printf("Sector %d: Address Area Length = %d\n", i, length);
            if (length < dp_controller_info->addr_area_length || length > (dp_controller_info->addr_area_length + 100)) {
                // Wrong length
                raw_sectors[i].status = -2;
                req_reset = true;
                continue;
            }
            if (!findbyte(&bram_base[(0x4000 + ((i*2) * 1024))], length, dp_controller_info->addr_area_sync_byte, &offset, &bit)) {
                // Can't find sync byte
                raw_sectors[i].status = -3;
                continue;
            }
            if ((offset + (bit ? 1 : 0) + dp_controller_info->addr_area_length) > length) {
                // Not enough data after the sync byte
                raw_sectors[i].status = -4;
                continue;
            }

            raw_sectors[i].address_start_location = (dp_controller_info->addr_area_assert / 10) + (offset * 8) + bit;

            copy_buff_start_at(raw_sectors[i].address_area, &bram_base[(0x4000 + ((i*2) * 1024))], offset + (bit ? 1 : 0) + dp_controller_info->addr_area_length, offset, bit);
            raw_sectors[i].address_read_ok = true;
        } else {
            printf("Descriptor not complete for sector %d address area\n", i);
            req_reset = true;
            break;
        }

        if (descriptors[((i * (0x40 * 2)) + 0x40 + 0x1C) >> 2] & 0x80000000) {   // Is the data area descriptor complete?
            int offset, bit;
            int length = descriptors[((i * (0x40 * 2)) + 0x40 + 0x1C) >> 2] & 0x03FFFFFF;
            // printf("Sector %d: Data Area Length = %d\n", i, length);
            if (length < dp_controller_info->data_area_length || length > (dp_controller_info->data_area_length + 100)) {
                // Wrong length
                raw_sectors[i].status = -5;
                req_reset = true;
                continue;
            }
            if (!findbyte(&bram_base[(0x4000 + (((i*2) + 1) * 1024))], length, dp_controller_info->data_area_sync_byte, &offset, &bit)) {
                // Can't find sync byte
                raw_sectors[i].status = -6;
                continue;
            }
            if ((offset + (bit ? 1 : 0) + dp_controller_info->data_area_length) > length) {
                // Not enough data after the sync byte
                raw_sectors[i].status = -7;
                continue;
            }

            raw_sectors[i].data_start_location = (dp_controller_info->data_area_assert / 10) + (offset * 8) + bit;

            copy_buff_start_at(raw_sectors[i].data_area, &bram_base[(0x4000 + (((i*2) + 1) * 1024))], offset + (bit ? 1 : 0) + dp_controller_info->data_area_length, offset, bit);
            raw_sectors[i].data_read_ok = true;
        } else {
            printf("Descriptor not complete for sector %d data area\n", i);
            req_reset = true;
            break;
        }
        raw_sectors[i].status = 0;
    }
    return req_reset;

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

int flush_fifo() {
    volatile uint32_t* descriptors = (volatile uint32_t*) bram_base;

    // Set current descriptor to the first
    dma_base[DMA_CURRDESC] = 0xa0040000;

    // Start DMA
    dma_set_enable(true);
    clock_t start = clock();
    while (dma_base[DMA_STAT] & 0x01) {
        if (((double)(clock() - start) / CLOCKS_PER_SEC) > 1) {
            printf("DMA did not leave halted state.\n");
            break;
        }
    }

    // Fill in the descriptor chain
    for (int i = 0; i < 128; i++) {
        uint32_t next_desc_offset;
        if (i != 127) {
            next_desc_offset = (i+1) * 0x40;
        }
        else {
            next_desc_offset = 0x00;
        }

        descriptors[((i * 0x40) + 0x00) >> 2] = 0xa0040000 + next_desc_offset;        // Next Descriptor
        descriptors[((i * 0x40) + 0x08) >> 2] = 0xa0040000 + 0x4000 + (i * 1024);     // Buffer Address
        descriptors[((i * 0x40) + 0x18) >> 2] = 1024;                                 // Control (Just Length)
        descriptors[((i * 0x40) + 0x1C) >> 2] = 0;                                    // Status
    }

    // Set the tail descriptor to the last
    dma_base[DMA_TAILDESC] = 0xa0040000 + (127 * 0x40);

    usleep(1000);

    // Wait for DMA to finish
    start = clock();
    while (((double)(clock() - start) / CLOCKS_PER_SEC) < 0.02) {
        if ((dma_base[DMA_STAT] & 0x2)) {
            break;
        }
    }

    dma_set_enable(false);

    for (int i = 0; i < 128; i++) {
        if (descriptors[((i * 0x40) + 0x1C) >> 2] & 0x80000000) {   // Is the descriptor complete?
            int length = descriptors[((i * (0x40 * 2)) + 0x1C) >> 2] & 0x03FFFFFF;
            printf("Flushed packet of length %d\n", length);
        } else {
            printf("Flushed %d packets from fifo\n", i);
            return i;
        }

    }
    return 128;
}
