
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
    reg [15:0] sector_counter;

    wire enable;
    wire soft_reset;
    wire read_address_area;
    wire read_data_areas;
    wire read_without_tasking;
    assign enable = control_register[0];
    assign soft_reset = control_register[1];
    assign read_address_area = control_register[2];
    assign read_data_areas = control_register[3];
    assign read_without_tasking = control_register[4];

    reg task_write_valid;
    wire task_write_ready;
    reg [15:0] task_write_data;

    wire buffered_task_valid;
    wire [15:0] buffered_task_data;

    wire [6:0] tasks_pending;

    reg current_task_valid;
    reg [15:0] current_task_data;


    fifo #(16, 7, 0) tasking_fifo (
        .clk            (csr_aclk),
        .resetn         (!(!csr_aresetn || soft_reset)),

        .in_tvalid      (task_write_valid),
        .in_tready      (task_write_ready),
        .in_tdata       (task_write_data),
        .in_tlast       (1'b0),

        .out_tvalid     (buffered_task_valid),
        .out_tready     (enable && !current_task_valid),
        .out_tdata      (buffered_task_data),
        .out_tlast      (),

        .num_free       (),
        .num_used       (tasks_pending)
    );


    always @(posedge csr_aclk)
    begin
        if (!csr_aresetn)
        begin
            esdi_read_gate <= 0;
            index_shift_reg <= 3'b111;
            sector_shift_reg <= 3'b111;

            control_register <= 32'b0000;
            task_write_valid <= 0;
            current_task_valid <= 0;

            write_addr_valid <= 0;
            write_data_valid <= 0;
            csr_bvalid <= 0;
            csr_rvalid <= 0;
        end
        else
        begin


            index_shift_reg <= {esdi_index, index_shift_reg[2:1]};
            sector_shift_reg <= {esdi_sector, sector_shift_reg[2:1]};

            if (task_write_ready)
                task_write_valid <= 0;

            if (enable)
            begin

                if (buffered_task_valid && !current_task_valid)
                begin
                    current_task_valid <= 1;
                    current_task_data <= buffered_task_data;
                end

                cycle_count <= cycle_count + 1;
                if (index_shift_reg[0] && !index_shift_reg[1])
                begin
                    cycle_count <= 0;
                    sector_counter <= 0;

                    if (sector_counter == current_task_data)
                        current_task_valid <= 0;
                end
                if (sector_shift_reg[0] && !sector_shift_reg[1])
                begin
                    cycle_count <= 0;
                    sector_counter <= sector_counter + 1;

                    if (sector_counter == current_task_data)
                        current_task_valid <= 0;
                end

                if (cycle_count == address_assert &&
                    read_address_area &&
                    (read_without_tasking || (current_task_valid && sector_counter == current_task_data))
                )
                    esdi_read_gate <= 1;

                if (cycle_count == address_deassert)
                    esdi_read_gate <= 0;

                if (cycle_count == data_area_assert &&
                    read_data_areas &&
                    (read_without_tasking || (current_task_valid && sector_counter == current_task_data))
                )
                    esdi_read_gate <= 1;

                if (cycle_count == data_area_deassert)
                    esdi_read_gate <= 0;

            end
            else
            begin
                esdi_read_gate <= 0;
            end

            if (soft_reset)
            begin
                task_write_valid <= 0;
                current_task_valid <= 0;
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
                    2 : address_assert <= write_data;
                    3 : address_deassert <= write_data;
                    4 : data_area_assert <= write_data;
                    5 : data_area_deassert <= write_data;
                    6 : begin
                        task_write_valid <= 1;
                        task_write_data <= write_data[15:0];
                    end
                endcase

                csr_bvalid <= 1;
                csr_bresp <= 2'b00;
            end

            if (csr_arvalid && (!csr_rvalid || csr_rready))
            begin

                case (csr_araddr[4:2])
                    0 : csr_rdata <= control_register;
                    1 : csr_rdata <= {30'b0, current_task_valid, task_write_valid};
                    2 : csr_rdata <= address_assert;
                    3 : csr_rdata <= address_deassert;
                    4 : csr_rdata <= data_area_assert;
                    5 : csr_rdata <= data_area_deassert;
                    6 : csr_rdata <= {25'b0, tasks_pending};
                endcase

                csr_rvalid <= 1;
                csr_rresp <= 2'b00;
            end

        end


    end


endmodule
