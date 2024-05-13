#ifndef CPU_H
#define CPU_H

#pragma once
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <string>
#include "decoder.h"
#include "memory.h"

using reg = int32_t;

class CPU {
public:
    CPU(Memory *ram);
    ~CPU();

    Memory* ram;
    bool bEbreak = false;
    uint32_t cycles = 0;

    // Fetch siguiente instrucción
    void fetch();
    void decode();
    int execute();

    reg registers[32];

    uint32_t pc = 0x00000000;  // Program counter
    uint32_t ir = 0x00000000;       // Instruction register

    std::unordered_map<int, int (CPU::*)()> vFunctionMap;

    Decoded instDecoded;

    std::vector<std::string> disassembly;
    std::string formatDissasembly(Decoded inst);

    void clock();
    void reset();

    // INSTRUCTIONS
    // R format
    int ADD(); int SUB(); int XOR(); int OR(); int AND();
    int SLL(); int SRL(); int SRA(); int SLT(); int SLTU();

    // I format
    int ADDI(); int XORI(); int ORI(); int ANDI();
    int SLLI(); int SRLI(); int SRAI(); int SLTI(); int SLTIU();
    int LB(); int LH(); int LW(); int LBU(); int LHU();

    int JALR();
    int ECALL(); int EBREAK();

    // S format
    int SB(); int SH(); int SW();

    // B format
    int BEQ(); int BNE(); int BLT(); int BGE(); int BLTU();
    int BGEU();

    // J format
    int JAL();

    // U format
    int LUI(); int AUIPC();

    int NOP();


    // Para los ciclos
    uint64_t ciclosTotales[40];
};

#endif // CPU_H
