
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

#include "util.h"

#include <stdio.h>

bool array_add_uniquely(int* array, int* array_length, int value) {
    bool new_value = true;
    for (int i = 0; i < (*array_length); i++) {
        if (array[i] == value) {
            new_value = false;
            break;
        }
    }

    if (new_value) {
        array[(*array_length)++] = value;
    }

    return new_value;
}

int array_find_value(int* array, int* array_length, int value) {
    for (int i = 0; i < *array_length; i++) {
        if (array[i] == value)
            return i;
    }
    return -1;
}

void array_remove_index(int* array, int* array_length, int index) {
    if ((index >= *array_length) || (*array_length < 0))
        return;
    
    for (int i = index; i < (*array_length - 1); i++) {
        array[i] = array[i+1];
    }

    *array_length--;
}

void hex_print(uint8_t* buffer, int length) {
    for (int i = 0; i < length; i++) {
        printf("%.2x ", buffer[i]);
        if ((i % 16) == 15) {
            printf("\n");
        }
    }
    if ((length % 16)) {
        printf("\n");
    }
}

void copy_buff_to_offset(uint8_t* buffer_dest, volatile uint8_t* buffer_src, int length, int offset, int bit) {

    uint8_t front, back, x;
    // printf("length = %d    offset = %d    bit = %d\n", length, offset, bit);


    for (int i = offset; i < (offset + length + ((bit == 0) ? 0 : 1)); i++) {
        if (bit == 0) {
            x = buffer_src[i - offset];
        } else {
            if (i != offset) {
                front = buffer_src[i - offset - 1] << (8 - bit);
            } else {
                front = 0;
            }
            
            if (i < (offset + length)) {
                back = buffer_src[i - offset] >> bit;
            } else {
                back = 0;
            }

            x = front | back;
        }

        // printf("i = %d    front = 0x%.2x   back = 0x%.2x    x = 0x%.2x\n", i, front, back, x);
        buffer_dest[i] = x;
    }

}
