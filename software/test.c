#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>

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
    return response;
}

void command(uint32_t cmd) {
    while(mem[0] & 0x01) {}
    mem[1] = cmd;
}

void reset_dma() {
    dma[DMA_CTRL] = 0x2;
    // while(dma[DMA_CTRL] & 0x02) {}
}

void dma_set_enable(bool enable) {
    uint32_t tmp = dma[DMA_CTRL];
    tmp &= ~0x01;
    if (enable)
        tmp |= 0x01;
    dma[DMA_CTRL] = tmp;
}

bool findbyte(uint8_t* buffer, int length, uint8_t pattern, int* offset, int* bit) {

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

void print_buff_start_at(uint8_t* buffer, int length, int offset, int bit) {

    uint8_t front, back;

    for (int i = offset; i < length; i++) {
        if (bit == 0) {
            printf("%.2x ", buffer[i]);
        } else {
            front = buffer[i] << bit;
            if (i != (length - 1)) {
                back = buffer[i+1] >> (8 - bit);
            } else {
                back = 0;
            }
            printf("%.2x ", front | back);
        }

    }
    printf("\n");

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


    timer = &mem[(0x1000 >> 2)];
    rxdatapath = &mem[0x2000 >> 2];
    bram = (uint8_t*) &mem[0x4000 >> 2];
    dma = &mem[0x10000 >> 2];

    // uint32_t resp = query(0x12000);
    // printf("General Status = 0x%x\n", resp);
    command(0x5000);
    // resp = query(0x12000);
    // printf("General Status after reset = 0x%x\n", resp);
    // resp = query(0x13000);
    // printf("General Config = 0x%x\n", resp);
    // uint32_t num_cylinders = query(0x13100); // Number cylin
    // printf("Num Fixed Cylinders = %d\n", num_cylinders);
    // uint32_t num_heads = query(0x13300) & 0xFF;
    // printf("Num Heads = %d\n", num_heads);


    // resp = query(0x13400);
    // printf("Minimum unformatted bytes per track = %d\n", resp);
    // resp = query(0x13500);
    // printf("Minimum unformatted bytes per sector = %d\n", resp);
    // resp = query(0x13600);
    // printf("Number of sectors per track = %d\n", resp);
    // resp = query(0x13700);
    // printf("Minimum bytes in ISG field = %d, %d\n", resp >> 8, resp & 0xFF);
    // resp = query(0x13800);
    // printf("Minimum bytes per PLO sync field = %d\n", resp);
    // resp = query(0x13900);
    // printf("Number of status words available = %d, %d\n", resp >> 8, resp & 0xFF);


    // resp = query(0x13F00);
    // printf("Vendor ID = %d\n", resp);

    // command(0x0);

    timer[1] = 408;
    timer[2] = 408 + 1872;
    timer[0] = 0x01;

    // for (int i = 0; i < num_cylinders; i++) {
    // 	command(i & 0x0FFF);
    // 	sleep(1);
    // }

    int lengths[40];

    printf("Resetting DMA\n");
    reset_dma();
    dma_set_enable(true);

    dma[DMA_ADDR] = 0xa0004000;
    dma[DMA_LEN] = 2048;

    printf("Enabling datapath\n");
    rxdatapath[0] = 0x3;

    // printf("waiting for DMA to finish\n");
    while(!(dma[DMA_STAT] & 0x2)) {}
    lengths[0] = dma[DMA_LEN] & 0x03FFFFFF;

    for (int i = 1; i < 40; i++) {
        dma[DMA_ADDR] = 0xa0004000 + (0x80 * i);
        dma[DMA_LEN] = 2048;
        while(!(dma[DMA_STAT] & 0x2)) {}
        lengths[i] = dma[DMA_LEN] & 0x03FFFFFF;
    }

    printf("Disable hw\n");
    rxdatapath[0] = 0x2;
    timer[0] = 0x00;

    printf("Print results\n");
    for (int j = 0; j < 40; j++) {

        // for (int i = 0; i < lengths[j]; i++) {
        // 	printf("%.2x ", bram[(j*0x80) + i]);
        // }
        int offset, bit;
        if (findbyte(&bram[j*0x80], lengths[j], 0xa1, &offset, &bit)) {
            // printf("\nFount A1 at offset %d bit %d", offset, bit);
            print_buff_start_at(&bram[j*0x80], lengths[j], offset, bit);
        }

    }

    close(fd);
}
