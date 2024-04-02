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

	// Devuelve la operación y los registros 
	Decoded dec;
	dec.op = nOperation;
	dec.registers = new uint32_t[3]{ rd, rs1, rs2 };
	return dec;
}



Decoded decode_I(uint32_t ir, uint8_t op) {
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
	dec.inmediate = inm;
	return dec;
}

Decoded decode_S(uint32_t ir) {
	// inm[11:5] rs2 rs1 funct3 inm[4:0] opcode
	uint32_t inm_1 = (ir >> 7) & 0x1F;		
	uint8_t funct3 = (ir >> 12) & 0x7;
	uint32_t rs1 = (ir >> 15) & 0x1F;
	uint32_t rs2 = (ir >> 20) & 0x1F;
	uint32_t inm_2 = (ir >> 25) & 0x7F;
	int nOperation = -1;

	int32_t inm = (inm_2 << 5) | inm_1; // desplaza 5 bits a la izquierda el inm_2 y lo une con inm_1

	if ((inm & 0b100000000000) != 0)	// Para el complemento a dos de los negativos
		inm = inm | 0xFFFFF000;

	if (funct3 == 0x0)
		nOperation = SB;
	else if (funct3 == 0x1)
		nOperation = SH;
	else
		nOperation = SW;

	// Devuelve la operación y los registros 
	Decoded dec;
	dec.op = nOperation;
	dec.registers = new uint32_t[2]{ rs1, rs2 };
	dec.inmediate = inm;
	return dec;
}

Decoded decode_B(uint32_t ir) {
	uint32_t inm_1 = (ir >> 7) & 0x1F;
	uint8_t funct3 = (ir >> 12) & 0x7;
	uint32_t rs1 = (ir >> 15) & 0x1F;
	uint32_t rs2 = (ir >> 20) & 0x1F;
	uint32_t inm_2 = (ir >> 25) & 0x7F;

	int nOperation = -1;

	switch (funct3)
	{
		case 0x0:
			nOperation = BEQ;
			break;
		case 0x1:
			nOperation = BNE;
			break;
		case 0x4:
			nOperation = BLT;
			break;
		case 0x5:
			nOperation = BGE;
			break;
		case 0x6:
			nOperation = BLTU;
			break;
		case 0x7:
			nOperation = BGEU;
			break;
		default:
			break;
	}

	// inm_1 tiene los bits 4:1|11
	// inm_2 tiene los bits 12|10:5
	// Esto con la extensión de signo hecha, es decir,
	// con el complemento a 2 si es necesario

	// el primer bit será el último del número
	uint8_t bit11 = inm_1 & 0x01;
	uint8_t bit12 = inm_2 >> 6;
	int32_t inmediate = 0;
	if (bit11 == 1) {
		inmediate = 0b100000000000;
	}
	
	inmediate = inmediate | (inm_1 & 0b11110); // 11011 -> 1101 0
	inmediate = inmediate | ((inm_2 << 5) & 0b011111111111); // 1001011    -> 1001 0110 0000

	if (bit12 == 1) {
		// 1111 1111 1111 1111 1111 0000 0000 0000
		inmediate = inmediate | 0xFFFFF000;
	}	

	// Devuelve la operación y los registros 
	Decoded dec;
	dec.op = nOperation;
	dec.registers = new uint32_t[2]{ rs1, rs2 };
	dec.inmediate = inmediate;
	return dec;
}

Decoded decode_U(uint32_t ir, uint8_t op) {
	uint32_t rd = (ir >> 7) & 0x1F;
	int32_t inm = (ir >> 12);

	Decoded dec;

	if (op == 0)
		dec.op = LUI;
	else
		dec.op = AUIPC;

	dec.inmediate = inm;
	dec.registers = new uint32_t[1] {rd};
	return dec;
}

Decoded decode_J(uint32_t ir) {
	uint32_t rd = (ir >> 7) & 0x1F;
	int32_t inm = (ir >> 12);

	// Bit 20 en la posición 19 (empezando en 0)
	uint32_t bit20 = inm & 0b10000000000000000000;

	// Bit 11 en la posición 8 (empezando en 0)
	uint16_t bit11 = inm & 0b100000000;

	uint32_t inm_2 = (inm & 0xFF) << 12;
	uint16_t inm_1 = (inm >> 8) & 0x7FE; // 0111 1111 1110

	int32_t inm_fin = inm_2 | inm_1;
	if (bit11 != 0)	// Si el bit 11 está en 1, lo añade
		inm_fin = inm_fin | 0b100000000000;
	
	// Si el bit 20 está en 1, significa que es
	// un número negativo, por lo que hay que rellenar todo
	// lo demás con 1.
	if (bit20 != 0)	
		inm_fin = inm_fin | 0xfff00000;


	Decoded dec;
	dec.op = JAL;
	dec.inmediate = inm_fin;
	dec.registers = new uint32_t[1]{ rd };
	return dec;
}
