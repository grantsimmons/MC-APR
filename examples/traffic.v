module trafficfsm (
        input wire clk,
        input wire reset,
        input wire Ta,
        input wire Tb,
        output reg [1:0] La, //Light outputs
        output reg [1:0] Lb
        );
    typedef enum reg [1:0] { R, Y, G } Color;
    Color colorA, colorB;
    typedef enum reg [1:0] { A, B, C, D } State;
    State currentState, nextState;

    always @(posedge clk) begin
        if (~reset)
            currentState <= A;
        else
            currentState <= nextState;
    end

    always @(*) begin
        case(currentState) //Define nextstate for each currentstate
            A:  if(Tb)  nextState = B;
                else    nextState = A;
            B:  nextState = C;
            C:  if(Ta)  nextState = D;
                else    nextState = C;
            D:  nextState = A;
            default:    nextState = A;
        endcase

        case(currentState) //Define light colors at each state
            A:  begin 
                    La = G;
                    Lb = R;
            end
            B:  begin
                    La = Y; 
                    Lb = R;
            end
            C:  begin
                    La = R; 
                    Lb = G;
            end
            D:  begin
                    La = R; 
                    Lb = Y;
            end
            default: begin
                    La = G; 
                    Lb = R;
            end
        endcase
    end
endmodule


