
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

module top (

    inout [14:0] DDR_addr,
    inout [2:0] DDR_ba,
    inout DDR_cas_n,
    inout DDR_ck_n,
    inout DDR_ck_p,
    inout DDR_cke,
    inout DDR_cs_n,
    inout [3:0] DDR_dm,
    inout [31:0] DDR_dq,
    inout [3:0] DDR_dqs_n,
    inout [3:0] DDR_dqs_p,
    inout DDR_odt,
    inout DDR_ras_n,
    inout DDR_reset_n,
    inout DDR_we_n,
    inout FIXED_IO_ddr_vrn,
    inout FIXED_IO_ddr_vrp,
    inout [53:0] FIXED_IO_mio,
    inout FIXED_IO_ps_clk,
    inout FIXED_IO_ps_porb,
    inout FIXED_IO_ps_srstb,

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

        .DDR_addr(DDR_addr),
        .DDR_ba(DDR_ba),
        .DDR_cas_n(DDR_cas_n),
        .DDR_ck_n(DDR_ck_n),
        .DDR_ck_p(DDR_ck_p),
        .DDR_cke(DDR_cke),
        .DDR_cs_n(DDR_cs_n),
        .DDR_dm(DDR_dm),
        .DDR_dq(DDR_dq),
        .DDR_dqs_n(DDR_dqs_n),
        .DDR_dqs_p(DDR_dqs_p),
        .DDR_odt(DDR_odt),
        .DDR_ras_n(DDR_ras_n),
        .DDR_reset_n(DDR_reset_n),
        .DDR_we_n(DDR_we_n),
        .FIXED_IO_ddr_vrn(FIXED_IO_ddr_vrn),
        .FIXED_IO_ddr_vrp(FIXED_IO_ddr_vrp),
        .FIXED_IO_mio(FIXED_IO_mio),
        .FIXED_IO_ps_clk(FIXED_IO_ps_clk),
        .FIXED_IO_ps_porb(FIXED_IO_ps_porb),
        .FIXED_IO_ps_srstb(FIXED_IO_ps_srstb),

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