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
    output [3:0] esdi_head_select,
    output esdi_drive_select
);

    wire esdi_transfer_reqn;
    wire esdi_command_datan;

    assign esdi_transfer_req = !esdi_transfer_reqn;
    assign esdi_command_data = !esdi_command_datan;

    wire [3:0] drive_select;
    assign esdi_drive_select = drive_select == 4'h2;

    soc_bd soc_bd_0 (
        .esdi_transfer_req      (esdi_transfer_reqn),
        .esdi_command_data      (esdi_command_datan),
        .esdi_transfer_ack      (esdi_transfer_ack),
        .esdi_confstat_data     (esdi_confstat_data),
        .esdi_command_complete  (1'b0),
        .esdi_attention         (esdi_attention),
        .esdi_index             (esdi_index),
        .esdi_read_gate         (esdi_read_gate),
        .esdi_sector            (esdi_sector),
        .esdi_read_clock        (esdi_read_clock),
        .esdi_read_data         (esdi_read_data),
        .esdi_drive_select      (drive_select),
        .esdi_head_select       (esdi_head_select),
        .esdi_ready             (esdi_ready)

    );

endmodule