set chip_name xc7z010clg400-1
set board_name digilentinc.com:zybo:part0:1.0
set ipdir [pwd]/ip
set out_dir [pwd]/gen

file delete -force -- $out_dir

file mkdir $out_dir

create_project -force p $out_dir -part $chip_name
set_property board_part $board_name [current_project]

create_bd_design "design_1"
startgroup
create_bd_cell -type ip -vlnv xilinx.com:ip:processing_system7:5.5 ps
endgroup
apply_bd_automation -rule xilinx.com:bd_rule:processing_system7 -config \
    {make_external "FIXED_IO, DDR" apply_board_preset "1" } \
    [get_bd_cells ps]
save_bd_design

set bd_file ${out_dir}/p.srcs/sources_1/bd/design_1/design_1.bd
set bd_source ${out_dir}/p.srcs/sources_1/bd/design_1/hdl/design_1.v

open_bd_design $bd_file
connect_bd_net [get_bd_pins ps/FCLK_CLK0] [get_bd_pins ps/M_AXI_GP0_ACLK]

generate_target synthesis [get_files $bd_file]

file copy -force ./gen/p.srcs/sources_1/bd/design_1/ip/design_1_ps_0/ps7_init.tcl ../sw
file copy -force ./gen/p.srcs/sources_1/bd/design_1/ip/design_1_ps_0/ps7_init.c ../sw
file copy -force ./gen/p.srcs/sources_1/bd/design_1/ip/design_1_ps_0/ps7_init.h ../sw

