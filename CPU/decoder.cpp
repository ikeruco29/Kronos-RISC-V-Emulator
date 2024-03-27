#include "decoder.h"

Decoded decode_R(uint32_t ir) {
	//funct7 rs2 rs1 funct3 rd opcode
	uint32_t rd = (ir >> 7) & 0x1F;
	uint8_t funct3 = (ir >> 12) & 0x7;
	uint32_t rs1 = (ir >> 15) & 0x1F;
	uint32_t rs2 = (ir >> 20) & 0x1F;
	uint32_t funct7 = (ir >> 25) & 0x7F;
	int nOperation = -1;

	// Ver qué operación hay que hacer
	if (funct7 == 0x00) {
		switch (funct3)
		{
		case 0x0:
			nOperation = ADD;
			break;
		case 0x4:
			nOperation = XOR;
			break;
		case 0x6:
			nOperation = OR;
			break;
		case 0x7:
			nOperation = AND;
			break;
		case 0x1:
			nOperation = SLL;
			break;
		case 0x5:
			nOperation = SRL;
			break;
		case 0x2:
			nOperation = SLT;
			break;
		case 0x3:
			nOperation = SLTU;
			break;
		default:
			std::cerr << "ERROR: No such an operation";
			break;
		}
	}
	else {	// Solo SRA y SUB
		switch (funct3)
		{
		case 0x0:
			nOperation = SUB;
			break;
		case 0x5:
			nOperation = SRA;
			break;
		default:
			std::cerr << "ERROR: No such an operation";
			break;
		}
	}

	/*
	std::cout << "RD: " << std::hex << rd << std::endl;
	std::cout << "Funct3: " << std::hex << funct3 << std::endl;
	std::cout << "RS1: " << std::hex << rs1 << std::endl;
	std::cout << "RS2: " << std::hex << rs2 << std::endl;
	std::cout << "Funct7: " << std::hex << funct7 << std::endl;
	*/

	// Devuelve la operación y los registros 
	Decoded dec;
	dec.op = nOperation;
	dec.registers = new uint32_t[3]{ rd, rs1, rs2 };
	return dec;
}



Decoded decode_I(uint32_t ir, uint8_t op = 0) {
	// inm rs1 funct3 rd opcode
	uint32_t rd = (ir >> 7) & 0x1F;
	uint8_t funct3 = (ir >> 12) & 0x7;
	uint32_t rs1 = (ir >> 15) & 0x1F;
	int32_t inm = (ir >> 20) & 0xfff;
	uint32_t funct7 = (ir >> 25) & 0x7F;
	int nOperation = -1;

	// 1000 0101 1100
	if (inm & 0x800) { // Si el bit más significativo está establecido
		inm |= 0xFFFFF000; // Extensión del signo
	}
	// 1111 1111 1111 1111 1111 1000 0101 1100

	/*
	std::cout << "RD: " << std::hex << rd << std::endl;
	std::cout << "Funct3: " << std::hex << funct3 << std::endl;
	std::cout << "RS1: " << std::hex << rs1 << std::endl;
	std::cout << "Inmediato: " << std::hex << inm << std::endl;
	*/
	
	if (op == 0) {
		switch (funct3)
		{
			// Inmediatas
			case 0x0:
				nOperation = ADDI;
				break;
			case 0x4:
				nOperation = XORI;
				break;
			case 0x6:
				nOperation = ORI;
				break;
			case 0x7:
				nOperation = ANDI;
				break;
			case 0x1:
				nOperation = SLLI;
				break;
			case 0x5:
				if (funct7 == 0)
					nOperation = SRLI;
				else
					nOperation = SRAI;
				break;
			case 0x2:
				nOperation = SLTI;
				break;
			case 0x3:
				nOperation = SLTIU;
				break;
			default:
				std::cout << "Operación no reconocida" << std::endl;
				break;
		}
	}
	else if (op == 1) {
		switch (funct3) {
			case 0x0:
				nOperation = LB;
				break;
			case 0x1:
				nOperation = LH;
				break;
			case 0x2:
				nOperation = LW;
				break;
			case 0x4:
				nOperation = LBU;
				break;
			case 0x5:
				nOperation = LHU;
				break;
			default:
				std::cout << "Operación no reconocida" << std::endl;
				break;
			}
	}
	else if(op == 2){
		nOperation = JALR;
	}
	else if (op == 3) {
		if (inm == 0x0)
			nOperation = ECALL;
		else
			nOperation = EBREAK;
	}

	// Devuelve la operación y los registros 
	Decoded dec;
	dec.op = nOperation;
	dec.registers = new uint32_t[2]{ rd, rs1 };
	return dec;
}

void decode_S() {

}

void decode_B() {

}

void decode_U() {

}

void decode_J() {

}
