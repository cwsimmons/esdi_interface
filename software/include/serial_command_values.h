
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

#define CMD_SEEK    0x0000
#define CMD_RECAL   0x1000
#define CMD_REQSTAT 0x2000
#define CMD_REQCONF 0x3000
#define CMD_SELHEAD 0x4000
#define CMD_CONTROL 0x5000

#define CMD_REQCONF_MOD_GENERAL         0x000
#define CMD_REQCONF_MOD_NUMCYLFIXED     0x100
#define CMD_REQCONF_MOD_NUMCYLREMOV     0x200
#define CMD_REQCONF_MOD_NUMHEADS        0x300
#define CMD_REQCONF_MOD_UFMTBYTEPERTRK  0x400
#define CMD_REQCONF_MOD_UFMTBYTEPERSEC  0x500
#define CMD_REQCONF_MOD_SECPERTRK       0x600
#define CMD_REQCONF_MOD_MINBYTESISG     0x700
#define CMD_REQCONF_MOD_MINBYTESPLO     0x800
#define CMD_REQCONF_MOD_NUMSTATWORDS    0x900

#define CMD_CONTROL_MOD_RESET           0x000
#define CMD_CONTROL_MOD_STOPMOTOR       0x200
#define CMD_CONTROL_MOD_STARTMOTOR      0x300