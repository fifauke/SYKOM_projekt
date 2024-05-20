/* verilator lint_off UNUSED */
/* verilator lint_off MULTIDRIVEN */
/* verilator lint_off UNDRIVEN */
/* verilator lint_off BLKANDNBLK */
/* verilator lint_off BLKSEQ */
/* verilator lint_off COMBDLY */
/* verilator lint_off WIDTH */

module gpioemu(n_reset, saddress[15:0], srd, swr, sdata_in[31:0], sdata_out[31:0],
gpio_in[31:0], gpio_latch, gpio_out[31:0], clk, gpio_in_s_insp[31:0]);
	
	input n_reset;
	input [15:0] saddress;
	input srd;
	input swr;
	input [31:0] sdata_in;
	output [31:0] sdata_out;	
	input [31:0] gpio_in;
    output [31:0] gpio_in_s_insp;
	input gpio_latch;
	output [31:0] gpio_out;
	reg [31:0] gpio_in_s;   
	reg [31:0] gpio_out_s;
	reg [31:0] sdata_out_s;
    reg[15:0] counter; 
	input clk;
	
    reg [9:0] A;
    reg S; //1 - pracuje, 0 - nie pracuje
    reg [31:0] W;
	
    reg [31:0] primes [0:999];
    reg [9:0] n_prime_rej;
    integer counter_prime; 
    integer n_prime; 
    integer i; 
    integer found_prime;	

	assign sdata_out = sdata_out_s;
	assign gpio_out = counter;
    assign gpio_in_s_insp = gpio_in_s;
    
    //wyznaczanie liczby pierwszej
    always @(posedge clk) 
    begin
        if(S==1)
        begin
            primes[0] = 2; 
            counter_prime = 1; 
            n_prime = 3; 
            n_prime_rej = 1; 

            while (counter_prime < A) 
            begin
                found_prime = 1; 

                for (i = 0; i < counter_prime; i = i + 1) 
                begin
                    if (n_prime % primes[i] == 0) 
                    begin
                        found_prime = 0; 
                        i = counter_prime;
                    end
                end

                if (found_prime) 
                begin
                    primes[counter_prime] = n_prime; 
                    counter_prime = counter_prime + 1; 
                end

                n_prime = n_prime + 2;

                n_prime_rej = n_prime_rej + 1;
            end
            W = primes[A-1];
            counter = counter + 1;
            S = 0;
        end
    end	

	//odpowiedz na reset (przywracanie stanu początkowego)
	always @(negedge n_reset) 
    begin
            gpio_in_s = 0;
            gpio_out_s = 0;
            sdata_out_s = 0;
			A = 0;
			S = 0;
            W = 0;
            counter = 8'd0;
	end
	
	//zapis z GPIO
	always @(posedge swr) 
    begin
        if(S == 0)
        begin
            if(saddress == 16'h258) 
            begin
                A = 0;
                A = sdata_in[9:0];
                S = 1;
            end
        end
	end
	
	//odczyt z GPIO
	always @(posedge srd) 
    begin
		//odczyt wyniku
		if(saddress == 16'h268) 
        begin
			if(S == 0) 
            begin
                sdata_out_s = W/*[31:0]*/;
			end

            else //invalid value
            begin   
                sdata_out_s = 32'b10101010101010101010101010101010;
            end
		end

        //odczyt statusu
		else if(saddress == 16'h270) 
        begin
            sdata_out_s = {31'b0, S};
            //sdata_out_s = 32'habcd;
		end

        //inne adresy
        else
        begin
            sdata_out_s = 0;
        end
	end

endmodule