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