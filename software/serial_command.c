
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

#include "serial_command.h"
#include <stdio.h>
#include <time.h>

volatile uint32_t* serial_command_base = 0;
int serial_retries = 3;

int serial_query(uint16_t command, uint16_t* response) {

    if (serial_command_base == 0) {
        return -1;
    }

    // Wait for module to be ready
    clock_t start = clock();
    while(serial_command_base[0] & 0x01) {
        if (((double)(clock() - start) / CLOCKS_PER_SEC) > 1) {
            printf("Serial interface never became ready.\n");
            return -1;
        }
    }

    uint32_t resp;

    // Read any leftover response that may be present
    if (serial_command_base[0] & 0x02) {
        resp = serial_command_base[1];
    }
    
    // Issue the command
    serial_command_base[1] = ((uint32_t) command) | (1 << 16);

    // Wait for response to arrive and read
    start = clock();
    while(!(serial_command_base[0] & 0x2)) {
        if (((double)(clock() - start) / CLOCKS_PER_SEC) > 1) {
            printf("Device did not respond.\n");
            return -1;
        }
    }
    resp = serial_command_base[1];

    // Check result for error conditions
    if (resp & 0x10000) {
        printf("bad parity\n");
        return -2;  // Bad parity
    }
    if (resp & 0x20000) {
        printf("serial timout\n");
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
    clock_t start = clock();
    while(serial_command_base[0] & 0x01) {
        if (((double)(clock() - start) / CLOCKS_PER_SEC) > 1) {
            printf("Serial interface never became ready.\n");
            return -1;
        }
    }

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

int serial_get_drive_configuration(struct drive_configuration* config) {
    uint16_t query_result;
    if (serial_query_with_retries(CMD_REQCONF | CMD_REQCONF_MOD_GENERAL, &query_result)) return -1;

    config->general_configuration[0] = query_result;

    if (config->general_configuration[0] & 0x0001) {
        uint8_t valid_subscripts[] = {1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
        for (int i = 0; i < 13; i++) {
            if (
                serial_query_with_retries(
                    CMD_REQCONF | CMD_REQCONF_MOD_GENERAL | valid_subscripts[i],
                    &query_result
                )
            ) return -1;

            config->general_configuration[valid_subscripts[i]] = query_result;
        }
    }

    uint8_t valid_specific[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 14, 15};
    for (int i = 0; i < 11; i++) {
        if (
            serial_query_with_retries(
                CMD_REQCONF | (((uint16_t) valid_specific[i]) << 8),
                &query_result
            )
        ) return -1;

        config->specific_configuration[valid_specific[i]-1] = query_result;
    }



    return 0;
}