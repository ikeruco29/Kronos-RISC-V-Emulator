/*
	Este programa contendrá todas las funciones de decodificación
	para tenerlo ordenado
*/
#include <cstdint>
#include <iostream>


enum Operation
{
	// Formato R
	ADD, SUB, XOR, OR, AND, SLL, SRL, SRA, SLT, SLTU,

	// Formato I
	ADDI, XORI, ORI, ANDI,
	SLLI, SRLI, SRAI, SLTI, SLTUI,
	LB, LH, LW, LBU, LHU,

	JALR,
	ECALL, EBREAK,

	// Formato S
	SB, SH, SW,

	// Formato B
	BEQ, BNE, BLT, BGE, BLTU,
	BGEU,

	// Formato J
	JAL,

	// Formato U
	LUI, AUIPC
};

struct Decoded
{
	int op;
	uint32_t* registers;
};

Decoded decode_R(uint32_t ir);

void decode_I(uint32_t ir);

void decode_S();

void decode_B();

void decode_U();

void decode_J();
