module axi_esdi_read_datapath #(

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

    input esdi_read_gate,
    input esdi_read_data,
    input esdi_read_clock,

    input gate_for_header,
    input gate_for_data,

    output reg parallel_tvalid,
    input parallel_tready,
    output reg [7:0] parallel_tdata

    output reg sector_tvalid,
    input sector_tready,
    output reg [7:0] sector_tdata
);

    reg write_addr_valid;
    reg write_data_valid;
    reg [4:0] write_addr;
    reg [31:0] write_data;

    assign csr_awready = !write_addr_valid;
    assign csr_wready = !write_data_valid;
    assign csr_arready = !csr_rvalid || csr_rready;

    reg [2:0] read_data_shift_reg;
    reg [2:0] read_clock_shift_reg;

    reg [31:0] control_register;
    reg [7:0] internal_clocks_per_bit;

    wire enable;
    wire decode_sectors;
    wire ignore_gate;
    wire use_internal_clock;
    assign enable = control_register[0];
    assign decode_sectors = control_register[1];
    assign ignore_gate = control_register[2];
    assign use_internal_clock = control_register[3];

    wire sample_pulse;
    reg [3:0] bit_count;
    reg internal_clock;
    reg [7:0] internal_clock_count;
    reg new_bit;
    reg new_bit_valid;
    reg [7:0] data_in;


    assign sample_pulse = (use_internal_clock) ? internal_clock : (!read_clock_shift_reg[0] && read_clock_shift_reg[1]);

    always @(posedge csr_aclk)
    begin
        if (!csr_aresetn)
        begin
            control_register <= {28'h0, 4'b0010};
            internal_clocks_per_bit <= 4;

            read_clock_shift_reg <= 3'b000;
            bit_count <= 0;
            internal_clock <= 0;
            internal_clock_count <= 0;

            write_addr_valid <= 0;
            write_data_valid <= 0;
            csr_bvalid <= 0;
            csr_rvalid <= 0;
        end
        else
        begin

            read_data_shift_reg <= {esdi_read_data, read_data_shift_reg[2:1]};
            read_clock_shift_reg <= {esdi_read_clock, read_clock_shift_reg[2:1]};

            if (parallel_tready)
                parallel_tvalid <= 0;

            if (sector_tready)
                sector_tvalid <= 0;

            internal_clock <= 0;
            new_bit_valid <= 0;

            if (enable)
            begin

                // Sample read data line
                if (sample_pulse && (esdi_read_gate || ignore_gate))
                begin
                    new_bit_valid <= 1;
                    new_bit <= read_data_shift_reg[0];
                end

                // Generate an internal sample clock
                if (internal_clock_count == internal_clocks_per_bit - 1)
                begin
                    internal_clock_count <= 0;
                    internal_clock <= 1;
                end
                else
                begin
                    internal_clock_count <= internal_clock_count + 1;
                end

                if (new_bit_valid)
                begin
                    data_in <= {data_in[6:0], new_bit};

                    if (bit_count == 7)
                    begin
                        bit_count <= 0;
                        parallel_tvalid <= 1;
                        parallel_tdata <= {data_in[6:0], new_bit};
                    end
                    else
                    begin
                        bit_count <= bit_count + 1;
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
                    1 : internal_clocks_per_bit <= write_data[7:0];
                endcase

                csr_bvalid <= 1;
                csr_bresp <= 2'b00;
            end

            if (csr_arvalid && (!csr_rvalid || csr_rready))
            begin

                case (csr_araddr[4:2])
                    0 : csr_rdata <= control_register;
                    1 : csr_rdata <= {24'h0, internal_clocks_per_bit};
                endcase

                csr_rvalid <= 1;
                csr_rresp <= 2'b00;
            end

        end


    end


endmodule