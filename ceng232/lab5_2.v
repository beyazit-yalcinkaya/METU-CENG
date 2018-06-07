`timescale 1ns / 1ps

module Elevator(input CLK,
					input [1:0] mode,
					input [3:0] request,
					output reg[3:0] currentFloor1,
					output reg[3:0] currentFloor2,
					output reg doorOpen1, 
					output reg doorOpen2, 
					output reg [3:0] listingLeds,
					output reg listBusy,
					output reg state);

//Write your code below

reg [3:0] taskPool[0:7];
reg [3:0] currentTask1;
reg [3:0] currentTask2;
integer i;//keeps last empty index of taskPool
integer j;//used for listing

initial begin
	currentFloor1[3:0] = 1;
	currentFloor2[3:0] = 1;
	doorOpen1=1;
	doorOpen2=1;
	listingLeds=4'b0000;
	listBusy = 0;
	state = 0;
	taskPool[0] = 0;
   taskPool[1] = 0;
   taskPool[2] = 0;
   taskPool[3] = 0;
   taskPool[4] = 0;
   taskPool[5] = 0;
   taskPool[6] = 0;
   taskPool[7] = 0;
	currentTask1 = 0;
	currentTask2 = 0;
   i = 0;
   j = 0;
end

always@(posedge CLK) begin
	if (state == 0) begin
		if (mode == 0) begin
		//ADD
			if (taskPool[0] != request && taskPool[1] != request &&
				 taskPool[2] != request && taskPool[3] != request &&
				 taskPool[4] != request && taskPool[5] != request &&
				 taskPool[6] != request && taskPool[7] != request && i < 8) begin
				state <= 0;
				listBusy <= 0;
				listingLeds <= 0;
				doorOpen1 <= doorOpen1;
				doorOpen2 <= doorOpen2;
				currentFloor1 <= currentFloor1;
				currentFloor2 <= currentFloor2;
				currentTask1 <= 0;
				currentTask2 <= 0;	
				taskPool[i] <= request;
				i <= i + 1;
				j <= 0;
			end
		end
		else if (mode == 1) begin
		//LIST
			if (i > j) begin
				state <= 0;
				listBusy <= 1;
				listingLeds <= taskPool[j];
				doorOpen1 <= doorOpen1;
				doorOpen2 <= doorOpen2;
				currentFloor1 <= currentFloor1;
				currentFloor2 <= currentFloor2;
				currentTask1 <= 0;
				currentTask2 <= 0;
				i <= i;
				j <= j + 1;
			end
			else begin
				state <= 0;
				listBusy <= 0;
				listingLeds <= 0;
				doorOpen1 <= doorOpen1;
				doorOpen2 <= doorOpen2;
				currentFloor1 <= currentFloor1;
				currentFloor2 <= currentFloor2;
				currentTask1 <= 0;
				currentTask2 <= 0;
				i <= i;
				j <= 0;
			end
		end
		else if (mode == 2) begin
		//DELETE
			if (taskPool[0] == request) begin
				state <= 0;
				listBusy <= 0;
				listingLeds <= 0;
				doorOpen1 <= doorOpen1;
				doorOpen2 <= doorOpen2;
				currentFloor1 <= currentFloor1;
				currentFloor2 <= currentFloor2;	
				currentTask1 <= 0;
				currentTask2 <= 0;			
				taskPool[0] <= taskPool[1];				
				taskPool[1] <= taskPool[2];				
				taskPool[2] <= taskPool[3];				
				taskPool[3] <= taskPool[4];
				taskPool[4] <= taskPool[5];
				taskPool[5] <= taskPool[6];
				taskPool[6] <= taskPool[7];
				taskPool[7] <= 0;
				i <= i - 1;
				j <= 0;
			end
			else if (taskPool[1] == request) begin
				state <= 0;
				listBusy <= 0;
				listingLeds <= 0;
				doorOpen1 <= doorOpen1;
				doorOpen2 <= doorOpen2;
				currentFloor1 <= currentFloor1;
				currentFloor2 <= currentFloor2;	
				currentTask1 <= 0;
				currentTask2 <= 0;			
				taskPool[0] <= taskPool[0];				
				taskPool[1] <= taskPool[2];				
				taskPool[2] <= taskPool[3];				
				taskPool[3] <= taskPool[4];
				taskPool[4] <= taskPool[5];
				taskPool[5] <= taskPool[6];
				taskPool[6] <= taskPool[7];
				taskPool[7] <= 0;
				i <= i - 1;
				j <= 0;
			end
			else if (taskPool[2] == request) begin
				state <= 0;
				listBusy <= 0;
				listingLeds <= 0;
				doorOpen1 <= doorOpen1;
				doorOpen2 <= doorOpen2;
				currentFloor1 <= currentFloor1;
				currentFloor2 <= currentFloor2;
				currentTask1 <= 0;
				currentTask2 <= 0;				
				taskPool[0] <= taskPool[0];				
				taskPool[1] <= taskPool[1];				
				taskPool[2] <= taskPool[3];				
				taskPool[3] <= taskPool[4];
				taskPool[4] <= taskPool[5];
				taskPool[5] <= taskPool[6];
				taskPool[6] <= taskPool[7];
				taskPool[7] <= 0;
				i <= i - 1;
				j <= 0;
			end
			else if (taskPool[3] == request) begin
				state <= 0;
				listBusy <= 0;
				listingLeds <= 0;
				doorOpen1 <= doorOpen1;
				doorOpen2 <= doorOpen2;
				currentFloor1 <= currentFloor1;
				currentFloor2 <= currentFloor2;
				currentTask1 <= 0;
				currentTask2 <= 0;			
				taskPool[0] <= taskPool[0];				
				taskPool[1] <= taskPool[1];				
				taskPool[2] <= taskPool[2];				
				taskPool[3] <= taskPool[4];
				taskPool[4] <= taskPool[5];
				taskPool[5] <= taskPool[6];
				taskPool[6] <= taskPool[7];
				taskPool[7] <= 0;
				i <= i - 1;
				j <= 0;
			end
			else if (taskPool[4] == request) begin
				state <= 0;
				listBusy <= 0;
				listingLeds <= 0;
				doorOpen1 <= doorOpen1;
				doorOpen2 <= doorOpen2;
				currentFloor1 <= currentFloor1;
				currentFloor2 <= currentFloor2;
				currentTask1 <= 0;
				currentTask2 <= 0;			
				taskPool[0] <= taskPool[0];				
				taskPool[1] <= taskPool[1];				
				taskPool[2] <= taskPool[2];				
				taskPool[3] <= taskPool[3];
				taskPool[4] <= taskPool[5];
				taskPool[5] <= taskPool[6];
				taskPool[6] <= taskPool[7];
				taskPool[7] <= 0;
				i <= i - 1;
				j <= 0;
			end
			else if (taskPool[5] == request) begin
				state <= 0;
				listBusy <= 0;
				listingLeds <= 0;
				doorOpen1 <= doorOpen1;
				doorOpen2 <= doorOpen2;
				currentFloor1 <= currentFloor1;
				currentFloor2 <= currentFloor2;
				currentTask1 <= 0;
				currentTask2 <= 0;			
				taskPool[0] <= taskPool[0];				
				taskPool[1] <= taskPool[1];				
				taskPool[2] <= taskPool[2];				
				taskPool[3] <= taskPool[3];
				taskPool[4] <= taskPool[4];
				taskPool[5] <= taskPool[6];
				taskPool[6] <= taskPool[7];
				taskPool[7] <= 0;
				i <= i - 1;
				j <= 0;
			end
			else if (taskPool[6] == request) begin
				state <= 0;
				listBusy <= 0;
				listingLeds <= 0;
				doorOpen1 <= doorOpen1;
				doorOpen2 <= doorOpen2;
				currentFloor1 <= currentFloor1;
				currentFloor2 <= currentFloor2;
				currentTask1 <= 0;
				currentTask2 <= 0;				
				taskPool[0] <= taskPool[0];				
				taskPool[1] <= taskPool[1];				
				taskPool[2] <= taskPool[2];				
				taskPool[3] <= taskPool[3];
				taskPool[4] <= taskPool[4];
				taskPool[5] <= taskPool[5];
				taskPool[6] <= taskPool[7];
				taskPool[7] <= 0;
				i <= i - 1;
				j <= 0;
			end
			else if (taskPool[7] == request) begin
				state <= 0;
				listBusy <= 0;
				listingLeds <= 0;
				doorOpen1 <= doorOpen1;
				doorOpen2 <= doorOpen2;
				currentFloor1 <= currentFloor1;
				currentFloor2 <= currentFloor2;
				currentTask1 <= 0;
				currentTask2 <= 0;			
				taskPool[0] <= taskPool[0];				
				taskPool[1] <= taskPool[1];				
				taskPool[2] <= taskPool[2];				
				taskPool[3] <= taskPool[3];
				taskPool[4] <= taskPool[4];
				taskPool[5] <= taskPool[5];
				taskPool[6] <= taskPool[6];
				taskPool[7] <= 0;
				i <= i - 1;
				j <= 0;
			end
		end
		else if (mode == 3) begin
		//START WORK
			listBusy <= 0;
			listingLeds <= 0;
			currentTask1 <= taskPool[0];
			currentTask2 <= taskPool[1];			
			taskPool[0] <= taskPool[2];				
			taskPool[1] <= taskPool[3];				
			taskPool[2] <= taskPool[4];				
			taskPool[3] <= taskPool[5];
			taskPool[4] <= taskPool[6];
			taskPool[5] <= taskPool[7];
			taskPool[6] <= 0;
			taskPool[7] <= 0;
			j <= 0;
			if (taskPool[0] != 0 && taskPool[1] != 0) begin
				i <= i - 2;
				state <= 1;
				doorOpen1 <= 0;
				doorOpen2 <= 0;
				if (taskPool[0] > currentFloor1) currentFloor1 <= currentFloor1 + 1;
				else if (taskPool[0] < currentFloor1) currentFloor1 <= currentFloor1 - 1;
				if (taskPool[1] > currentFloor2) currentFloor2 <= currentFloor2 + 1;
				else if (taskPool[1] < currentFloor2) currentFloor2 <= currentFloor2 - 1;
				if ((taskPool[0] == currentFloor1 || taskPool[0] == currentFloor1 - 1 ||taskPool[0] == currentFloor1 + 1) &&
					 (taskPool[1] == currentFloor2 || taskPool[1] == currentFloor2 - 1 ||taskPool[1] == currentFloor2 + 1)) begin
					if (i == 2) state <= 0;
					doorOpen1 <= 1;
					doorOpen2 <= 1;
					currentTask1 <= 0;
					currentTask2 <= 0;
				end
				else if (taskPool[0] == currentFloor1 || taskPool[0] == currentFloor1 - 1 ||taskPool[0] == currentFloor1 + 1) begin
					doorOpen1 <= 1;
					currentTask1 <= 0;
				end
				else if (taskPool[1] == currentFloor2 || taskPool[1] == currentFloor2 - 1 ||taskPool[1] == currentFloor2 + 1) begin
					doorOpen2 <= 1;
					currentTask2 <= 0;
				end
			end
			else if (taskPool[0] != 0) begin
				i <= i - 1;
				state <= 1;
				doorOpen1 <= 0;
				doorOpen2 <= 1;
				if (taskPool[0] > currentFloor1) currentFloor1 <= currentFloor1 + 1;
				else if (taskPool[0] < currentFloor1) currentFloor1 <= currentFloor1 - 1;
				if (taskPool[0] == currentFloor1 || taskPool[0] == currentFloor1 - 1 ||taskPool[0] == currentFloor1 + 1) begin
					if (i == 1) state <= 0;
					doorOpen1 <= 1;
					currentTask1 <= 0;
				end
			end
			else begin
				i <= i;
				state <= 0;
				doorOpen1 <= 1;
				doorOpen2 <= 1;
			end
			if (i > 2) state <= 1;
		end
	end
	else if (state == 1) begin
		listBusy <= 0;
		listingLeds <= 0;
		if (currentTask1 == 0 && currentTask2 == 0) begin
			currentTask1 <= taskPool[0];
			currentTask2 <= taskPool[1];			
			taskPool[0] <= taskPool[2];				
			taskPool[1] <= taskPool[3];				
			taskPool[2] <= taskPool[4];				
			taskPool[3] <= taskPool[5];
			taskPool[4] <= taskPool[6];
			taskPool[5] <= taskPool[7];
			taskPool[6] <= 0;
			taskPool[7] <= 0;
			j <= 0;
			if (taskPool[0] != 0 && taskPool[1] != 0) begin
				i <= i - 2;
				state <= 1;
				doorOpen1 <= 0;
				doorOpen2 <= 0;
				if (taskPool[0] > currentFloor1) currentFloor1 <= currentFloor1 + 1;
				else if (taskPool[0] < currentFloor1) currentFloor1 <= currentFloor1 - 1;
				if (taskPool[1] > currentFloor2) currentFloor2 <= currentFloor2 + 1;
				else if (taskPool[1] < currentFloor2) currentFloor2 <= currentFloor2 - 1;
				if ((taskPool[0] == currentFloor1 || taskPool[0] == currentFloor1 - 1 ||taskPool[0] == currentFloor1 + 1) &&
					 (taskPool[1] == currentFloor2 || taskPool[1] == currentFloor2 - 1 ||taskPool[1] == currentFloor2 + 1)) begin
					if (i == 2) state <= 0;
					doorOpen1 <= 1;
					doorOpen2 <= 1;
					currentTask1 <= 0;
					currentTask2 <= 0;
				end
				else if (taskPool[0] == currentFloor1 || taskPool[0] == currentFloor1 - 1 ||taskPool[0] == currentFloor1 + 1) begin
					doorOpen1 <= 1;
					currentTask1 <= 0;
				end
				else if (taskPool[1] == currentFloor2 || taskPool[1] == currentFloor2 - 1 ||taskPool[1] == currentFloor2 + 1) begin
					doorOpen2 <= 1;
					currentTask2 <= 0;
				end
			end
			else if (taskPool[0] != 0) begin
				i <= i - 1;
				state <= 1;
				doorOpen1 <= 0;
				doorOpen2 <= 1;
				if (taskPool[0] > currentFloor1) currentFloor1 <= currentFloor1 + 1;
				else if (taskPool[0] < currentFloor1) currentFloor1 <= currentFloor1 - 1;
				if (taskPool[0] == currentFloor1 || taskPool[0] == currentFloor1 - 1 ||taskPool[0] == currentFloor1 + 1) begin
					if (i == 1) state <= 0;
					doorOpen1 <= 1;
					currentTask1 <= 0;
				end
			end
			else begin
				i <= i;
				state <= 0;
				doorOpen1 <= 1;
				doorOpen2 <= 1;
			end
			if (i > 2) state <= 1;
		end
		else if (currentTask1 == 0) begin
			currentTask1 <= taskPool[0];		
			taskPool[0] <= taskPool[1];				
			taskPool[1] <= taskPool[2];				
			taskPool[2] <= taskPool[3];				
			taskPool[3] <= taskPool[4];
			taskPool[4] <= taskPool[5];
			taskPool[5] <= taskPool[6];
			taskPool[6] <= taskPool[7];
			taskPool[7] <= 0;
			j <= 0;
			if (currentTask2 > currentFloor2) currentFloor2 <= currentFloor2 + 1;
			else if (currentTask2 < currentFloor2) currentFloor2 <= currentFloor2 - 1;
			if (taskPool[0] != 0) begin
				i <= i - 1;
				state <= 1;
				doorOpen1 <= 0;
				if (taskPool[0] > currentFloor1) currentFloor1 <= currentFloor1 + 1;
				else if (taskPool[0] < currentFloor1) currentFloor1 <= currentFloor1 - 1;	
				if ((taskPool[0] == currentFloor1 || taskPool[0] == currentFloor1 - 1 ||taskPool[0] == currentFloor1 + 1) &&
					 (currentTask2 == currentFloor2 || currentTask2 == currentFloor2 - 1 || currentTask2 == currentFloor2 + 1)) begin
					if (i == 1) state <= 0;
					doorOpen1 <= 1;
					doorOpen2 <= 1;
					currentTask1 <= 0;
					currentTask2 <= 0;
				end
				else if (taskPool[0] == currentFloor1 || taskPool[0] == currentFloor1 - 1 ||taskPool[0] == currentFloor1 + 1) begin
					doorOpen1 <= 1;
					currentTask1 <= 0;
				end
				else if (currentTask2 == currentFloor2 || currentTask2 == currentFloor2 - 1 || currentTask2 == currentFloor2 + 1) begin
					doorOpen2 <= 1;
					currentTask2 <= 0;
				end
			end
			else begin
				state <= 1;
				doorOpen1 <= 1;
				currentTask1 <= 0;
				if (currentTask2 == currentFloor2 || currentTask2 == currentFloor2 - 1 || currentTask2 == currentFloor2 + 1) begin
					state <= 0;
					doorOpen2 <= 1;
					currentTask2 <= 0;
				end
			end
			if (i > 1) state <= 1;
		end
		else if (currentTask2 == 0) begin
			currentTask2 <= taskPool[0];		
			taskPool[0] <= taskPool[1];				
			taskPool[1] <= taskPool[2];				
			taskPool[2] <= taskPool[3];				
			taskPool[3] <= taskPool[4];
			taskPool[4] <= taskPool[5];
			taskPool[5] <= taskPool[6];
			taskPool[6] <= taskPool[7];
			taskPool[7] <= 0;
			j <= 0;
			if (currentTask1 > currentFloor1) currentFloor1 <= currentFloor1 + 1;
			else if (currentTask1 < currentFloor1) currentFloor1 <= currentFloor1 - 1;
			if (taskPool[0] != 0) begin
				i <= i - 1;
				state <= 1;
				doorOpen2 <= 0;
				if (taskPool[0] > currentFloor2) currentFloor2 <= currentFloor2 + 1;
				else if (taskPool[0] < currentFloor2) currentFloor2 <= currentFloor2 - 1;	
				if ((taskPool[0] == currentFloor2 || taskPool[0] == currentFloor2 - 1 ||taskPool[0] == currentFloor2 + 1) &&
					 (currentTask1 == currentFloor1 || currentTask1 == currentFloor1 - 1 || currentTask1 == currentFloor1 + 1)) begin
					if (i == 1) state <= 0;
					doorOpen1 <= 1;
					doorOpen2 <= 1;
					currentTask1 <= 0;
					currentTask2 <= 0;
				end
				else if (taskPool[0] == currentFloor2 || taskPool[0] == currentFloor2 - 1 ||taskPool[0] == currentFloor2 + 1) begin
					doorOpen2 <= 1;
					currentTask2 <= 0;
				end
				else if (currentTask1 == currentFloor1 || currentTask1 == currentFloor1 - 1 || currentTask1 == currentFloor1 + 1) begin
					doorOpen1 <= 1;
					currentTask1 <= 0;
				end
			end
			else begin
				state <= 1;
				doorOpen2 <= 1;
				currentTask2 <= 0;
				if (currentTask1 == currentFloor1 || currentTask1 == currentFloor1 - 1 || currentTask1 == currentFloor1 + 1) begin
					state <= 0;
					doorOpen1 <= 1;
					currentTask1 <= 0;
				end
			end
			if (i > 1) state <= 1;
		end
		else begin
			state <= 1;
			j <= 0;
			if (currentTask1 > currentFloor1) currentFloor1 <= currentFloor1 + 1;
			else if (currentTask1 < currentFloor1) currentFloor1 <= currentFloor1 - 1;
			if (currentTask2 > currentFloor2) currentFloor2 <= currentFloor2 + 1;
			else if (currentTask2 < currentFloor2) currentFloor2 <= currentFloor2 - 1;
			if ((currentTask1 == currentFloor1 || currentTask1 == currentFloor1 - 1 || currentTask1 == currentFloor1 + 1) &&
				 (currentTask2 == currentFloor2 || currentTask2 == currentFloor2 - 1 || currentTask2 == currentFloor2 + 1)) begin
				if (i == 0) state <= 0;
				doorOpen1 <= 1;
				doorOpen2 <= 1;
				currentTask1 <= 0;
				currentTask2 <= 0;
			end
			else if (currentTask1 == currentFloor1 || currentTask1 == currentFloor1 - 1 || currentTask1 == currentFloor1 + 1) begin
				doorOpen1 <= 1;
				currentTask1 <= 0;
			end
			else if (currentTask2 == currentFloor2 || currentTask2 == currentFloor2 - 1 ||currentTask2 == currentFloor2 + 1) begin
				doorOpen2 <= 1;
				currentTask2 <= 0;
			end
			if (i > 0) state <= 1;
		end
	end
end

endmodule
