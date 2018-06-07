`timescale 1ns / 1ps

module lab4_2(
	input[3:0] userID, 
	input CLK, 
	input team,
	input [1:0] mode,
	output reg  [7:0] numPlyLP,
	output reg  [7:0] numPlyCF,
	output reg  teamWng,
	output reg capWngLP,
	output reg capWngCF,
	output reg findRes,
	output reg [3:0] listOut,
	output reg  listMode
    );
//Write your code below

	reg [3:0] LogicPowerRam[0:4];
	reg [3:0] CengForceRam[0:4];
	integer counterLP;
	integer counterCF;
		
	initial begin
		numPlyLP = 8'd0;
		numPlyCF = 8'd0;
		teamWng = 1'b0;
		capWngLP = 1'b1;
		capWngCF = 1'b1;
		findRes = 1'b0;
		//listOut = 4'd0000;
		listMode = 1'b0;
		LogicPowerRam[0] = 4'b0000;
		LogicPowerRam[1] = 4'b0000;
		LogicPowerRam[2] = 4'b0000;
		LogicPowerRam[3] = 4'b0000;
		LogicPowerRam[4] = 4'b0000;
		CengForceRam[0] = 4'b0000;
		CengForceRam[1] = 4'b0000;
		CengForceRam[2] = 4'b0000;
		CengForceRam[3] = 4'b0000;
		CengForceRam[4] = 4'b0000;
		counterLP = 0;
		counterCF = 0;
	end
	
	always@(posedge CLK) begin
		if (mode == 2'b00) begin
			// Logout Mode
			counterLP <= 0;
			counterCF <= 0;
			if (team != userID[3] || userID == 4'b0000) begin
				numPlyLP <= numPlyLP;
				numPlyCF <= numPlyCF;
				teamWng <= 1'b0;
				capWngLP <= capWngLP;//since <= is concurent
				capWngCF <= capWngCF;
				findRes <= 1'b0;
				//listOut = 4'd0000;
				listMode <= 1'b0;
			end
			else begin
				case (userID)
					LogicPowerRam[0]: begin
						LogicPowerRam[0] <= 4'b0000;
						numPlyLP <= numPlyLP - 1;
						numPlyCF <= numPlyCF;
						teamWng <= 1'b0;
						capWngLP <= ((numPlyLP - 1 == 0) ? 1'b1 : 1'b0);//since <= is concurent
						capWngCF <= capWngCF;
						findRes <= 1'b0;
						//listOut = 4'd0000;
						listMode <= 1'b0;
					end
					LogicPowerRam[1]: begin
						LogicPowerRam[1] <= 4'b0000;
						numPlyLP <= numPlyLP - 1;
						numPlyCF <= numPlyCF;
						teamWng <= 1'b0;
						capWngLP <= ((numPlyLP - 1 == 0) ? 1'b1 : 1'b0);//since <= is concurent
						capWngCF <= capWngCF;
						findRes <= 1'b0;
						//listOut = 4'd0000;
						listMode <= 1'b0;				
					end
					LogicPowerRam[2]: begin
						LogicPowerRam[2] <= 4'b0000;
						numPlyLP <= numPlyLP - 1;
						numPlyCF <= numPlyCF;
						teamWng <= 1'b0;
						capWngLP <= ((numPlyLP - 1 == 0) ? 1'b1 : 1'b0);//since <= is concurent
						capWngCF <= capWngCF;
						findRes <= 1'b0;
						//listOut = 4'd0000;
						listMode <= 1'b0;				
					end
					LogicPowerRam[3]: begin
						LogicPowerRam[3] <= 4'b0000;
						numPlyLP <= numPlyLP - 1;
						numPlyCF <= numPlyCF;
						teamWng <= 1'b0;
						capWngLP <= ((numPlyLP - 1 == 0) ? 1'b1 : 1'b0);//since <= is concurent
						capWngCF <= capWngCF;
						findRes <= 1'b0;
						//listOut = 4'd0000;
						listMode <= 1'b0;
					end
					LogicPowerRam[4]: begin
						LogicPowerRam[4] <= 4'b0000;
						numPlyLP <= numPlyLP - 1;
						numPlyCF <= numPlyCF;
						teamWng <= 1'b0;
						capWngLP <= ((numPlyLP - 1 == 0) ? 1'b1 : 1'b0);//since <= is concurent
						capWngCF <= capWngCF;
						findRes <= 1'b0;
						//listOut = 4'd0000;
						listMode <= 1'b0;				
					end
					CengForceRam[0]: begin
						CengForceRam[0] <= 4'b0000;
						numPlyLP <= numPlyLP;
						numPlyCF <= numPlyCF - 1;
						teamWng <= 1'b0;
						capWngLP <= capWngLP;
						capWngCF <= ((numPlyCF - 1 == 0) ? 1'b1 : 1'b0);//since <= is concurent
						findRes <= 1'b0;
						//listOut = 4'd0000;
						listMode <= 1'b0;
					end
					CengForceRam[1]: begin
						CengForceRam[1] <= 4'b0000;
						numPlyLP <= numPlyLP;
						numPlyCF <= numPlyCF - 1;
						teamWng <= 1'b0;
						capWngLP <= capWngLP;
						capWngCF <= ((numPlyCF - 1 == 0) ? 1'b1 : 1'b0);//since <= is concurent
						findRes <= 1'b0;
						//listOut = 4'd0000;
						listMode <= 1'b0;
					end
					CengForceRam[2]: begin
						CengForceRam[2] <= 4'b0000;
						numPlyLP <= numPlyLP;
						numPlyCF <= numPlyCF - 1;
						teamWng <= 1'b0;
						capWngLP <= capWngLP;
						capWngCF <= ((numPlyCF - 1 == 0) ? 1'b1 : 1'b0);//since <= is concurent
						findRes <= 1'b0;
						//listOut = 4'd0000;
						listMode <= 1'b0;
					end
					CengForceRam[3]: begin
						CengForceRam[3] <= 4'b0000;
						numPlyLP <= numPlyLP;
						numPlyCF <= numPlyCF - 1;
						teamWng <= 1'b0;
						capWngLP <= capWngLP;
						capWngCF <= ((numPlyCF - 1 == 0) ? 1'b1 : 1'b0);//since <= is concurent
						findRes <= 1'b0;
						//listOut = 4'd0000;
						listMode <= 1'b0;
					end
					CengForceRam[4]: begin
						CengForceRam[4] <= 4'b0000;
						numPlyLP <= numPlyLP;
						numPlyCF <= numPlyCF - 1;
						teamWng <= 1'b0;
						capWngLP <= capWngLP;
						capWngCF <= ((numPlyCF - 1 == 0) ? 1'b1 : 1'b0);//since <= is concurent
						findRes <= 1'b0;
						//listOut = 4'd0000;
						listMode <= 1'b0;
					end
				endcase
			end
		end
		else if (mode == 2'b01) begin
			// Login Mode
			counterLP <= 0;
			counterCF <= 0;
			if (userID == 4'b0000) begin
				numPlyLP <= numPlyLP;
				numPlyCF <= numPlyCF;
				teamWng <= 1'b0;
				capWngLP <= capWngLP;
				capWngCF <= capWngCF;
				findRes <= 1'b0;
				//listOut = 4'd0000;
				listMode <= 1'b0;
			end
			else if (team != userID[3]) begin
				numPlyLP <= numPlyLP;
				numPlyCF <= numPlyCF;
				teamWng <= 1'b1;
				capWngLP <= capWngLP;
				capWngCF <= capWngCF;
				findRes <= 1'b0;
				//listOut = 4'd0000;
				listMode <= 1'b0;
			end
			else begin
				if (team == 1'b0) begin
					if (numPlyLP >= 8'd5) begin
							numPlyLP <= numPlyLP;
							numPlyCF <= numPlyCF;
							teamWng <= 1'b0;
							capWngLP <= 1'b1;
							capWngCF <= capWngCF;
							findRes <= 1'b0;
							//listOut = 4'd0000;
							listMode <= 1'b0;			
					end
					else begin
						if (LogicPowerRam[0] == userID
						 || LogicPowerRam[1] == userID
						 || LogicPowerRam[2] == userID
						 || LogicPowerRam[3] == userID
						 || LogicPowerRam[4] == userID) begin
							numPlyLP <= numPlyLP;
							numPlyCF <= numPlyCF;
							teamWng <= 1'b0;
							capWngLP <= capWngLP;
							capWngCF <= capWngCF;
							findRes <= 1'b0;
							//listOut = 4'd0000;
							listMode <= 1'b0;					 
						end
						else begin
							if (LogicPowerRam[0] == 4'b0000) begin
								LogicPowerRam[0] <= userID;
								numPlyLP <= numPlyLP + 1;
								numPlyCF <= numPlyCF;
								teamWng <= 1'b0;
								capWngLP <= ((numPlyLP + 1 == 5) ? 1'b1 : 1'b0);
								capWngCF <= capWngCF;
								findRes <= 1'b0;
								//listOut = 4'd0000;
								listMode <= 1'b0;
							end
							else if (LogicPowerRam[1] == 4'b0000) begin
								LogicPowerRam[1] <= userID;
								numPlyLP <= numPlyLP + 1;
								numPlyCF <= numPlyCF;
								teamWng <= 1'b0;
								capWngLP <= ((numPlyLP + 1 == 5) ? 1'b1 : 1'b0);
								capWngCF <= capWngCF;
								findRes <= 1'b0;
								//listOut = 4'd0000;
								listMode <= 1'b0;								
							end
							else if (LogicPowerRam[2] == 4'b0000) begin
								LogicPowerRam[2] <= userID;
								numPlyLP <= numPlyLP + 1;
								numPlyCF <= numPlyCF;
								teamWng <= 1'b0;
								capWngLP <= ((numPlyLP + 1 == 5) ? 1'b1 : 1'b0);
								capWngCF <= capWngCF;
								findRes <= 1'b0;
								//listOut = 4'd0000;
								listMode <= 1'b0;
							end
							else if (LogicPowerRam[3] == 4'b0000) begin
								LogicPowerRam[3] <= userID;
								numPlyLP <= numPlyLP + 1;
								numPlyCF <= numPlyCF;
								teamWng <= 1'b0;
								capWngLP <= ((numPlyLP + 1 == 5) ? 1'b1 : 1'b0);
								capWngCF <= capWngCF;
								findRes <= 1'b0;
								//listOut = 4'd0000;
								listMode <= 1'b0;
							end
							else begin
								LogicPowerRam[4] <= userID;
								numPlyLP <= numPlyLP + 1;
								numPlyCF <= numPlyCF;
								teamWng <= 1'b0;
								capWngLP <= ((numPlyLP + 1 == 5) ? 1'b1 : 1'b0);
								capWngCF <= capWngCF;
								findRes <= 1'b0;
								//listOut = 4'd0000;
								listMode <= 1'b0;
							end
						end
					end	
				end
				else begin
					if (numPlyCF >= 8'd5) begin
							numPlyLP <= numPlyLP;
							numPlyCF <= numPlyCF;
							teamWng <= 1'b0;
							capWngLP <= capWngLP;
							capWngCF <= 1'b1;
							findRes <= 1'b0;
							//listOut = 4'd0000;
							listMode <= 1'b0;			
					end
					else begin
						if (CengForceRam[0] == userID
						 || CengForceRam[1] == userID
						 || CengForceRam[2] == userID
						 || CengForceRam[3] == userID
						 || CengForceRam[4] == userID) begin
							numPlyLP <= numPlyLP;
							numPlyCF <= numPlyCF;
							teamWng <= 1'b0;
							capWngLP <= capWngLP;
							capWngCF <= capWngCF;
							findRes <= 1'b0;
							//listOut = 4'd0000;
							listMode <= 1'b0;					 
						end
						else begin
							if (CengForceRam[0] == 4'b0000) begin
								CengForceRam[0] <= userID;
								numPlyLP <= numPlyLP;
								numPlyCF <= numPlyCF + 1;
								teamWng <= 1'b0;
								capWngLP <= capWngLP;
								capWngCF <= ((numPlyCF + 1 == 5) ? 1'b1 : 1'b0);
								findRes <= 1'b0;
								//listOut = 4'd0000;
								listMode <= 1'b0;
							end
							else if (CengForceRam[1] == 4'b0000) begin
								CengForceRam[1] <= userID;
								numPlyLP <= numPlyLP;
								numPlyCF <= numPlyCF + 1;
								teamWng <= 1'b0;
								capWngLP <= capWngLP;
								capWngCF <= ((numPlyCF + 1 == 5) ? 1'b1 : 1'b0);
								findRes <= 1'b0;
								//listOut = 4'd0000;
								listMode <= 1'b0;							
							end
							else if (CengForceRam[2] == 4'b0000) begin
								CengForceRam[2] <= userID;
								numPlyLP <= numPlyLP;
								numPlyCF <= numPlyCF + 1;
								teamWng <= 1'b0;
								capWngLP <= capWngLP;
								capWngCF <= ((numPlyCF + 1 == 5) ? 1'b1 : 1'b0);
								findRes <= 1'b0;
								//listOut = 4'd0000;
								listMode <= 1'b0;
							end
							else if (CengForceRam[3] == 4'b0000) begin
								CengForceRam[3] <= userID;
								numPlyLP <= numPlyLP;
								numPlyCF <= numPlyCF + 1;
								teamWng <= 1'b0;
								capWngLP <= capWngLP;
								capWngCF <= ((numPlyCF + 1 == 5) ? 1'b1 : 1'b0);
								findRes <= 1'b0;
								//listOut = 4'd0000;
								listMode <= 1'b0;
							end
							else begin
								CengForceRam[4] <= userID;
								numPlyLP <= numPlyLP;
								numPlyCF <= numPlyCF + 1;
								teamWng <= 1'b0;
								capWngLP <= capWngLP;
								capWngCF <= ((numPlyCF + 1 == 5) ? 1'b1 : 1'b0);
								findRes <= 1'b0;
								//listOut = 4'd0000;
								listMode <= 1'b0;
							end
						end
					end				
				end
			end
		end
		else if (mode == 2'b10) begin
			// Find Mode
			counterLP <= 0;
			counterCF <= 0;
			if (userID == LogicPowerRam[0] || userID == CengForceRam[0]
			 || userID == LogicPowerRam[1] || userID == CengForceRam[1]
			 || userID == LogicPowerRam[2] || userID == CengForceRam[2]
			 || userID == LogicPowerRam[3] || userID == CengForceRam[3]
			 || userID == LogicPowerRam[4] || userID == CengForceRam[4]) begin
				numPlyLP <= numPlyLP;
				numPlyCF <= numPlyCF;
				teamWng <= 1'b0;
				capWngLP <= capWngLP;
				capWngCF <= capWngCF;
				findRes <= 1'b1;
				//listOut = 4'd0000;
				listMode <= 1'b0;
			end
			else begin
				numPlyLP <= numPlyLP;
				numPlyCF <= numPlyCF;
				teamWng <= 1'b0;
				capWngLP <= capWngLP;
				capWngCF <= capWngCF;
				findRes <= 1'b0;
				//listOut = 4'd0000;
				listMode <= 1'b0;
			end
		end
		else begin
			// List Mode
			if (team == 1'b0) begin
				numPlyLP <= numPlyLP;
				numPlyCF <= numPlyCF;
				teamWng <= 1'b0;
				capWngLP <= capWngLP;
				capWngCF <= capWngCF;
				findRes <= 1'b0;
				listOut <= LogicPowerRam[counterLP];
				listMode <= 1'b1;
				counterLP <= ((counterLP == 4) ? 0 : (counterLP + 1));
			end
			else begin
				numPlyLP <= numPlyLP;
				numPlyCF <= numPlyCF;
				teamWng <= 1'b0;
				capWngLP <= capWngLP;
				capWngCF <= capWngCF;
				findRes <= 1'b0;
				listOut <= CengForceRam[counterCF];
				listMode <= 1'b1;
				counterCF <= ((counterCF == 4) ? 0 : (counterCF + 1));
			end
		end
	end
endmodule
