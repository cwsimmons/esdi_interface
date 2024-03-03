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

    output esdi_transfer_req,
    output esdi_command_data,
    input esdi_transfer_ack,
    input esdi_confstat_data,
    input esdi_command_complete,
    input esdi_attention
);

    reg write_addr_valid;
    reg write_data_valid;
    reg [4:0] write_addr;
    reg [31:0] write_data;

    assign csr_awready = !write_addr_valid;
    assign csr_wready = !write_data_valid;
    assign csr_arready = !csr_rvalid || csr_rready;

    reg [31:0] control_register;

    always @(posedge csr_aclk)
    begin
        if (!csr_aresetn)
        begin
            write_addr_valid <= 0;
            write_data_valid <= 0;
            csr_bvalid <= 0;
            csr_rvalid <= 0;
        end
        else
        begin

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
                endcase

                csr_bvalid <= 1;
                csr_bresp <= 2'b00;
            end

            if (csr_arvalid && (!csr_rvalid || csr_rready))
            begin

                case (csr_araddr[4:2])
                    0 : csr_rdata <= control_register;
                endcase

                csr_rvalid <= 1;
                csr_rresp <= 2'b00;
            end

        end


    end


endmodule