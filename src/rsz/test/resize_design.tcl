# resize to target_slew
source "helpers.tcl"
read_liberty Nangate45/Nangate45_typ.lib
read_lef Nangate45/Nangate45.lef
read_def reg3.def

create_clock -name clk -period 1 clk
source Nangate45/Nangate45.rc
set_wire_rc -layer metal3
estimate_parasitics -placement

set_load -pin_load 2000 out
report_net out
report_checks -fields {slew input_pin} -digits 3
# Resize_design will wipe out slews for now
resize_design
report_checks -fields {slew input_pin} -digits 3
