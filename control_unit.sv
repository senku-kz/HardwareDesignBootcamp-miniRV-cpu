module control_unit(
    input logic [31:0] instruction,

    output logic [6:0] opcode,
    output logic [4:0] register_destination,
    output logic [4:0] register_source_1,
    output logic [4:0] register_source_2,
    output logic [11:0] immediate_12bit,
    output logic [2:0] function_3,
    output logic [6:0] function_7
);

    // Extract all fields first (constant slices outside always_comb)
    logic [6:0] instr_opcode;
    logic [4:0] instr_rd;
    logic [4:0] instr_rs1;
    logic [4:0] instr_rs2;
    logic [11:0] instr_imm;
    logic [2:0] instr_funct3;
    logic [6:0] instr_funct7;

    assign instr_opcode = instruction[6:0];
    assign instr_rd = instruction[11:7];
    assign instr_rs1 = instruction[19:15];
    assign instr_rs2 = instruction[24:20];
    assign instr_imm = instruction[31:20];
    assign instr_funct3 = instruction[14:12];
    assign instr_funct7 = instruction[31:25];

    // Determine the type of instruction format (shift right by 2 bits)
    // Extract full opcode from instruction [6:0]
    logic [4:0] type_of_instruction_format;
    
    assign type_of_instruction_format = instruction[6:2];
    
    // Decode based on opcode
    always_comb begin
        // Default values for all outputs (prevents latches)
        opcode = instr_opcode;
        register_destination = 5'b00000;
        register_source_1 = 5'b00000;
        register_source_2 = 5'b00000;
        immediate_12bit = 12'b0000_0000_0000;
        function_3 = 3'b000;
        function_7 = 7'b0000000;
        
        case (type_of_instruction_format)
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
                immediate_12bit = instr_imm;
            end
            5'b00000: begin
                // I-Type (Immediate/Loads/JALR) (LOAD) (LW-type / LB-type)
                register_destination = instr_rd;
                register_source_1 = instr_rs1;
                function_3 = instr_funct3;
                immediate_12bit = instr_imm;
            end
            5'b11001: begin
                // I-Type (Immediate/Loads/JALR) (JALR)
                register_destination = instr_rd;
                register_source_1 = instr_rs1;
                function_3 = instr_funct3;
                immediate_12bit = instr_imm;
            end
            5'b11100: begin
                // I-Type (Immediate/Loads/JALR) (SYSTEM) 
                register_destination = instr_rd;
                register_source_1 = instr_rs1;
                function_3 = instr_funct3;
                immediate_12bit = instr_imm;
            end
            5'b00110: begin
                // I-Type (Immediate/Loads/JALR) (OP-IMM-32)
                register_destination = instr_rd;
                register_source_1 = instr_rs1;
                function_3 = instr_funct3;
                immediate_12bit = instr_imm;
            end

            // S-Type (Store)
            5'b01000: begin
                // S-Type (Store) (STORE)
                register_source_1 = instr_rs1;
                register_source_2 = instr_rs2;
                immediate_12bit = instr_imm;
            end
            // B-Type (Branch)
            5'b11000: begin
                // B-Type (Branch) (BRANCH)
                register_source_1 = instr_rs1;
                register_source_2 = instr_rs2;
                immediate_12bit = instr_imm;
            end
            // U-Type (LUI/AUIPC)
            5'b01101: begin
                // U-Type (LUI) (LUI)
                register_destination = instr_rd;
                immediate_12bit = instr_imm;
            end
            5'b00101: begin
                // U-Type (LUI) (AUIPC)
                register_destination = instr_rd;
                immediate_12bit = instr_imm;
            end
            // J-Type (JAL)
            5'b11011: begin
                // J-Type (JAL) (JAL)
                register_destination = instr_rd;
                immediate_12bit = instr_imm;
            end
            // V-Type (V)
            5'b10101: begin
                // V-Type (V) (OP-V)
                register_destination = instr_rd;
                register_source_1 = instr_rs1;
                register_source_2 = instr_rs2;
                immediate_12bit = instr_imm;
            end
            5'b00001: begin
                // V-Type (V) (LOAD-FP)
                register_destination = instr_rd;
                register_source_1 = instr_rs1;
                immediate_12bit = instr_imm;
            end
            5'b01001: begin
                // V-Type (V) (STORE-FP)
                register_source_1 = instr_rs1;
                register_source_2 = instr_rs2;
            end
            5'b11101: begin
                // V-Type (V) (OP-VE)
                register_destination = instr_rd;
                register_source_1 = instr_rs1;
                register_source_2 = instr_rs2;
                immediate_12bit = instr_imm;
            end
            default: begin
                // Default case
                opcode = 7'b0000000;
                register_destination = 5'b00000;
                register_source_1 = 5'b00000;
                register_source_2 = 5'b00000;
                immediate_12bit = 12'b0000_0000_0000;
                function_3 = 3'b000;
                function_7 = 7'b0000000;
            end
        endcase
    end

endmodule