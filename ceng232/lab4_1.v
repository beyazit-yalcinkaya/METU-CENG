`timescale 1ns / 1ps
module FUNCROM (input [3:0] romAddress, output reg[4:0] romData);

/*Write your code here*/

	always@(*) begin
		case(romAddress)
			4'b0000: romData = 5'b00000;
			4'b0001: romData = 5'b00010;
			4'b0010: romData = 5'b00100;
			4'b0011: romData = 5'b00111;
			4'b0100: romData = 5'b01010;
			4'b0101: romData = 5'b01011;
			4'b0110: romData = 5'b01101;
			4'b0111: romData = 5'b01110;
			4'b1000: romData = 5'b10001;
			4'b1001: romData = 5'b10010;
			4'b1010: romData = 5'b10100;
			4'b1011: romData = 5'b10111;
			4'b1100: romData = 5'b11001;
			4'b1101: romData = 5'b11010;
			4'b1110: romData = 5'b11110;
			4'b1111: romData = 5'b11111;
		endcase
	end
endmodule


module FUNCRAM (input mode,input [3:0] ramAddress, input [4:0] dataIn,input op, input [1:0] arg,  input CLK, output reg [8:0] dataOut);

/*Write your code here*/
	
	reg [8:0] ram[0:15];
	
	initial begin
		ram[0]  = 9'b000000000;
		ram[1]  = 9'b000000000;
		ram[2]  = 9'b000000000;
		ram[3]  = 9'b000000000;
		ram[4]  = 9'b000000000;
		ram[5]  = 9'b000000000;
		ram[6]  = 9'b000000000;
		ram[7]  = 9'b000000000;
		ram[8]  = 9'b000000000;
		ram[9]  = 9'b000000000;
		ram[10] = 9'b000000000;
		ram[11] = 9'b000000000;
		ram[12] = 9'b000000000;
		ram[13] = 9'b000000000;
		ram[14] = 9'b000000000;
		ram[15] = 9'b000000000;
	end
	
	always@(*) begin
		if (mode == 0) begin
			// Read
			dataOut = ram[ramAddress];
		end
	end

	always@(posedge CLK) begin
		if (mode == 1) begin
			// Write
			if (op == 0) begin
				// Modulo 7
				case((((dataIn[4] == 1'b0) ? 1 : -1) * ((arg == 2'b00) ? 16 : ((arg == 2'b01) ? 1 : ((arg == 2'b10) ?  1 : 16)))
					+  ((dataIn[3] == 1'b0) ? 1 : -1) * ((arg == 2'b00) ?  8 : ((arg == 2'b01) ? 1 : ((arg == 2'b10) ? -1 : -8)))
					+  ((dataIn[2] == 1'b0) ? 1 : -1) * ((arg == 2'b00) ?  4 : ((arg == 2'b01) ? 1 : ((arg == 2'b10) ?  1 :  4)))
					+  ((dataIn[1] == 1'b0) ? 1 : -1) * ((arg == 2'b00) ?  2 : ((arg == 2'b01) ? 1 : ((arg == 2'b10) ? -1 : -2)))
					+  ((dataIn[0] == 1'b0) ? 1 : -1)) % 7)
					-6: begin ram[ramAddress] <= 9'b000000001; dataOut <= 9'b000000001; end
					-5: begin ram[ramAddress] <= 9'b000000010; dataOut <= 9'b000000010; end
					-4: begin ram[ramAddress] <= 9'b000000011; dataOut <= 9'b000000011; end
					-3: begin ram[ramAddress] <= 9'b000000100; dataOut <= 9'b000000100; end
					-2: begin ram[ramAddress] <= 9'b000000101; dataOut <= 9'b000000101; end
					-1: begin ram[ramAddress] <= 9'b000000110; dataOut <= 9'b000000110; end
					 0: begin ram[ramAddress] <= 9'b000000000; dataOut <= 9'b000000000; end
					 1: begin ram[ramAddress] <= 9'b000000001; dataOut <= 9'b000000001; end
					 2: begin ram[ramAddress] <= 9'b000000010; dataOut <= 9'b000000010; end
					 3: begin ram[ramAddress] <= 9'b000000011; dataOut <= 9'b000000011; end
					 4: begin ram[ramAddress] <= 9'b000000100; dataOut <= 9'b000000100; end
					 5: begin ram[ramAddress] <= 9'b000000101; dataOut <= 9'b000000101; end
					 6: begin ram[ramAddress] <= 9'b000000110; dataOut <= 9'b000000110; end
				endcase
			end
			else begin
				// Derivative
				case(4 * ((dataIn[4] == 1'b0) ? 1 : -1) * ((arg == 2'b00) ?  8 : ((arg == 2'b01) ? 1 : ((arg == 2'b10) ? -1 : -8)))
					+ 3 * ((dataIn[3] == 1'b0) ? 1 : -1) * ((arg == 2'b00) ?  4 : ((arg == 2'b01) ? 1 : ((arg == 2'b10) ?  1 :  4)))
					+ 2 * ((dataIn[2] == 1'b0) ? 1 : -1) * ((arg == 2'b00) ?  2 : ((arg == 2'b01) ? 1 : ((arg == 2'b10) ? -1 : -2)))
					+     ((dataIn[1] == 1'b0) ? 1 : -1))
					-49: begin ram[ramAddress] <= 9'b100110001; dataOut <= 9'b100110001; end
					-48: begin ram[ramAddress] <= 9'b100110000; dataOut <= 9'b100110000; end
					-47: begin ram[ramAddress] <= 9'b100101111; dataOut <= 9'b100101111; end
					-46: begin ram[ramAddress] <= 9'b100101110; dataOut <= 9'b100101110; end
					-45: begin ram[ramAddress] <= 9'b100101101; dataOut <= 9'b100101101; end
					-44: begin ram[ramAddress] <= 9'b100101100; dataOut <= 9'b100101100; end
					-43: begin ram[ramAddress] <= 9'b100101011; dataOut <= 9'b100101011; end
					-42: begin ram[ramAddress] <= 9'b100101010; dataOut <= 9'b100101010; end
					-41: begin ram[ramAddress] <= 9'b100101001; dataOut <= 9'b100101001; end
					-40: begin ram[ramAddress] <= 9'b100101000; dataOut <= 9'b100101000; end
					-39: begin ram[ramAddress] <= 9'b100100111; dataOut <= 9'b100100111; end
					-38: begin ram[ramAddress] <= 9'b100100110; dataOut <= 9'b100100110; end
					-37: begin ram[ramAddress] <= 9'b100100101; dataOut <= 9'b100100101; end
					-36: begin ram[ramAddress] <= 9'b100100100; dataOut <= 9'b100100100; end
					-35: begin ram[ramAddress] <= 9'b100100011; dataOut <= 9'b100100011; end
					-34: begin ram[ramAddress] <= 9'b100100010; dataOut <= 9'b100100010; end
					-33: begin ram[ramAddress] <= 9'b100100001; dataOut <= 9'b100100001; end
					-32: begin ram[ramAddress] <= 9'b100100000; dataOut <= 9'b100100000; end
					-31: begin ram[ramAddress] <= 9'b100011111; dataOut <= 9'b100011111; end
					-30: begin ram[ramAddress] <= 9'b100011110; dataOut <= 9'b100011110; end
					-29: begin ram[ramAddress] <= 9'b100011101; dataOut <= 9'b100011101; end
					-28: begin ram[ramAddress] <= 9'b100011100; dataOut <= 9'b100011100; end
					-27: begin ram[ramAddress] <= 9'b100011011; dataOut <= 9'b100011011; end
					-26: begin ram[ramAddress] <= 9'b100011010; dataOut <= 9'b100011010; end
					-25: begin ram[ramAddress] <= 9'b100011001; dataOut <= 9'b100011001; end
					-24: begin ram[ramAddress] <= 9'b100011000; dataOut <= 9'b100011000; end
					-23: begin ram[ramAddress] <= 9'b100010111; dataOut <= 9'b100010111; end
					-22: begin ram[ramAddress] <= 9'b100010110; dataOut <= 9'b100010110; end
					-21: begin ram[ramAddress] <= 9'b100010101; dataOut <= 9'b100010101; end
					-20: begin ram[ramAddress] <= 9'b100010100; dataOut <= 9'b100010100; end
					-19: begin ram[ramAddress] <= 9'b100010011; dataOut <= 9'b100010011; end
					-18: begin ram[ramAddress] <= 9'b100010010; dataOut <= 9'b100010010; end
					-17: begin ram[ramAddress] <= 9'b100010001; dataOut <= 9'b100010001; end
					-16: begin ram[ramAddress] <= 9'b100010000; dataOut <= 9'b100010000; end
					-15: begin ram[ramAddress] <= 9'b100001111; dataOut <= 9'b100001111; end
					-14: begin ram[ramAddress] <= 9'b100001110; dataOut <= 9'b100001110; end
					-13: begin ram[ramAddress] <= 9'b100001101; dataOut <= 9'b100001101; end
					-12: begin ram[ramAddress] <= 9'b100001100; dataOut <= 9'b100001100; end
					-11: begin ram[ramAddress] <= 9'b100001011; dataOut <= 9'b100001011; end
					-10: begin ram[ramAddress] <= 9'b100001010; dataOut <= 9'b100001010; end
					 -9: begin ram[ramAddress] <= 9'b100001001; dataOut <= 9'b100001001; end
					 -8: begin ram[ramAddress] <= 9'b100001000; dataOut <= 9'b100001000; end
					 -7: begin ram[ramAddress] <= 9'b100000111; dataOut <= 9'b100000111; end
					 -6: begin ram[ramAddress] <= 9'b100000110; dataOut <= 9'b100000110; end
					 -5: begin ram[ramAddress] <= 9'b100000101; dataOut <= 9'b100000101; end
					 -4: begin ram[ramAddress] <= 9'b100000100; dataOut <= 9'b100000100; end
					 -3: begin ram[ramAddress] <= 9'b100000011; dataOut <= 9'b100000011; end
					 -2: begin ram[ramAddress] <= 9'b100000010; dataOut <= 9'b100000010; end
					 -1: begin ram[ramAddress] <= 9'b100000001; dataOut <= 9'b100000001; end
					  0: begin ram[ramAddress] <= 9'b000000000; dataOut <= 9'b000000000; end
				     1: begin ram[ramAddress] <= 9'b000000001; dataOut <= 9'b000000001; end
					  2: begin ram[ramAddress] <= 9'b000000010; dataOut <= 9'b000000010; end
					  3: begin ram[ramAddress] <= 9'b000000011; dataOut <= 9'b000000011; end
					  4: begin ram[ramAddress] <= 9'b000000100; dataOut <= 9'b000000100; end
					  5: begin ram[ramAddress] <= 9'b000000101; dataOut <= 9'b000000101; end
				 	  6: begin ram[ramAddress] <= 9'b000000110; dataOut <= 9'b000000110; end
					  7: begin ram[ramAddress] <= 9'b000000111; dataOut <= 9'b000000111; end
					  8: begin ram[ramAddress] <= 9'b000001000; dataOut <= 9'b000001000; end
					  9: begin ram[ramAddress] <= 9'b000001001; dataOut <= 9'b000001001; end
					 10: begin ram[ramAddress] <= 9'b000001010; dataOut <= 9'b000001010; end
					 11: begin ram[ramAddress] <= 9'b000001011; dataOut <= 9'b000001011; end
					 12: begin ram[ramAddress] <= 9'b000001100; dataOut <= 9'b000001100; end
					 13: begin ram[ramAddress] <= 9'b000001101; dataOut <= 9'b000001101; end
					 14: begin ram[ramAddress] <= 9'b000001110; dataOut <= 9'b000001110; end
					 15: begin ram[ramAddress] <= 9'b000001111; dataOut <= 9'b000001111; end
					 16: begin ram[ramAddress] <= 9'b000010000; dataOut <= 9'b000010000; end
					 17: begin ram[ramAddress] <= 9'b000010001; dataOut <= 9'b000010001; end
					 18: begin ram[ramAddress] <= 9'b000010010; dataOut <= 9'b000010010; end
					 19: begin ram[ramAddress] <= 9'b000010011; dataOut <= 9'b000010011; end
					 20: begin ram[ramAddress] <= 9'b000010100; dataOut <= 9'b000010100; end
					 21: begin ram[ramAddress] <= 9'b000010101; dataOut <= 9'b000010101; end
					 22: begin ram[ramAddress] <= 9'b000010110; dataOut <= 9'b000010110; end
					 23: begin ram[ramAddress] <= 9'b000010111; dataOut <= 9'b000010111; end
					 24: begin ram[ramAddress] <= 9'b000011000; dataOut <= 9'b000011000; end
					 25: begin ram[ramAddress] <= 9'b000011001; dataOut <= 9'b000011001; end
					 26: begin ram[ramAddress] <= 9'b000011010; dataOut <= 9'b000011010; end
					 27: begin ram[ramAddress] <= 9'b000011011; dataOut <= 9'b000011011; end
					 28: begin ram[ramAddress] <= 9'b000011100; dataOut <= 9'b000011100; end
					 29: begin ram[ramAddress] <= 9'b000011101; dataOut <= 9'b000011101; end
					 30: begin ram[ramAddress] <= 9'b000011110; dataOut <= 9'b000011110; end
					 31: begin ram[ramAddress] <= 9'b000011111; dataOut <= 9'b000011111; end
					 32: begin ram[ramAddress] <= 9'b000100000; dataOut <= 9'b000100000; end
					 33: begin ram[ramAddress] <= 9'b000100001; dataOut <= 9'b000100001; end
					 34: begin ram[ramAddress] <= 9'b000100010; dataOut <= 9'b000100010; end
					 35: begin ram[ramAddress] <= 9'b000100011; dataOut <= 9'b000100011; end
					 36: begin ram[ramAddress] <= 9'b000100100; dataOut <= 9'b000100100; end
					 37: begin ram[ramAddress] <= 9'b000100101; dataOut <= 9'b000100101; end
					 38: begin ram[ramAddress] <= 9'b000100110; dataOut <= 9'b000100110; end
					 39: begin ram[ramAddress] <= 9'b000100111; dataOut <= 9'b000100111; end
					 40: begin ram[ramAddress] <= 9'b000101000; dataOut <= 9'b000101000; end
					 41: begin ram[ramAddress] <= 9'b000101001; dataOut <= 9'b000101001; end
					 42: begin ram[ramAddress] <= 9'b000101010; dataOut <= 9'b000101010; end
					 43: begin ram[ramAddress] <= 9'b000101011; dataOut <= 9'b000101011; end
					 44: begin ram[ramAddress] <= 9'b000101100; dataOut <= 9'b000101100; end
					 45: begin ram[ramAddress] <= 9'b000101101; dataOut <= 9'b000101101; end
					 46: begin ram[ramAddress] <= 9'b000101110; dataOut <= 9'b000101110; end
					 47: begin ram[ramAddress] <= 9'b000101111; dataOut <= 9'b000101111; end
					 48: begin ram[ramAddress] <= 9'b000110000; dataOut <= 9'b000110000; end
					 49: begin ram[ramAddress] <= 9'b000110001; dataOut <= 9'b000110001; end
				endcase
			end
		end
	end
endmodule


module FUNCMEMORY(input mode, input [6:0] memInput, input CLK, output wire [8:0] result);
	/*Don't edit this module*/
	wire [4:0]  romData;

	FUNCROM RO(memInput[6:3], romData);
	FUNCRAM RA(mode, memInput[6:3], romData, memInput[2],memInput[1:0], CLK, result);

endmodule
