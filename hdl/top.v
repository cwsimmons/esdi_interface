
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

module top (
    output [3:0] leds,

    output esdi_transfer_req,
    output esdi_command_data,
    input esdi_transfer_ack,
    input esdi_confstat_data,
    input esdi_index,
    input esdi_sector,
    output esdi_read_gate,
    input esdi_read_clock,
    input esdi_read_data,
    input esdi_attention,
    input esdi_ready,
    input esdi_command_complete,
    input esdi_drive_selected,
    output [3:0] esdi_head_select,
    output [2:0] esdi_drive_select,
    output esdi_address_mark_enable
);

    wire esdi_transfer_reqn;
    wire esdi_command_datan;

    assign esdi_transfer_req = !esdi_transfer_reqn;
    assign esdi_command_data = !esdi_command_datan;

    soc_bd soc_bd_0 (
        .esdi_transfer_req      (esdi_transfer_reqn),
        .esdi_command_data      (esdi_command_datan),
        .esdi_transfer_ack      (esdi_transfer_ack),
        .esdi_confstat_data     (esdi_confstat_data),
        .esdi_command_complete  (esdi_command_complete),
        .esdi_attention         (esdi_attention),
        .esdi_index             (esdi_index),
        .esdi_read_gate         (esdi_read_gate),
        .esdi_sector            (esdi_sector),
        .esdi_read_clock        (esdi_read_clock),
        .esdi_read_data         (esdi_read_data),
        .esdi_drive_select      (esdi_drive_select),
        .esdi_head_select       (esdi_head_select),
        .esdi_ready             (esdi_ready),
        .esdi_drive_selected    (esdi_drive_selected),
        .esdi_address_mark_enable (esdi_address_mark_enable)

    );

endmodule