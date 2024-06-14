
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <time.h>

#define DMA_CTRL (0x30 >> 2)
#define DMA_STAT (0x34 >> 2)
#define DMA_ADDR (0x48 >> 2)
#define DMA_LEN  (0x58 >> 2)

volatile uint32_t* mem;
volatile uint32_t* timer;
volatile uint32_t* rxdatapath;
volatile uint8_t* bram;
volatile uint32_t* dma;


uint32_t query(uint32_t cmd) {

    if (!(cmd & 0x10000)) {
        printf("Not a query!");
        return 0;
    }

    while(mem[0] & 0x01) {}

    uint32_t response;
    if (mem[0] & 0x02) {
        response = mem[1];
    }
    

    mem[1] = cmd;

    while(!(mem[0] & 0x2)) {}
    response = mem[1];
    if (response & 0x10000) {
        printf("Bad parity!\n");
    }
    if (response & 0x20000) {
        printf("Timeout during serial query\n");
    }
    return response;
}

void command(uint32_t cmd) {
    while(mem[0] & 0x01) {}
    mem[1] = cmd;
}

void reset_dma() {
    dma[DMA_CTRL] = 0x2;
    // while(dma[DMA_CTRL] & 0x02) {}
    dma[DMA_CTRL] &= ~0x2;
}

void dma_set_enable(bool enable) {
    uint32_t tmp = dma[DMA_CTRL];
    tmp &= ~0x01;
    if (enable)
        tmp |= 0x01;
    dma[DMA_CTRL] = tmp;
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

void print_buff_start_at(volatile uint8_t* buffer, int length, int offset, int bit) {

    uint8_t front, back;
    int count = 0;
    char x;
    char ascii[17];
    memset(ascii, ' ', 16);
    ascii[16] = 0;

    for (int i = offset; i < length; i++) {
        if (bit == 0) {
            x = buffer[i];
        } else {
            front = buffer[i] << bit;
            if (i != (length - 1)) {
                back = buffer[i+1] >> (8 - bit);
            } else {
                back = 0;
            }
            x = front | back;
        }
        printf("%.2x ", x);
        if (x >= 0x20 && x < 0x7F) {
            ascii[count % 16] = x;
        }
        count++;
        if (count % 16 == 0) {
            printf("    %s\n", ascii);
            memset(ascii, ' ', 16);
        }
    }
    printf("\n");

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


void timer_set_enable(bool enable) {
    uint32_t tmp = timer[0];
    tmp &= ~0x01;
    if (enable)
        tmp |= 0x01;
    timer[0] = tmp;
}

void timer_reset() {
    uint32_t tmp = timer[0];
    tmp |= 0x02;
    timer[0] = tmp;
    timer[0] = tmp & ~0x02;
}

void set_drive_select(uint8_t ds) {
    mem[2] = (uint32_t) ds & 0xF;
}

void set_head_select(uint8_t hs) {
    mem[3] = (uint32_t) hs & 0xF;
}



int main()
{

    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd == -1) {
        printf("Failed to open /dev/mem\n");
        return 1;
    }

    mem = (uint32_t*) mmap(NULL, 0x20000, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0xa0000000);
    if (mem == MAP_FAILED) {
        printf("Failed to mmap\n");
        return 1;
    }

    FILE* img_file = fopen("image.rei", "wb");

    if (img_file == NULL) {
        printf("Failed to open image output file\n");
        exit(1);
    }

    uint8_t buffer[2048];
    uint8_t pad_bytes[16];
    memset(pad_bytes, 0x00, 16);

    timer = &mem[(0x1000 >> 2)];
    rxdatapath = &mem[0x2000 >> 2];
    bram = (uint8_t*) &mem[0x4000 >> 2];
    dma = &mem[0x10000 >> 2];

    set_drive_select(2);

    uint32_t resp = query(0x12000);
    printf("General Status = 0x%x\n", resp);
    command(0x5000);
    resp = query(0x12000);
    printf("General Status after reset = 0x%x\n", resp);
    resp = query(0x13000);
    printf("General Config = 0x%x\n", resp);
    uint32_t num_cylinders = query(0x13100); // Number cylin
    printf("Num Fixed Cylinders = %d\n", num_cylinders);
    uint32_t num_heads = query(0x13300) & 0xFF;
    printf("Num Heads = %d\n", num_heads);


    resp = query(0x13400);
    printf("Minimum unformatted bytes per track = %d\n", resp);
    resp = query(0x13500);
    printf("Minimum unformatted bytes per sector = %d\n", resp);
    resp = query(0x13600);
    printf("Number of sectors per track = %d\n", resp);
    resp = query(0x13700);
    printf("Minimum bytes in ISG field = %d, %d\n", resp >> 8, resp & 0xFF);
    resp = query(0x13800);
    printf("Minimum bytes per PLO sync field = %d\n", resp);
    resp = query(0x13900);
    printf("Number of status words available = %d, %d\n", resp >> 8, resp & 0xFF);


    resp = query(0x13F00);
    printf("Vendor ID = %d\n\n\n", resp);

    

    // timer[0] = 0b01000;
    timer[0] = 0b101000;

    // PC measured
    // timer[2] = 408;
    // timer[3] = 408 + 1872;
    // timer[4] = 2540;
    // timer[5] = 2540 + 42636;

    // RT measured
    // timer[2] = 242;
    // timer[3] = 242 + 1940;
    // timer[4] = 2270;
    // timer[5] = 2270 + 42636 + 30;

    // S36 guessed
    // timer[2] = 242;
    // timer[3] = 242 + 1940;
    // timer[4] = 2270;
    // timer[5] = 2270 + 21318 + 600;

    // Soft Sector 1558 Guess
    timer[2] = 0;
    timer[3] = 1872;
    timer[4] = 2300;
    timer[5] = 2300 + 42836;

    // for (int i = 0; i < num_cylinders; i++) {
    // 	command(i & 0x0FFF);
    // 	sleep(1);
    // }

    int length;
    // printf("DMA status = %.8x\n", dma[DMA_STAT]);
    // printf("Resetting DMA\n");
    reset_dma();
    // printf("DMA status = %.8x\n", dma[DMA_STAT]);
    dma_set_enable(true);
    // printf("DMA status = %.8x\n", dma[DMA_STAT]);
    // printf("Wait for DMA to indicate that it is running\n");
    while (dma[DMA_STAT] & 0x01) {}

    // printf("Enabling datapath\n");
    rxdatapath[0] = 0x3;

    // printf("Internal sampling clocks per bit = %d\n", rxdatapath[1]);
    timer_reset();
    timer_set_enable(true);

    // printf("DMA status = %.8x\n", dma[DMA_STAT]);
    
    for (int cyl = 0; cyl < 2; cyl++)
    {
        command(cyl & 0xFFF);
        usleep(100000);

        for (int head = 0; head < 5; head++) {
            
            set_head_select(head);

            for (int sector = 0; sector < 36; sector++) {

    //             printf(
    // "#####################################################\n\
    // Reading head %d sector %d\n\
    // #####################################################\n",
    //                 head,
    //                 sector
    //             );
                printf("Cyl = %.3d Head = %.2d Sec = %.2d\n", cyl, head, sector);
                // command(cyl & 0x0FFF);
                // sleep(1);
                // timer_set_enable(true);
                dma[DMA_ADDR] = 0xa0004000;
                dma[DMA_LEN] = 2048;

                //printf("Enabling datapath\n");
                // rxdatapath[0] = 0x3;
                usleep(1000);
                timer[6] = sector;
                // printf("timer[1] = 0x%x\n", timer[1]);
                // printf("num tasks = %d\n", timer[6]);

                // printf("Wait for timer module to be done\n");
                clock_t start = clock();
                while (((double)(clock() - start) / CLOCKS_PER_SEC) < 0.1) {
                    if (!(timer[1] & 0x2))
                        break;
                }
                if (timer[1] & 0x2) {
                    printf("Sector not reached!!!\n");
                    timer_set_enable(false);
                    timer_reset();
                    timer_set_enable(true);
                    continue;
                }




                // printf("waiting for DMA to finish\n");
                start = clock();
                while (((double)(clock() - start) / CLOCKS_PER_SEC) < 0.1) {
                    if ((dma[DMA_STAT] & 0x2))
                        break;
                }

                if (!(dma[DMA_STAT] & 0x2)) {
                    printf("DMA Timed out!!!\n");
                    reset_dma();
                    // printf("DMA status = %.8x\n", dma[DMA_STAT]);
                    dma_set_enable(true);
                    // printf("DMA status = %.8x\n", dma[DMA_STAT]);
                    // printf("Wait for DMA to indicate that it is running\n");
                    while (dma[DMA_STAT] & 0x01) {}
                    continue;
                }

                length = dma[DMA_LEN] & 0x03FFFFFF;



                // for (int i = 1; i < 2; i++) {
                //     dma[DMA_ADDR] = 0xa0004000 + (0x500 * i);
                //     dma[DMA_LEN] = 2048;
                //     while(!(dma[DMA_STAT] & 0x2)) {}
                //     lengths[i] = dma[DMA_LEN] & 0x03FFFFFF;
                // }

                //printf("Disable hw\n");
                // rxdatapath[0] = 0x2;
                // timer_set_enable(false);

                // printf("Print results\n");
                printf("    Raw Length = %d\n", length);
                // for (int i = 0; i < length; i++) {
                // 	printf("%.2x ", bram[i]);
                // }
                // printf("\n");
                // Rt controller uses 0xFE as sync byte, Pc uses 0xA1
                int offset, bit;
                bool no_data = false;
                // printf("[%.4d,%d,%.2d] = ", cyl, head, sector);
                if (length < 517 || length > 550) {
                    no_data = true;
                    printf("Unexpected data length!\n");
                } else if (findbyte(&bram[0], length, 0xfe, &offset, &bit)) {
                    printf("    Fount sync byte at offset %d bit %d\n", offset, bit);
                    // print_buff_start_at(&bram[0], length, offset, bit);
                    copy_buff_start_at(buffer, &bram[0], length, offset, bit);
                    fwrite(buffer, 1, 512 + 5, img_file);

                } else {
                    no_data = true;
                    printf("Sync byte not found!\n");
                }

                if (no_data) {
                    memset(buffer, 0xff, 517);
                    fwrite(buffer, 1, 517, img_file);
                }

                fwrite(pad_bytes, 1, 11, img_file);
                // usleep(10000);

                // if (findbyte(&bram[0], length, 0xfe, &offset, &bit)) {
                //     print_buff_start_at(&bram[0], length, offset, bit);
                // } else {
                //     printf("Sync byte not found!\n");
                // }

            }
        }

    }


    // printf("Disable hw\n");
    rxdatapath[0] = 0x2;
    timer_set_enable(false);
    

    close(fd);
}
