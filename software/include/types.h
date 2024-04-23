
#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include <stdint.h>

struct chs {
    int c,h,s;
};

struct drive_parameters {

    int heads;
    int cylinders;
    int sectors;

    bool is_soft_sectored;
    int sectors_hard;

};

struct raw_sector {
    // Address
    int cylinder;
    int head;
    int physical_sector;

    // Info
    int status;
    uint8_t* address_area;
    uint8_t* data_area;
};

struct processed_sector {
    int lba;
    
    int length;
    uint8_t* data;
};

#endif // TYPES_H