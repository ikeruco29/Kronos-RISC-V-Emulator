#include "computer.h"
#include <sstream>
#include <iostream>
#include <iomanip>

Computer::Computer(int RAM_SIZE) : ram(RAM(RAM_SIZE)), cpu(CPU(&ram)), ram_size(RAM_SIZE) {};

Computer::~Computer() {}

void Computer::On(bool pasoapaso) {

    if(pasoapaso == true){
        while (pasoapaso != true) {
            cpu.fetch(ram.readWord(cpu.pc));
            cpu.decode();
            cpu.execute();

            cpu.pc += 4;
            while(!cpu.bEbreak){

            }
            //cycles -= 1;
        }
    } else {
        while (pasoapaso != true) {
            cpu.fetch(ram.readWord(cpu.pc));
            cpu.decode();
            cpu.execute();

            cpu.pc += 4;

            //cycles -= 1;
        }
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
    return 0;
}

std::string Computer::exportRam(){
    std::stringstream ss;
    ss << "          00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\r\n\n";
    ss << "00000000  ";
    int count = 1;
    for(int i = 0; i < ram_size; i++){
        ss << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(ram.readByte(i)) << " ";

        if (count == 16) {
            ss << "\r\n";
            ss << std::hex << std::setw(8) << std::setfill('0') << i+1;
            ss << "  ";
            count = 0;
        }

        count++;
    }

    return ss.str();

}
