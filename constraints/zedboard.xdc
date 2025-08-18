

set_property PACKAGE_PIN T22 [get_ports {leds[0]}];  # "LD0"
set_property PACKAGE_PIN T21 [get_ports {leds[1]}];  # "LD1"
set_property PACKAGE_PIN U22 [get_ports {leds[2]}];  # "LD2"
set_property PACKAGE_PIN U21 [get_ports {leds[3]}];  # "LD3"

set_property PACKAGE_PIN T17 [get_ports {esdi_head_select[0]}];  # "FMC-LA07_N"
set_property PACKAGE_PIN T16 [get_ports {esdi_head_select[2]}];  # "FMC-LA07_P"
set_property PACKAGE_PIN J21 [get_ports {esdi_head_select[3]}];  # "FMC-LA08_P"
set_property PACKAGE_PIN N18 [get_ports {esdi_drive_select[1]}];  # "FMC-LA11_N"
set_property PACKAGE_PIN N17 [get_ports {esdi_transfer_req}];  # "FMC-LA11_P"
set_property PACKAGE_PIN P21 [get_ports {esdi_drive_select[0]}];  # "FMC-LA12_N"
set_property PACKAGE_PIN P20 [get_ports {esdi_head_select[1]}];  # "FMC-LA12_P"
set_property PACKAGE_PIN J17 [get_ports {esdi_address_mark_enable}];  # "FMC-LA15_N"
set_property PACKAGE_PIN J16 [get_ports {esdi_read_gate}];  # "FMC-LA15_P"
set_property PACKAGE_PIN K21 [get_ports {esdi_command_data}];  # "FMC-LA16_N"
set_property PACKAGE_PIN J20 [get_ports {esdi_drive_select[2]}];  # "FMC-LA16_P"
set_property PACKAGE_PIN A19 [get_ports {esdi_sector}];  # "FMC-LA24_N"
set_property PACKAGE_PIN A18 [get_ports {esdi_transfer_ack}];  # "FMC-LA24_P"
set_property PACKAGE_PIN C22 [get_ports {esdi_attention}];  # "FMC-LA25_N"
set_property PACKAGE_PIN D22 [get_ports {esdi_confstat_data}];  # "FMC-LA25_P"
set_property PACKAGE_PIN A17 [get_ports {esdi_command_complete}];  # "FMC-LA28_N"
set_property PACKAGE_PIN A16 [get_ports {esdi_ready}];  # "FMC-LA28_P"
set_property PACKAGE_PIN C18 [get_ports {esdi_drive_selected}];  # "FMC-LA29_N"
set_property PACKAGE_PIN C17 [get_ports {esdi_index}];  # "FMC-LA29_P"
set_property PACKAGE_PIN C15 [get_ports {esdi_read_clock}];  # "FMC-LA30_P"
set_property PACKAGE_PIN A21 [get_ports {esdi_read_data}];  # "FMC-LA32_P"



# ----------------------------------------------------------------------------
# IOSTANDARD Constraints
#
# Note that these IOSTANDARD constraints are applied to all IOs currently
# assigned within an I/O bank.  If these IOSTANDARD constraints are 
# evaluated prior to other PACKAGE_PIN constraints being applied, then 
# the IOSTANDARD specified will likely not be applied properly to those 
# pins.  Therefore, bank wide IOSTANDARD constraints should be placed 
# within the XDC file in a location that is evaluated AFTER all 
# PACKAGE_PIN constraints within the target bank have been evaluated.
#
# Un-comment one or more of the following IOSTANDARD constraints according to
# the bank pin assignments that are required within a design.
# ---------------------------------------------------------------------------- 

# Note that the bank voltage for IO Bank 33 is fixed to 3.3V on ZedBoard. 
set_property IOSTANDARD LVCMOS33 [get_ports -of_objects [get_iobanks 33]];

# Set the bank voltage for IO Bank 34 to 1.8V by default.
# set_property IOSTANDARD LVCMOS33 [get_ports -of_objects [get_iobanks 34]];
set_property IOSTANDARD LVCMOS25 [get_ports -of_objects [get_iobanks 34]];
# set_property IOSTANDARD LVCMOS18 [get_ports -of_objects [get_iobanks 34]];

# Set the bank voltage for IO Bank 35 to 1.8V by default.
# set_property IOSTANDARD LVCMOS33 [get_ports -of_objects [get_iobanks 35]];
set_property IOSTANDARD LVCMOS25 [get_ports -of_objects [get_iobanks 35]];
# set_property IOSTANDARD LVCMOS18 [get_ports -of_objects [get_iobanks 35]];

# Note that the bank voltage for IO Bank 13 is fixed to 3.3V on ZedBoard. 
set_property IOSTANDARD LVCMOS33 [get_ports -of_objects [get_iobanks 13]];