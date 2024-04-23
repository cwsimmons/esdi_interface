
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
