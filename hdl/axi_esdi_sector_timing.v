
module axi_esdi_sector_timing (
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

    input esdi_index,
    input esdi_sector,
    output reg esdi_read_gate
);

    reg write_addr_valid;
    reg write_data_valid;
    reg [4:0] write_addr;
    reg [31:0] write_data;

    assign csr_awready = !write_addr_valid;
    assign csr_wready = !write_data_valid;
    assign csr_arready = !csr_rvalid || csr_rready;

    reg [2:0] index_shift_reg;
    reg [2:0] sector_shift_reg;

    reg [31:0] control_register;
    reg [31:0] address_assert;
    reg [31:0] address_deassert;
    reg [31:0] data_area_assert;
    reg [31:0] data_area_deassert;

    reg [31:0] cycle_count;

    wire enable;
    assign enable = control_register[0];


    always @(posedge csr_aclk)
    begin
        if (!csr_aresetn)
        begin
            esdi_read_gate <= 0;
            index_shift_reg <= 3'b111;
            sector_shift_reg <= 3'b111;

            write_addr_valid <= 0;
            write_data_valid <= 0;
            csr_bvalid <= 0;
            csr_rvalid <= 0;
        end
        else
        begin


            index_shift_reg <= {esdi_index, index_shift_reg[2:1]};
            sector_shift_reg <= {esdi_sector, sector_shift_reg[2:1]};

            if (enable)
            begin
                cycle_count <= cycle_count + 1;
                if (index_shift_reg[0] && !index_shift_reg[1])
                    cycle_count <= 0;
                if (sector_shift_reg[0] && !sector_shift_reg[1])
                    cycle_count <= 0;

                if (cycle_count == address_assert)
                    esdi_read_gate <= 1;
                if (cycle_count == address_deassert)
                    esdi_read_gate <= 0;
                if (cycle_count == data_area_assert)
                    esdi_read_gate <= 1;
                if (cycle_count == data_area_deassert)
                    esdi_read_gate <= 0;
            end
            else
            begin
                esdi_read_gate <= 0;
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
                    1 : address_assert <= write_data;
                    2 : address_deassert <= write_data;
                    3 : data_area_assert <= write_data;
                    4 : data_area_deassert <= write_data;
                endcase

                csr_bvalid <= 1;
                csr_bresp <= 2'b00;
            end

            if (csr_arvalid && (!csr_rvalid || csr_rready))
            begin

                case (csr_araddr[4:2])
                    0 : csr_rdata <= control_register;
                    1 : csr_rdata <= address_assert;
                    2 : csr_rdata <= address_deassert;
                    3 : csr_rdata <= data_area_assert;
                    4 : csr_rdata <= data_area_deassert;
                endcase

                csr_rvalid <= 1;
                csr_rresp <= 2'b00;
            end

        end


    end


endmodule
