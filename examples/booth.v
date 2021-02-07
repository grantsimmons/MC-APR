module booth #(parameter OPERAND_BITS = 4) (
        input wire clk,
        input wire [OPERAND_BITS - 1:0] mul1,
        input wire [OPERAND_BITS - 1:0] mul2,
        //input wire [4:0] res_in,
        //output wire [1:0] op,
        //output wire [3:0] val1,
        //output wire [3:0] val2,
        output wire [2 * OPERAND_BITS - 1:0] res_out,
        output reg ready,
        output wire [2 * OPERAND_BITS - 1:0] comp
        );
    localparam N_BITS = $clog2(OPERAND_BITS + 1);
    reg [OPERAND_BITS - 1:0] A;
    reg [OPERAND_BITS - 1:0] Q;
    reg q0;
    reg [OPERAND_BITS - 1:0] M;
    reg [OPERAND_BITS - 1:0] mul2_mem;
    reg [N_BITS - 1:0] n;
    //alu a1 (.clk(clk), .op(op), .val1(val1), .val2(val2), .res(res_in));
    //typedef enum reg [1:0] { LNOP, ADD, SUB, HNOP } operation;
    //operation op1 = SUB; //First action always either shift or subtract
    //assign op = op1;
    assign res_out = {A, Q};
    wire [1:0] op;
    assign op = {Q[0], q0};
    wire [2 * OPERAND_BITS:0] test;
    assign test = {A, Q, q0};
    assign comp = ~(res_out) + 1'b1; //Take 2's comp to get true value
    initial begin
        n <= OPERAND_BITS + 1;
    end
    always @(posedge clk) begin
        if (n == OPERAND_BITS + 1) begin
            A <= 0;
            Q <= mul2;
            q0 <= 1'b0;
            M <= mul1; //Initialize M = mul1
            mul2_mem <= mul2;
        end
        if (ready && ((mul1 != M) || (mul2 != mul2_mem))) begin
            ready = 0;
            n = OPERAND_BITS + 1;
            A <= 0;
            Q <= mul2;
            q0 <= 1'b0;
            M <= mul1;
            mul2_mem <= mul2;
        end
        if (n < OPERAND_BITS + 1 && n != 0) begin
            case(op)
                2'b01: A = A - M;
                2'b10: A = A + M;
                default: A = A;
            endcase
            {A, Q, q0} = {A, Q, q0} >> 1;
            A[OPERAND_BITS - 1] <= A[OPERAND_BITS - 2];
        end
        if(n <= 1) begin
            ready <= 1;
            n = 0;
        end
        else begin
            ready <= 0;
            n = n - 1;
        end
    end
endmodule
