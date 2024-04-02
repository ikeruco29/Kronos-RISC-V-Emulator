#include "Computer.h"

Computer::Computer(int RAM_SIZE) : ram(RAM(RAM_SIZE)), cpu(CPU(&ram)) {};

Computer::~Computer() {}

void Computer::On(int cycles) {
	while (cycles != 0) {
		cpu.fetch(ram.readWord(cpu.pc));
		cpu.decode();
		cpu.execute();
		
		cpu.pc += 4;

		cycles -= 1;
	}
}

int Computer::LoadProgram(std::string filename) {

	// Abrir el archivo en modo binario
	std::ifstream file(filename, std::ios::binary);
	if (!file.is_open()) {
		std::cerr << "Error al abrir el archivo: " << filename << std::endl;
		return 1;
	}

	// Leer el archivo byte por byte
	uint8_t byte;
	int i = 0;
	while (file.read(reinterpret_cast<char*>(&byte), sizeof(uint8_t))) {
		ram.writeByte(i, byte);
		i++;
	}

	file.close();
}