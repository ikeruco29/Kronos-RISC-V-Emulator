#include "CPU.h"
#include <iostream>

CPU::CPU(){
    // Inicialización de los registros
    for (int i = 0; i < 32; i++)
    {
        registers[i] = 0x00000000;
    }
    

    opcodes = {
        {R,0b0110011,0x0,0x00,&CPU::ADD,1},
        // {R,0b0110011,0x0,0x20,&CPU::SUB,1},
        // {R,0b0110011,0x4,0x00,&CPU::XOR,1},
        // {R,0b0110011,0x6,0x00,&CPU::OR,1},
        // {R,0b0110011,0x7,0x00,&CPU::AND,1},
        // {R,0b0110011,0x1,0x00,&CPU::SLL,1},
        // {R,0b0110011,0x5,0x00,&CPU::SRL,1},
        // {R,0b0110011,0x5,0x20,&CPU::SRA,1},
        // {R,0b0110011,0x2,0x00,&CPU::SLT,1},
        // {R,0b0110011,0x3,0x00,&CPU::SLTU,1},
        
        // TODO terminar opcodes

    };

}

CPU::~CPU(){}

void CPU::fetch(uint32_t mem){
    ir = mem;
}
CPU::Instruction CPU::decode(){
    Instruction currInstruction = Instruction();

    uint8_t nOpcode = ir & 0x7F;

    currInstruction.opcode = nOpcode;

    if(nOpcode == 0b00110011)
        currInstruction.format = R;
    else if(nOpcode == 0b00010011 || nOpcode == 0b00000011 || nOpcode == 0b01100111 || nOpcode == 0b01110011)
        currInstruction.format = I;
    else if(nOpcode == 0b00100011)
        currInstruction.format = S;
    else if(nOpcode == 0b01101111)
        currInstruction.format = J;
    else 
        currInstruction.format = U;
    
    // 5 bits
    //uint8_t nRd = (ir >> )
    //// 3 bits
    //uint8_t funct3 =
    ////5 bits
    //uint8_t nRs1 = 
    //// 5 bits
    //uint8_t nRs2 =
    //// 7 bits
    //uint8_t nFunct7 =
    return currInstruction;
}

uint32_t CPU::execute(){
    return 0;
}

void CPU::ADD(){
    
}
