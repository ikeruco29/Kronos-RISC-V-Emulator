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
            instDecoded = decode_R(ir); // Recoge la instrucción decodificada y la guarda en la variable global de la clase
            //std::cout << instDecoded.op << ", " << instDecoded.registers[2] << std::endl;
            break;
        case 0b00010011:    // I
            instDecoded = decode_I(ir);
            break;
        case 0b00000011:    // I
            instDecoded = decode_I(ir, 1);
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
            instDecoded = decode_I(ir, 2);
            break;
        case 0b00110111:    // U
            decode_U();
            break;
        case 0b00010111:    // U
            decode_U();
            break;
        case 0b01110011:    // I
            instDecoded = decode_I(ir, 3);
            break;
        default:
            break;
    }
}

uint32_t CPU::execute(){
    switch (instDecoded.op)
    {
        // Formato R
        case Operation::ADD:
            ADD();
            break;
        case Operation::SUB:
            SUB();
            break;
        case Operation::XOR:
            XOR();
            break;
        case Operation::OR:
            OR();
            break;
        case Operation::AND:
            AND();
            break;
        case Operation::SLL:
            SLL();
            break;
        case Operation::SRL:
            SRL();
            break;
        case Operation::SRA:
            SRA();
            break;
        case Operation::SLT:
            SLT();
            break;
        case Operation::SLTU:
            SLTU();
            break;
            
        // Formato I
        case Operation::ADDI:
            ADDI();
            break;
        case Operation::SLTI:
            SLTI();
            break;
        case Operation::SLTIU:
            SLTIU();
            break;
        case Operation::XORI:
            XORI();
            break;
        case Operation::ORI:
            ORI();
            break;
        case Operation::ANDI:
            ANDI();
            break;
        case Operation::SLLI:
            SLLI();
            break;
        case Operation::SRLI:
            SRLI();
            break;
        case Operation::SRAI:
            SRAI();
            break;
        case Operation::LB:
            LB();
            break;
        case Operation::LH:
            LH();
            break;
        case Operation::LW:
            LW();
            break;
        case Operation::LBU:
            LBU();
            break;
        case Operation::LHU:
            LHU();
            break;
        case Operation::JALR:
            JALR();
            break;
        case Operation::ECALL:
            ECALL();
            break;
        case Operation::EBREAK:
            EBREAK();
            break;

        // Formato S
        default:
            break;
    }
    return 0;
}

void CPU::ADD(){
    uint32_t rs1 = instDecoded.registers[1];
    uint32_t rs2 = instDecoded.registers[2];
    registers[instDecoded.registers[0]] = registers[rs1] + registers[rs2];
}
