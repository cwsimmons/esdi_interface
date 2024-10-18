
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

#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
#include <stdbool.h>

bool array_add_uniquely(int* array, int* array_length, int value);
int array_find_value(int* array, int* array_length, int value);
void array_remove_index(int* array, int* array_length, int index);
void hex_print(uint8_t* buffer, int length);

#endif // UTIL_H