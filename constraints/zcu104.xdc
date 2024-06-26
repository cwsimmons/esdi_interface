
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

set_property PACKAGE_PIN E12      [get_ports "esdi_transfer_req"];
set_property IOSTANDARD LVCMOS18  [get_ports "esdi_transfer_req"];
set_property PACKAGE_PIN E17      [get_ports "esdi_command_data"];
set_property IOSTANDARD LVCMOS18  [get_ports "esdi_command_data"];
set_property PACKAGE_PIN D16      [get_ports "esdi_transfer_ack"];
set_property IOSTANDARD LVCMOS18  [get_ports "esdi_transfer_ack"];
set_property PACKAGE_PIN D12      [get_ports "esdi_confstat_data"];
set_property IOSTANDARD LVCMOS18  [get_ports "esdi_confstat_data"];
set_property PACKAGE_PIN B6       [get_ports "esdi_read_clock"];
set_property IOSTANDARD LVCMOS18  [get_ports "esdi_read_clock"];
set_property PACKAGE_PIN L17      [get_ports "esdi_index"];
set_property IOSTANDARD LVCMOS18  [get_ports "esdi_index"];
set_property PACKAGE_PIN J16      [get_ports "esdi_sector"];
set_property IOSTANDARD LVCMOS18  [get_ports "esdi_sector"];
set_property PACKAGE_PIN F18      [get_ports "esdi_read_gate"];
set_property IOSTANDARD LVCMOS18  [get_ports "esdi_read_gate"];
set_property PACKAGE_PIN B10      [get_ports "esdi_read_data"];
set_property IOSTANDARD  LVCMOS18 [get_ports "esdi_read_data"];
set_property PACKAGE_PIN A13      [get_ports "esdi_attention"];
set_property IOSTANDARD  LVCMOS18 [get_ports "esdi_attention"];
set_property PACKAGE_PIN L20      [get_ports "esdi_ready"];
set_property IOSTANDARD  LVCMOS18 [get_ports "esdi_ready"];
set_property PACKAGE_PIN K18      [get_ports "esdi_head_select[0]"];
set_property IOSTANDARD  LVCMOS18 [get_ports "esdi_head_select[0]"];
set_property PACKAGE_PIN F16      [get_ports "esdi_head_select[1]"];
set_property IOSTANDARD  LVCMOS18 [get_ports "esdi_head_select[1]"];
set_property PACKAGE_PIN H12      [get_ports "esdi_head_select[2]"];
set_property IOSTANDARD  LVCMOS18 [get_ports "esdi_head_select[2]"];
set_property PACKAGE_PIN C6       [get_ports "esdi_head_select[3]"];
set_property IOSTANDARD  LVCMOS18 [get_ports "esdi_head_select[3]"];
set_property PACKAGE_PIN C17      [get_ports "esdi_drive_select"];
set_property IOSTANDARD  LVCMOS18 [get_ports "esdi_drive_select"];

set_property PACKAGE_PIN G8       [get_ports "esdi_address_mark_enable"];            # PMOD0_0
set_property IOSTANDARD LVCMOS33  [get_ports "esdi_address_mark_enable"];            # PMOD0_0