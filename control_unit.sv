module control_unit(
    input logic [31:0] instruction,

    output logic [6:0] opcode,
    output logic [4:0] register_destination,
    output logic [4:0] register_source_1,
    output logic [4:0] register_source_2,
    output logic [2:0] function_3,
    output logic [6:0] function_7,
    output logic [6:0] immediate_7bit,
    output logic [11:0] immediate_12bit,
    output logic [19:0] immediate_20bit,
    output logic [31:0] immediate_32bit
);

    // Extract all fields first (constant slices outside always_comb)
    logic [6:0] instr_opcode;
    logic [4:0] instr_opcode_5bit;
    logic [4:0] instr_rd;
    logic [4:0] instr_rs1;
    logic [4:0] instr_rs2;
    logic [6:0] instr_imm_7bit;
    logic [11:0] instr_imm_12bit;
    logic [19:0] instr_imm_20bit;
    logic [31:0] instr_imm_32bit;
    logic [2:0] instr_funct3;
    logic [6:0] instr_funct7;
    logic [31:0] instr_imm_i;
    logic [31:0] instr_imm_s;
    logic [31:0] instr_imm_u;


    assign instr_opcode = instruction[6:0];
    assign instr_opcode_5bit = instruction[6:2];
    assign instr_rd = instruction[11:7];
    assign instr_rs1 = instruction[19:15];
    assign instr_rs2 = instruction[24:20];

    assign instr_funct3 = instruction[14:12];
    assign instr_funct7 = instruction[31:25];

    assign instr_imm_7bit = instruction[31:25];
    assign instr_imm_12bit = instruction[31:20];
    assign instr_imm_20bit = instruction[31:12];

    // I-type imm[11:0] sign-extended
    assign instr_imm_i = {{20{instruction[31]}}, instruction[31:20]};

    // S-type imm[11:5|4:0] sign-extended
    assign instr_imm_s = {{20{instruction[31]}}, instruction[31:25], instruction[11:7]};

    // U-type imm[31:12] << 12
    assign instr_imm_u = {instruction[31],instruction[30:12], 12'b0};



    // Decode based on opcode
    always_comb begin
        // Default values for all outputs (prevents latches)
        opcode = instr_opcode;
        register_destination = 5'b00000;
        register_source_1 = 5'b00000;
        register_source_2 = 5'b00000;
        function_3 = 3'b000;
        function_7 = 7'b0000000;
        immediate_7bit = 7'b0000000;
        immediate_12bit = 12'b0000_0000_0000;
        immediate_20bit = 20'b0000_0000_0000_0000_0000;
        immediate_32bit = 32'b0000_0000_0000_0000_0000_0000_0000_0000;

        
        case (instr_opcode_5bit)
            // R-Type (Register-Register)
            5'b01100: begin
                // R-Type (Register-Register) (OP)
                register_destination = instr_rd;
                register_source_1 = instr_rs1;
                register_source_2 = instr_rs2;
                function_3 = instr_funct3;
                function_7 = instr_funct7;
            end
            5'b01110: begin
                // R-Type (Register-Register) (OP-32)
                register_destination = instr_rd;
                register_source_1 = instr_rs1;
                register_source_2 = instr_rs2;
                function_3 = instr_funct3;
                function_7 = instr_funct7;
            end

            // I-Type (Immediate/Loads/JALR)
            5'b00100: begin
                // I-Type (Immediate/Loads/JALR) (OP-IMM) (ADD-type / ADDI-type)
                register_destination = instr_rd;
                register_source_1 = instr_rs1;
                function_3 = instr_funct3;
                immediate_12bit = instr_imm_12bit;
                immediate_32bit = instr_imm_i;
            end
            5'b00000: begin
                // I-Type (Immediate/Loads/JALR) (LOAD) (LW-type / LB-type)
                register_destination = instr_rd;
                register_source_1 = instr_rs1;
                function_3 = instr_funct3;
                immediate_12bit = instr_imm_12bit;
                immediate_32bit = instr_imm_i;
            end
            5'b11001: begin
                // I-Type (Immediate/Loads/JALR) (JALR)
                register_destination = instr_rd;
                register_source_1 = instr_rs1;
                function_3 = instr_funct3;
                immediate_12bit = instr_imm_12bit;
                immediate_32bit = instr_imm_i;
            end
            5'b11100: begin
                // I-Type (Immediate/Loads/JALR) (SYSTEM) 
                register_destination = instr_rd;
                register_source_1 = instr_rs1;
                function_3 = instr_funct3;
                immediate_12bit = instr_imm_12bit;
                immediate_32bit = instr_imm_i;
            end
            5'b00110: begin
                // I-Type (Immediate/Loads/JALR) (OP-IMM-32)
                register_destination = instr_rd;
                register_source_1 = instr_rs1;
                function_3 = instr_funct3;
                immediate_12bit = instr_imm_12bit;
                immediate_32bit = instr_imm_i;
            end

            // S-Type (Store)
            5'b01000: begin
                // S-Type (Store) (STORE)
                register_destination = instr_rd;
                function_3 = instr_funct3;
                register_source_1 = instr_rs1;
                register_source_2 = instr_rs2;
                immediate_7bit = instr_imm_7bit;
                immediate_32bit = instr_imm_s;
            end

            // B-Type (Branch)
            5'b11000: begin
                // B-Type (Branch) (BRANCH)
                register_source_1 = instr_rs1;
                register_source_2 = instr_rs2;
                immediate_7bit = instr_imm_7bit;
                immediate_32bit = instr_imm_s;
            end

            // U-Type (LUI/AUIPC)
            5'b01101: begin
                // U-Type (LUI) (LUI)
                register_destination = instr_rd;
                immediate_20bit = instr_imm_20bit;
                immediate_32bit = instr_imm_u;
            end
            5'b00101: begin
                // U-Type (LUI) (AUIPC)
                register_destination = instr_rd;
                immediate_20bit = instr_imm_20bit;
                immediate_32bit = instr_imm_u;
            end

            // J-Type (JAL)
            5'b11011: begin
                // J-Type (JAL) (JAL)
                register_destination = instr_rd;
                immediate_20bit = instr_imm_20bit;
                immediate_32bit = instr_imm_u;
            end

            default: begin
                // Default case
                opcode = 7'b0000000;
                register_destination = 5'b00000;
                register_source_1 = 5'b00000;
                register_source_2 = 5'b00000;
                immediate_7bit = 7'b0000000;
                immediate_12bit = 12'b0000_0000_0000;
                immediate_20bit = 20'b0000_0000_0000_0000_0000;
                immediate_32bit = 32'b0000_0000_0000_0000_0000_0000_0000_0000;
                function_3 = 3'b000;
                function_7 = 7'b0000000;
            end
        endcase
    end

endmodule