#ifndef DRIVE_H
#define DRIVE_H

#include <stdint.h>

void set_drive_select(uint8_t ds);
void set_head_select(uint8_t hs);

void drive_reset();

void drive_seek(int cyl);

#endif // DRIVE_H