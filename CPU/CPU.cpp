#include "CPU.h"
#include <iostream>


CPU::CPU(){
    // Inicialización de los registros
    for (int i = 0; i < 32; i++)
    {
        registers[i] = 0x00000000;
    }
    
}

CPU::~CPU(){}

void CPU::fetch(uint32_t mem){
    ir = mem;
}

void CPU::decode() {
    // Recoge el opcode (últimos 7 bits)
    uint32_t opcode = ir & 0x7F;

    switch (opcode)
    {
        case 0b00110011:    // R
            Decoded decoded = decode_R(ir);            
            std::cout << decoded.op << ", " << decoded.registers[2] << std::endl;
            break;
        case 0b00010011:    // I
            decode_I(ir);
            break;
        case 0b00000011:    // I
            decode_I(ir);
            break;
        case 0b00100011:    // S
            decode_S();
            break;
        case 0b01100011:    // B
            decode_B();
            break;
        case 0b01101111:    // J
            decode_J();
            break;
        case 0b01100111:    // I
            decode_I(ir);
            break;
        case 0b00110111:    // U
            decode_U();
            break;
        case 0b00010111:    // U
            decode_U();
            break;
        case 0b01110011:    // I
            decode_I(ir);
            break;
        default:
            break;
    }
}

uint32_t CPU::execute(){
    return 0;
}

void CPU::ADD(){
    
}
