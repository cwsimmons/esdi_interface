
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

`timescale 1ns / 1ps

module axi_esdi_cmd_controller_tb ();

    reg csr_aclk;
    reg csr_aresetn;
    reg csr_awvalid;
    reg [4:0] csr_awaddr;
    reg csr_wvalid;
    reg [31:0] csr_wdata;
    reg csr_arvalid;
    reg [4:0] csr_araddr;

    wire esdi_transfer_req;
    wire esdi_transfer_ack;

    assign esdi_transfer_ack = esdi_transfer_req;

    axi_esdi_cmd_controller #(6, 6, 40) uut0 (
        .csr_aclk               (csr_aclk),
        .csr_aresetn            (csr_aresetn),
        .csr_awvalid            (csr_awvalid),
        .csr_awready            (),
        .csr_awaddr             (csr_awaddr),
        .csr_awprot             (3'b000),
        .csr_wvalid             (csr_wvalid),
        .csr_wready             (),
        .csr_wdata              (csr_wdata),
        .csr_wstrb              (4'b1111),
        .csr_bvalid             (),
        .csr_bready             (1'b1),
        .csr_bresp              (),
        .csr_arvalid            (csr_arvalid),
        .csr_arready            (),
        .csr_araddr             (csr_araddr),
        .csr_arprot             (3'b000),
        .csr_rvalid             (),
        .csr_rready             (1'b1),
        .csr_rdata              (),
        .csr_rresp              (),
        
        .esdi_transfer_req      (esdi_transfer_req),
        .esdi_command_data      (),
        .esdi_transfer_ack      (esdi_transfer_ack),
        .esdi_confstat_data     (1'b1),
        .esdi_command_complete  (1'b0),
        .esdi_attention         (1'b0)
    );

    always #5 csr_aclk <= !csr_aclk;

    initial
    begin
        csr_aclk <= 0;
        csr_aresetn <= 0;
        csr_awvalid <= 0;
        csr_awaddr <= 0;
        csr_wvalid <= 0;
        csr_wdata <= 0;
        csr_arvalid <= 0;
        csr_araddr <= 0;

        #10;

        csr_aresetn <= 1;

        #10;

        csr_awvalid <= 1;
        csr_awaddr <= 4;
        csr_wvalid <= 1;
        csr_wdata <= 32'h0001de05;

        #10;

        csr_awvalid <= 0;
        csr_awaddr <= 0;
        csr_wvalid <= 0;
        csr_wdata <= 32'h00000000;


    end

endmodule