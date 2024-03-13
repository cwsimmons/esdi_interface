module top (
    output [3:0] leds,

    output esdi_transfer_req,
    output esdi_command_data,
    input esdi_transfer_ack,
    // input esdi_confstat_data,
    input esdi_index,
    input esdi_sector,
    output esdi_read_gate,
    input esdi_read_clock,
    input esdi_read_data
);

    soc_bd soc_bd_0 (
        .esdi_transfer_req      (esdi_transfer_req),
        .esdi_command_data      (esdi_command_data),
        .esdi_transfer_ack      (esdi_transfer_ack),
        // .esdi_confstat_data     (esdi_confstat_data),
        .esdi_confstat_data     (1'b1),
        .esdi_command_complete  (1'b0),
        .esdi_attention         (1'b0),
        .esdi_index             (esdi_index),
        .esdi_read_gate         (esdi_read_gate),
        .esdi_sector            (esdi_sector),
        .esdi_read_clock        (esdi_read_clock),
        .esdi_read_data         (esdi_read_data)
    );

endmodule