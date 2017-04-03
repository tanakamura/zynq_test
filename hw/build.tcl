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
    {apply_board_preset "1" } \
    [get_bd_cells ps]

set_property -dict \
    [list \
         CONFIG.PCW_USE_DDR {0}\
         CONFIG.PCW_USE_M_AXI_GP0 {1}\
         CONFIG.PCW_USE_M_AXI_GP1 {0}\
         CONFIG.PCW_USE_S_AXI_GP0 {1}\
         CONFIG.PCW_USE_S_AXI_GP1 {0}\
         CONFIG.PCW_USE_S_AXI_ACP {0}\
         CONFIG.PCW_USE_S_AXI_HP0 {0}\
         CONFIG.PCW_USE_S_AXI_HP1 {0}\
         CONFIG.PCW_USE_S_AXI_HP2 {0}\
         CONFIG.PCW_USE_S_AXI_HP3 {0}]\
    [get_bd_cells ps]
save_bd_design

set bd_file ${out_dir}/p.srcs/sources_1/bd/design_1/design_1.bd
set bd_source ${out_dir}/p.srcs/sources_1/bd/design_1/hdl/design_1.v

open_bd_design $bd_file

proc export_axi { name s_m} {
    create_bd_intf_port -mode $s_m -vlnv xilinx.com:interface:aximm_rtl:1.0 $name
    set ext_port [get_bd_intf_ports $name]
    set ps_pin [get_bd_intf_pins ps/$name]

    set_property -dict [list CONFIG.DATA_WIDTH [get_property CONFIG.DATA_WIDTH [get_bd_intf_pins ps/$name]] CONFIG.PROTOCOL [get_property CONFIG.PROTOCOL [get_bd_intf_pins ps/$name]] CONFIG.ID_WIDTH [get_property CONFIG.ID_WIDTH [get_bd_intf_pins ps/$name]] CONFIG.AWUSER_WIDTH [get_property CONFIG.AWUSER_WIDTH [get_bd_intf_pins ps/$name]] CONFIG.ARUSER_WIDTH [get_property CONFIG.ARUSER_WIDTH [get_bd_intf_pins ps/$name]] CONFIG.HAS_REGION [get_property CONFIG.HAS_REGION [get_bd_intf_pins ps/$name]] CONFIG.NUM_READ_OUTSTANDING [get_property CONFIG.NUM_READ_OUTSTANDING [get_bd_intf_pins ps/$name]] CONFIG.NUM_WRITE_OUTSTANDING [get_property CONFIG.NUM_WRITE_OUTSTANDING [get_bd_intf_pins ps/$name]] CONFIG.MAX_BURST_LENGTH [get_property CONFIG.MAX_BURST_LENGTH [get_bd_intf_pins ps/$name]]] [get_bd_intf_ports $name]

    connect_bd_intf_net $ext_port $ps_pin
    connect_bd_net [get_bd_pins ps/FCLK_CLK0] [get_bd_pins ps/${name}_ACLK]
}

export_axi S_AXI_GP0 Slave
export_axi M_AXI_GP0 Master

#connect_bd_net [get_bd_pins ps/FCLK_CLK0] [get_bd_pins ps/M_AXI_GP0_ACLK]
#connect_bd_net [get_bd_pins ps/FCLK_CLK0] [get_bd_pins ps/M_AXI_GP1_ACLK]
#connect_bd_net [get_bd_pins ps/FCLK_CLK0] [get_bd_pins ps/S_AXI_GP0_ACLK]
#connect_bd_net [get_bd_pins ps/FCLK_CLK0] [get_bd_pins ps/S_AXI_GP1_ACLK]
#connect_bd_net [get_bd_pins ps/FCLK_CLK0] [get_bd_pins ps/S_AXI_ACP_ACLK]
#connect_bd_net [get_bd_pins ps/FCLK_CLK0] [get_bd_pins ps/S_AXI_HP0_ACLK]
#connect_bd_net [get_bd_pins ps/FCLK_CLK0] [get_bd_pins ps/S_AXI_HP1_ACLK]
#connect_bd_net [get_bd_pins ps/FCLK_CLK0] [get_bd_pins ps/S_AXI_HP2_ACLK]
#connect_bd_net [get_bd_pins ps/FCLK_CLK0] [get_bd_pins ps/S_AXI_HP3_ACLK]

generate_target synthesis [get_files $bd_file]

file copy -force ./gen/p.srcs/sources_1/bd/design_1/ip/design_1_ps_0/ps7_init.tcl ../sw
file copy -force ./gen/p.srcs/sources_1/bd/design_1/ip/design_1_ps_0/ps7_init.c ../sw
file copy -force ./gen/p.srcs/sources_1/bd/design_1/ip/design_1_ps_0/ps7_init.h ../sw

