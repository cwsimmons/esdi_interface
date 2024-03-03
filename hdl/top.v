module top (
    output [3:0] leds,

    output esdi_transfer_req,
    output esdi_command_data,
    input esdi_transfer_ack,
    input esdi_confstat_data,
    input esdi_command_complete,
    input esdi_attention
);

    soc_bd soc_bd_0 (
        .esdi_transfer_req      (esdi_transfer_req),
        .esdi_command_data      (esdi_command_data),
        .esdi_transfer_ack      (esdi_transfer_ack),
        .esdi_confstat_data     (esdi_confstat_data),
        .esdi_command_complete  (esdi_command_complete),
        .esdi_attention         (esdi_attention)
    );

endmodule