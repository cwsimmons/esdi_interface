
# Copyright 2024 Christopher Simmons

# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the Free
# Software Foundation, either version 2 of the License, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
# more details.

# You should have received a copy of the GNU General Public License along
# with this program. If not, see <https://www.gnu.org/licenses/>.


set_property PACKAGE_PIN D5       [get_ports "leds[0]"];
set_property IOSTANDARD LVCMOS33  [get_ports "leds[0]"];
set_property PACKAGE_PIN D6       [get_ports "leds[1]"];
set_property IOSTANDARD LVCMOS33  [get_ports "leds[1]"];
set_property PACKAGE_PIN A5       [get_ports "leds[2]"];
set_property IOSTANDARD LVCMOS33  [get_ports "leds[2]"];
set_property PACKAGE_PIN B5       [get_ports "leds[3]"];
set_property IOSTANDARD LVCMOS33  [get_ports "leds[3]"];

## FMC Row H
set_property PACKAGE_PIN J16      [get_ports "esdi_head_select[2]"]; # FMC_LPC_LA07_P
set_property IOSTANDARD  LVCMOS12 [get_ports "esdi_head_select[2]"]; # FMC_LPC_LA07_P
set_property PACKAGE_PIN J15      [get_ports "esdi_head_select[0]"]; # FMC_LPC_LA07_N
set_property IOSTANDARD  LVCMOS12 [get_ports "esdi_head_select[0]"]; # FMC_LPC_LA07_N
set_property PACKAGE_PIN A13      [get_ports "esdi_transfer_req"]; # FMC_LPC_LA11_P
set_property IOSTANDARD  LVCMOS12 [get_ports "esdi_transfer_req"]; # FMC_LPC_LA11_P
set_property PACKAGE_PIN A12      [get_ports "esdi_drive_select[1]"]; # FMC_LPC_LA11_N
set_property IOSTANDARD  LVCMOS12 [get_ports "esdi_drive_select[1]"]; # FMC_LPC_LA11_N
set_property PACKAGE_PIN D16      [get_ports "esdi_read_gate"]; # FMC_LPC_LA15_P
set_property IOSTANDARD  LVCMOS12 [get_ports "esdi_read_gate"]; # FMC_LPC_LA15_P
set_property PACKAGE_PIN C16      [get_ports "esdi_address_mark_enable"]; # FMC_LPC_LA15_N
set_property IOSTANDARD  LVCMOS12 [get_ports "esdi_address_mark_enable"]; # FMC_LPC_LA15_N
# set_property PACKAGE_PIN D12      [get_ports ""]; # FMC_LPC_LA19_P   Write Clock
# set_property IOSTANDARD  LVCMOS12 [get_ports ""]; # FMC_LPC_LA19_P


# set_property PACKAGE_PIN B10      [get_ports ""]; # FMC_LPC_LA21_P   Write Data
# set_property IOSTANDARD  LVCMOS12 [get_ports ""]; # FMC_LPC_LA21_P


set_property PACKAGE_PIN B6       [get_ports "esdi_transfer_ack"]; # FMC_LPC_LA24_P
set_property IOSTANDARD  LVCMOS12 [get_ports "esdi_transfer_ack"]; # FMC_LPC_LA24_P
set_property PACKAGE_PIN A6       [get_ports "esdi_sector"]; # FMC_LPC_LA24_N
set_property IOSTANDARD  LVCMOS12 [get_ports "esdi_sector"]; # FMC_LPC_LA24_N
set_property PACKAGE_PIN M13      [get_ports "esdi_ready"]; # FMC_LPC_LA28_P
set_property IOSTANDARD  LVCMOS12 [get_ports "esdi_ready"]; # FMC_LPC_LA28_P
set_property PACKAGE_PIN L13      [get_ports "esdi_command_complete"]; # FMC_LPC_LA28_N
set_property IOSTANDARD  LVCMOS12 [get_ports "esdi_command_complete"]; # FMC_LPC_LA28_N
set_property PACKAGE_PIN E9       [get_ports "esdi_read_clock"]; # FMC_LPC_LA30_P
set_property IOSTANDARD  LVCMOS12 [get_ports "esdi_read_clock"]; # FMC_LPC_LA30_P


set_property PACKAGE_PIN F8       [get_ports "esdi_read_data"]; # FMC_LPC_LA32_P
set_property IOSTANDARD  LVCMOS12 [get_ports "esdi_read_data"]; # FMC_LPC_LA32_P


## FMC Row G
set_property PACKAGE_PIN E18      [get_ports "esdi_head_select[3]"]; # FMC_LPC_LA08_P
set_property IOSTANDARD  LVCMOS12 [get_ports "esdi_head_select[3]"]; # FMC_LPC_LA08_P
# set_property PACKAGE_PIN E17      [get_ports ""]; # FMC_LPC_LA08_N   Write Gate
# set_property IOSTANDARD  LVCMOS12 [get_ports ""]; # FMC_LPC_LA08_N
set_property PACKAGE_PIN G18      [get_ports "esdi_head_select[1]"]; # FMC_LPC_LA12_P
set_property IOSTANDARD  LVCMOS12 [get_ports "esdi_head_select[1]"]; # FMC_LPC_LA12_P
set_property PACKAGE_PIN F18      [get_ports "esdi_drive_select[0]"]; # FMC_LPC_LA12_N
set_property IOSTANDARD  LVCMOS12 [get_ports "esdi_drive_select[0]"]; # FMC_LPC_LA12_N
set_property PACKAGE_PIN D17      [get_ports "esdi_drive_select[2]"]; # FMC_LPC_LA16_P
set_property IOSTANDARD  LVCMOS12 [get_ports "esdi_drive_select[2]"]; # FMC_LPC_LA16_P
set_property PACKAGE_PIN C17      [get_ports "esdi_command_data"]; # FMC_LPC_LA16_N
set_property IOSTANDARD  LVCMOS12 [get_ports "esdi_command_data"]; # FMC_LPC_LA16_N
set_property PACKAGE_PIN C7       [get_ports "esdi_confstat_data"]; # FMC_LPC_LA25_P
set_property IOSTANDARD  LVCMOS12 [get_ports "esdi_confstat_data"]; # FMC_LPC_LA25_P
set_property PACKAGE_PIN C6       [get_ports "esdi_attention"]; # FMC_LPC_LA25_N
set_property IOSTANDARD  LVCMOS12 [get_ports "esdi_attention"]; # FMC_LPC_LA25_N
set_property PACKAGE_PIN K10      [get_ports "esdi_index"]; # FMC_LPC_LA29_P
set_property IOSTANDARD  LVCMOS12 [get_ports "esdi_index"]; # FMC_LPC_LA29_P
set_property PACKAGE_PIN J10      [get_ports "esdi_drive_selected"]; # FMC_LPC_LA29_N
set_property IOSTANDARD  LVCMOS12 [get_ports "esdi_drive_selected"]; # FMC_LPC_LA29_N