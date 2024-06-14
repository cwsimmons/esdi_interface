
/* 

    Copyright 2019,2024 Christopher Simmons

  This program is free software: you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the Free
  Software Foundation, either version 2 of the License, or (at your option)
  any later version.

  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along
  with this program. If not, see <https://www.gnu.org/licenses/>.

*/

`timescale 1ns / 1ps

module fifo #(
    parameter DATA_WIDTH = 8,
    parameter DEPTH_EXP = 8,
    parameter WRITE_WHEN_FULL = 1
) (
    input clk,
    input resetn,

    input in_tvalid,
    output in_tready,
    input [DATA_WIDTH-1:0] in_tdata,
    input in_tlast,

    output out_tvalid,
    input out_tready,
    output [DATA_WIDTH-1:0] out_tdata,
    output out_tlast,

    output [DEPTH_EXP-1:0] num_free,
    output [DEPTH_EXP-1:0] num_used
);
    
    reg [DATA_WIDTH - 1:0] mem [0: 2**DEPTH_EXP - 1];
    reg tlast [0:2**DEPTH_EXP - 1];
    reg [DEPTH_EXP - 1:0] top;
    reg [DEPTH_EXP - 1:0] bottom;
    
    assign num_used = top - bottom;
    assign num_free = 2**DEPTH_EXP - num_used - 1;

    assign in_tready = num_free > 0 || WRITE_WHEN_FULL;
    assign out_tvalid = num_used > 0;

    assign out_tdata = mem[bottom];
    assign out_tlast = tlast[bottom];
    
    always @(posedge clk)
    begin
        
        if (!resetn)
        begin
            top <= 0;
            bottom <= 0;
        end
        else
        begin
            
            if (in_tvalid && in_tready)
            begin
               
                mem[top] <= in_tdata;
                tlast[top] <= in_tlast;
                top <= top + 1;

                if (!num_free)
                    bottom <= bottom + 1;
            end

            if (out_tvalid && out_tready)
            begin
                bottom <= bottom + 1;
            end
            
        end
        
    end
    
endmodule