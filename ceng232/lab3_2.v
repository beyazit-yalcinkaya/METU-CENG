`timescale 1ns / 1ps 
module lab3_2(
			input[4:0] word,
			input CLK, 
			input selection,
			input mode,
			output reg [7:0] hipsterians1,
			output reg [7:0] hipsterians0,
			output reg [7:0] nerdians1,
			output reg [7:0] nerdians0,
			output reg warning
    );

	initial begin
		hipsterians0=0;
		nerdians0=0;
		hipsterians1=0;
		nerdians1=0;
		warning=0;
	end
   //Modify the lines below to implement your design .

	always@(posedge CLK) begin
		if (selection == 0) begin
			if (word[4] == 1'b0 && word[3] == 1'b0
			 || word[3] == 1'b0 && word[2] == 1'b0
		    || word[2] == 1'b0 && word[1] == 1'b0
	       || word[1] == 1'b0 && word[0] == 1'b0) begin
				warning <= 1'b0;
				if (mode == 0) begin
					if (hipsterians1 == 8'd0) begin
						if (hipsterians0 == 8'd0) begin
						end
						else begin
							hipsterians0 <= hipsterians0 - 8'd1;
						end
					end
					else if (hipsterians1 == 8'd1) begin
						if (hipsterians0 == 8'd0) begin
							hipsterians1 <= 8'd0;
							hipsterians0 <= 8'd9;				
						end
						else begin
							hipsterians0 <= hipsterians0 - 8'd1;
						end
					end
					else if (hipsterians1 == 8'd2 && hipsterians0 == 8'd0) begin
						hipsterians1 <= 8'd1;
						hipsterians0 <= 8'd9;						
					end
					else begin
						warning <= 1;
					end
				end
				else begin
					if (hipsterians1 == 8'd2 && hipsterians0 == 8'd0) begin
						hipsterians1 <= 8'd0;
						hipsterians0 <= 8'd0;
					end
					else if (hipsterians1 == 8'd1)begin
						if (hipsterians0 == 8'd9 )begin
							hipsterians1 <= 8'd2;
							hipsterians0 <= 8'd0;
						end
						else begin
							hipsterians0 <= hipsterians0 + 8'd1;   
						end
					end
					else if (hipsterians1 == 8'd0) begin
						if (hipsterians0 == 8'd9) begin
							hipsterians1 <= 8'd1;
							hipsterians0 <= 8'd0;
						end
						else begin
							hipsterians0 <= hipsterians0 + 8'd1;
						end
					end
					else begin
						warning <= 1'b1;
					end
				end
			end
			else begin
				warning <= 1'b1;
			end
		end
		else begin
			if (word[4] == 1'b1 && word[3] == 1'b1
			 || word[3] == 1'b1 && word[2] == 1'b1
		 	 || word[2] == 1'b1 && word[1] == 1'b1
			 || word[1] == 1'b1 && word[0] == 1'b1) begin
				warning <= 1'b0;
				if (mode == 0) begin
					if (nerdians1 == 8'd0) begin
						if (nerdians0 == 8'd0) begin
						end
						else begin
							nerdians0 <= nerdians0 - 8'd1;
						end
					end
					else if (nerdians1 == 8'd1) begin
						if (nerdians0 == 8'd0) begin
							nerdians1 <= 8'd0;
							nerdians0 <= 8'd9;					
						end
						else begin
							nerdians0 <= nerdians0 - 8'd1;
						end
					end
					else if (nerdians1 == 8'd2 && nerdians0 == 8'd0) begin
						nerdians1 <= 8'd1;
						nerdians0 <= 8'd9;						
					end
					else begin
						warning <= 1;
					end
				end
				else begin
					if (nerdians1 == 8'd2 && nerdians0 == 8'd0) begin
						nerdians1 <= 8'd0;
						nerdians0 <= 8'd0;
					end
					else if (nerdians1 == 8'd1)begin
						if (nerdians0 == 8'd9 )begin
							nerdians1 <= 8'd2;
							nerdians0 <= 8'd0;
						end
						else begin
							nerdians0 <= nerdians0 + 8'd1;   
						end
					end
					else if (nerdians1 == 8'd0) begin
						if (nerdians0 == 8'd9) begin
							nerdians1 <= 8'd1;
							nerdians0 <= 8'd0;
						end
						else begin
							nerdians0 <= nerdians0 + 8'd1;
						end
					end
					else begin
						warning <= 1'b1;
					end
				end
			end
			else begin
				warning <= 1'b1;
			end
		end
	end
	
endmodule

