
#include "serial_command.h"

volatile uint32_t* serial_command_base = 0;
int serial_retries = 3;

int serial_query(uint16_t command, uint16_t* response) {

    if (serial_command_base == 0) {
        return -1;
    }

    // Wait for module to be ready
    while(serial_command_base[0] & 0x01) {}

    uint32_t resp;

    // Read any leftover response that may be present
    if (serial_command_base[0] & 0x02) {
        resp = serial_command_base[1];
    }
    
    // Issue the command
    serial_command_base[1] = ((uint32_t) command) | (1 << 16);

    // Wait for response to arrive and read
    while(!(serial_command_base[0] & 0x2)) {}
    resp = serial_command_base[1];

    // Check result for error conditions
    if (resp & 0x10000) {
        return -2;  // Bad parity
    }
    if (resp & 0x20000) {
        return -3;  // Bit timeout
    }

    *response = resp & 0xFFFF;
    return 0;
}

int serial_command(uint16_t command) {

    if (serial_command_base == 0) {
        return -1;
    }

    // Wait for module to be ready
    while(serial_command_base[0] & 0x01) {}

    // Issue the command
    serial_command_base[1] = (uint32_t) command;

    // the verilog should probably be reworked to signal errors
    // on commands, not just queries
    return 0;
}

int serial_query_with_retries(uint16_t command, uint16_t* response) {

    int ret = -1;
    for (int i = 0; i < serial_retries; i++) {
        ret = serial_query(command, response);
        if (!ret) return 0;
    }

    return ret;
}

int serial_query_drive_parameters(struct drive_parameters* parameters) {

    uint16_t general_config;
    uint16_t heads_response;
    uint16_t cyl_response;
    uint16_t hard_sect_response;

    if (serial_query_with_retries(CMD_REQCONF | CMD_REQCONF_MOD_GENERAL, &general_config)) return -1;
    parameters->is_soft_sectored = (general_config & 0x0004) >> 2;
    if (serial_query_with_retries(CMD_REQCONF | CMD_REQCONF_MOD_NUMHEADS, &heads_response)) return -1;
    parameters->heads = heads_response & 0x00FF;
    if (serial_query_with_retries(CMD_REQCONF | CMD_REQCONF_MOD_NUMCYLFIXED, &cyl_response)) return -1;
    parameters->cylinders = cyl_response;
    if (serial_query_with_retries(CMD_REQCONF | CMD_REQCONF_MOD_SECPERTRK, &hard_sect_response)) return -1;
    parameters->sectors_hard = hard_sect_response & 0x00FF;

    return 0;
}
