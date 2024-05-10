#ifndef CPU_H
#define CPU_H

#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include "decoder.h"
#include "ram.h"

using reg = int32_t;

class CPU {
public:
    CPU(RAM *ram);
    ~CPU();

    RAM* ram;
    bool bEbreak = false;
    uint32_t cycles = 0;

    // Fetch siguiente instrucción
    void fetch(uint32_t mem);
    void decode();
    uint32_t execute();

    reg registers[32];

    uint32_t pc = 0x00000000;  // Program counter
    uint32_t ir = 0x00000000;       // Instruction register

    Decoded instDecoded;

    std::vector<std::string> disassembly;
    std::string formatDissasembly(Decoded inst);

    void clock();
    void reset();

    // INSTRUCTIONS
    // R format
    void ADD(); void SUB(); void XOR(); void OR(); void AND();
    void SLL(); void SRL(); void SRA(); void SLT(); void SLTU();

    // I format
    void ADDI(); void XORI(); void ORI(); void ANDI();
    void SLLI(); void SRLI(); void SRAI(); void SLTI(); void SLTIU();
    void LB(); void LH(); void LW(); void LBU(); void LHU();

    void JALR();
    void ECALL(); void EBREAK();

    // S format
    void SB(); void SH(); void SW();

    // B format
    void BEQ(); void BNE(); void BLT(); void BGE(); void BLTU();
    void BGEU();

    // J format
    void JAL();

    // U format
    void LUI(); void AUIPC();



    // Para los ciclos
    uint64_t ciclosTotales[39];
};

#endif // CPU_H
