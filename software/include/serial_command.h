#ifndef SERIAL_COMMAND_H
#define SERIAL_COMMAND_H

#include <stdint.h>
#include "types.h"
#include "serial_command_values.h"

extern volatile uint32_t* serial_command_base;
extern int serial_retries;

int serial_query(uint16_t command, uint16_t* response);
int serial_command(uint16_t command);
int serial_query_with_retries(uint16_t command, uint16_t* response);

int serial_query_drive_parameters(struct drive_parameters* parameters);

#endif // SERIAL_COMMAND_H