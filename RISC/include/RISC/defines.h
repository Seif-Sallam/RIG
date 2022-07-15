#pragma once
#define OPCODE_Branch 0b1100011
#define OPCODE_Load 0b0000011
#define OPCODE_Store 0b0100011
#define OPCODE_JALR 0b1100111
#define OPCODE_JAL 0b1101111
#define OPCODE_Arith_I 0b0010011
#define OPCODE_Arith_R 0b0110011
#define OPCODE_AUIPC 0b0010111
#define OPCODE_LUI 0b0110111
#define OPCODE_ENV 0b1110011

#define F3_ADD 0b000
#define F3_SLL 0b001
#define F3_SLT 0b010
#define F3_SLTU 0b011
#define F3_XOR 0b100
#define F3_SRL 0b101
#define F3_OR 0b110
#define F3_AND 0b111

#define BR_BEQ 0b000
#define BR_BNE 0b001
#define BR_BLT 0b100
#define BR_BGE 0b101
#define BR_BLTU 0b110
#define BR_BGEU 0b111
#define S_W 0b010
#define S_H 0b001
#define S_B 0b000
#define L_W 0b010
#define L_H 0b001
#define L_B 0b000
#define L_BU 0b100
#define L_HU 0b101
