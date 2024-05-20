`timescale 1ns/100ps

module gpioemu_tb;

	reg n_reset = 1;
	reg [15:0] saddress = 0;
	reg srd = 0;
	reg swr = 0;
	reg [31:0] sdata_in = 0;
	reg [31:0] gpio_in = 0;
	reg gpio_latch = 0;
	reg clk = 0;
	reg ctr = 0;
	
	wire [31:0] gpio_in_s_insp;
	wire [31:0] sdata_out;
	wire [31:0] gpio_out;

initial begin
$dumpfile("gpioemu.vcd");
$dumpvars(0, gpioemu_tb);
end

initial begin
    forever begin
    #5 clk = ~clk;
    end
end

initial begin

//pierwszy reset
# 5 n_reset = 0;
# 5 n_reset = 1;

//test zapisu 
# 5 sdata_in = 4;
# 5 saddress = 16'h258;
# 5 swr = 1;
# 5 swr = 0;

# 5 sdata_in = 5;
# 5 saddress = 16'h258;
# 5 swr = 1;
# 5 swr = 0;

//zly adres
# 5 sdata_in = 4;
# 5 saddress = 16'h248;
# 5 swr = 1;
# 5 swr = 0;


//test odczytu wyniku
# 5 saddress = 16'h268;
# 5 srd = 1;
# 5 srd = 0;

//test odczytu statusu
# 5 saddress = 16'h270;
# 5 srd = 1;
# 5 srd = 0;

//test odczytu wyniku
# 5 saddress = 16'h268;
# 5 srd = 1;
# 5 srd = 0;

//test odczytu zly adres
# 5 saddress = 16'h269;
# 5 srd = 1;
# 5 srd = 0;



end 
gpioemu e1(n_reset, saddress, srd, swr, sdata_in, sdata_out, gpio_in, gpio_latch, gpio_out, clk, gpio_in_s_insp);

initial # 5000 $finish;

endmodule