
/* 

    Copyright 2025 Christopher Simmons

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

#ifndef ADDRESSES_H
#define ADDRESSES_H

#ifdef ZYNQ7

#define PL_BRIDGE_ADDR 0x40000000
#define PL_BRIDGE_LENGTH 0x2040000

#define SIZE_BRAM  (256*1024)
#define ADDR_BRAM  0x42000000
#define ADDR_DMA   0x40400000
#define ADDR_CMD   0x40000000
#define ADDR_READ  0x40001000
#define ADDR_TIMER 0x40002000

#endif

#ifdef  MPSOC

#define PL_BRIDGE_ADDR 0xA0000000
#define PL_BRIDGE_LENGTH 0x80000

#define SIZE_BRAM  (256*1024)
#define ADDR_BRAM  0xA0040000
#define ADDR_DMA   0xA0010000
#define ADDR_CMD   0xA0000000
#define ADDR_READ  0xA0002000
#define ADDR_TIMER 0xA0001000

#endif

#endif //ADDRESSES_H