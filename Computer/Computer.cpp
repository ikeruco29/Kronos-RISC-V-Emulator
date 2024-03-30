#include "Computer.h"

Computer::Computer(int RAM_SIZE) : ram(RAM(RAM_SIZE)), cpu(CPU(&ram)) {};

Computer::~Computer() {}

void Computer::On(int cycles) {
	while (cycles != 0) {
		cpu.fetch(ram.read(cpu.pc));
		cpu.decode();
		cpu.execute();
		
		cpu.pc += 1;

		cycles -= 1;
	}
}