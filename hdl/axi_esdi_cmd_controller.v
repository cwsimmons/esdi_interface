
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

module axi_esdi_cmd_controller #(
    // Resonable settings assuming 100 MHz clock
    parameter DATA_SETUP = 6, // Command Data setup time. Minimum is 50ns
    parameter ACK_TO_NREQ = 6, // Transfer Ack to Transfer Req deassert. Minimum is 50ns
    parameter BIT_TIMEOUT = 10_000_00 // 10ms
) (
    input csr_aclk,
    input csr_aresetn,

    input csr_awvalid,
    output csr_awready,
    input [4:0] csr_awaddr,
    input [2:0] csr_awprot,

    input csr_wvalid,
    output csr_wready,
    input [31:0] csr_wdata,
    input [3:0] csr_wstrb,

    output reg csr_bvalid,
    input csr_bready,
    output reg [1:0] csr_bresp,

    input csr_arvalid,
    output csr_arready,
    input [4:0] csr_araddr,
    input [2:0] csr_arprot,

    output reg csr_rvalid,
    input csr_rready,
    output reg [31:0] csr_rdata,
    output reg [1:0] csr_rresp,

    output reg esdi_transfer_req,
    output reg esdi_command_data,
    input esdi_transfer_ack,
    input esdi_confstat_data,
    input esdi_command_complete,
    input esdi_attention,
    input esdi_ready,
    input esdi_drive_selected,
    output reg [2:0] esdi_drive_select,
    output reg [3:0] esdi_head_select
);

    reg write_addr_valid;
    reg write_data_valid;
    reg [4:0] write_addr;
    reg [31:0] write_data;

    assign csr_awready = !write_addr_valid;
    assign csr_wready = !write_data_valid;
    assign csr_arready = !csr_rvalid || csr_rready;

    reg [31:0] control_register;

    reg buffered_data_out_valid;
    reg [31:0] buffered_data_out;

    reg buffered_data_in_valid;
    reg [31:0] buffered_data_in;

    reg [2:0] state;
    reg reading;
    reg [5:0] bit_count;
    reg [31:0] cycle_count;
    reg [16:0] data_out;    // bit 0 is odd parity bit
    reg is_query;           // 0 = command, 1 = configuration/status transfer
    reg [16:0] data_in;

    reg [2:0] esdi_transfer_ack_shift;
    reg [2:0] esdi_confstat_data_shift;
    reg [2:0] esdi_command_complete_shift;
    reg [2:0] esdi_attention_shift;
    reg [2:0] esdi_ready_shift;
    reg [2:0] esdi_drive_selected_shift;


    always @(posedge csr_aclk)
    begin
        if (!csr_aresetn)
        begin
            esdi_transfer_req <= 1;
            esdi_command_data <= 1;
            state <= 0;
            buffered_data_out_valid <= 0;
            buffered_data_in_valid <= 0;

            write_addr_valid <= 0;
            write_data_valid <= 0;
            csr_bvalid <= 0;
            csr_rvalid <= 0;
        end
        else
        begin

            /* Serial Processing */

            cycle_count <= cycle_count + 1;

            esdi_transfer_ack_shift <= {esdi_transfer_ack, esdi_transfer_ack_shift[2:1]};
            esdi_confstat_data_shift <= {esdi_confstat_data, esdi_confstat_data_shift[2:1]};
            esdi_command_complete_shift <= {esdi_command_complete, esdi_command_complete_shift[2:1]};
            esdi_attention_shift <= {esdi_attention, esdi_attention_shift[2:1]};
            esdi_ready_shift <= {esdi_ready, esdi_ready_shift[2:1]};
            esdi_drive_selected_shift <= {esdi_drive_selected, esdi_drive_selected_shift[2:1]};
            

            // Wait for data to send
            if (state == 0)
            begin
                if (buffered_data_out_valid)
                begin
                    buffered_data_out_valid <= 0;
                    data_out <= {buffered_data_out[15:0], ~^buffered_data_out[15:0]};
                    is_query <= buffered_data_out[16];
                    state <= 1;
                    reading <= 0;
                    bit_count <= 0;
                    cycle_count <= 0;
                end
                esdi_transfer_req <= 1;
                esdi_command_data <= 1;
            end
            // Send a bit of data
            else if (state == 1)
            begin
                
                if (cycle_count == 0)
                begin
                    if (!reading)
                    begin
                        esdi_command_data <= !data_out[16];
                        data_out <= data_out << 1;
                    end
                    bit_count <= bit_count + 1;
                end
                else if (cycle_count == DATA_SETUP)
                begin
                    esdi_transfer_req <= 0;
                    state <= 2;
                    cycle_count <= 0;
                end
            end
            // Wait for Ack
            else if (state == 2)
            begin

                if (!esdi_transfer_ack_shift[0])
                begin
                    state <= 3;
                    cycle_count <= 0;
                    if (reading)
                    begin
                        data_in <= {data_in[15:0], !esdi_confstat_data_shift[0]};
                    end
                end
                else if (cycle_count == BIT_TIMEOUT)
                begin
                    state <= 0;
                    if (is_query)
                    begin
                        buffered_data_in_valid <= 1;
                        buffered_data_in <= {15'h1, 17'h0};
                    end
                end
            end
            // Wait some time to deassert req
            else if (state == 3)
            begin

                if (cycle_count == ACK_TO_NREQ)
                begin
                    esdi_transfer_req <= 1;
                    state <= 4;
                    cycle_count <= 0;
                end
            end
            // Wait for Ack deassert
            else if (state == 4)
            begin

                if (esdi_transfer_ack_shift[0])
                begin

                    if (bit_count == 17)
                    begin
                        if (is_query)
                        begin
                            if (!reading)
                            begin
                                // Go back to transfering bit 0, but this time reading instead of writing
                                state <= 1;
                                reading <= 1;
                                bit_count <= 0;
                                cycle_count <= 0;
                            end
                            else
                            begin
                                // We've read back 17 bits, so return the results to the user
                                state <= 0;
                                buffered_data_in_valid <= 1;
                                buffered_data_in <= {15'h0, (~^data_in[16:1] != data_in[0]), data_in[16:1]};
                            end
                        end
                        else
                        begin
                            // We've finished sending 17 bits of a command, return to idle state
                            state <= 0;
                        end

                    end
                    else
                    begin
                        // We have more bits to process
                        state <= 1;
                        cycle_count <= 0;
                    end

                end
                else if (cycle_count == BIT_TIMEOUT)
                begin
                    state <= 0;
                    if (is_query)
                    begin
                        buffered_data_in_valid <= 1;
                        buffered_data_in <= {15'h1, 17'h0};
                    end
                end
            end

            /* Register Interface*/

            if (csr_bready)
                csr_bvalid <= 0;

            if (csr_rready)
                csr_rvalid <= 0;

            if (csr_awvalid && csr_awready)
            begin
                write_addr_valid <= 1;
                write_addr <= csr_awaddr;
            end

            if (csr_wvalid && csr_wready)
            begin
                write_data_valid <= 1;
                write_data <= csr_wdata;
            end

            if (write_addr_valid && write_data_valid && (!csr_bvalid || csr_bready))
            begin
                write_addr_valid <= 0;
                write_data_valid <= 0;

                case (write_addr[4:2])
                    0 : control_register <= write_data;
                    1 : begin
                        buffered_data_out_valid <= 1;
                        buffered_data_out <= write_data;
                    end
                    2 : esdi_drive_select <= write_data[2:0];
                    3 : esdi_head_select <= write_data[3:0];
                endcase

                csr_bvalid <= 1;
                csr_bresp <= 2'b00;
            end

            if (csr_arvalid && (!csr_rvalid || csr_rready))
            begin

                case (csr_araddr[4:2])
                    0 : csr_rdata <= {30'h0, buffered_data_in_valid, buffered_data_out_valid};
                    1 : begin
                        csr_rdata <= buffered_data_in;
                        buffered_data_in_valid <= 0;
                    end
                    2 : csr_rdata <= {29'h0, esdi_drive_select};
                    3 : csr_rdata <= {28'h0, esdi_head_select};
                    4 : csr_rdata <= {28'h0, esdi_drive_selected_shift[0], esdi_command_complete_shift[0], esdi_attention_shift[0], esdi_ready_shift[0]};
                endcase

                csr_rvalid <= 1;
                csr_rresp <= 2'b00;
            end

        end


    end


endmodule