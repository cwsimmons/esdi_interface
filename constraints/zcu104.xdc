set_property PACKAGE_PIN D5 [get_ports "leds[0]"];
set_property IOSTANDARD LVCMOS33 [get_ports "leds[0]"];
set_property PACKAGE_PIN D6 [get_ports "leds[1]"];
set_property IOSTANDARD LVCMOS33 [get_ports "leds[1]"];
set_property PACKAGE_PIN A5 [get_ports "leds[2]"];
set_property IOSTANDARD LVCMOS33 [get_ports "leds[2]"];
set_property PACKAGE_PIN B5 [get_ports "leds[3]"];
set_property IOSTANDARD LVCMOS33 [get_ports "leds[3]"];

set_property PACKAGE_PIN G8 [get_ports "esdi_transfer_req"];            # PMOD0_0
set_property IOSTANDARD LVCMOS33 [get_ports "esdi_transfer_req"];       # PMOD0_0
set_property PACKAGE_PIN H8 [get_ports "esdi_command_data"];            # PMOD0_1
set_property IOSTANDARD LVCMOS33 [get_ports "esdi_command_data"];       # PMOD0_1
set_property PACKAGE_PIN G7 [get_ports "esdi_transfer_ack"];            # PMOD0_2
set_property IOSTANDARD LVCMOS33 [get_ports "esdi_transfer_ack"];       # PMOD0_2
set_property PACKAGE_PIN H7 [get_ports "esdi_confstat_data"];           # PMOD0_3
set_property IOSTANDARD LVCMOS33 [get_ports "esdi_confstat_data"];      # PMOD0_3
set_property PACKAGE_PIN G6 [get_ports "esdi_command_complete"];        # PMOD0_4
set_property IOSTANDARD LVCMOS33 [get_ports "esdi_command_complete"];   # PMOD0_4
set_property PACKAGE_PIN H6 [get_ports "esdi_attention"];               # PMOD0_5
set_property IOSTANDARD LVCMOS33 [get_ports "esdi_attention"];          # PMOD0_5