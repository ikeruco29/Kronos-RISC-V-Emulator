/*
	Este programa contendrá todas las funciones de decodificación
	para tenerlo ordenado
*/

#include <cstdint>
#include <iostream>

int decode_R(uint32_t ir) {
	//funct7 rs2 rs1 funct3 rd opcode
	uint32_t rd = (ir >> 7) & 0x1F;
	uint32_t funct3 = (ir >> 12) & 0x7;
	uint32_t rs1 = (ir >> 15) & 0x1F;
	uint32_t rs2 = (ir >> 20) & 0x1F;
	uint32_t funct7 = (ir >> 25) & 0x7F;

	/* IMPRIMIR
	
	std::cout << "RD: " << std::hex << rd << std::endl;
	std::cout << "Funct3: " << std::hex << funct3 << std::endl;
	std::cout << "RS1: " << std::hex << rs1 << std::endl;
	std::cout << "RS2: " << std::hex << rs2 << std::endl;
	std::cout << "Funct7: " << std::hex << funct7 << std::endl;
	*/
	return 0;
}

void decode_I() {

}

void decode_S() {

}

void decode_B() {

}

void decode_U() {

}

void decode_J() {

}