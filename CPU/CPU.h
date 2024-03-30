#pragma once
#include <cstdint>
#include <vector>
#include "decoder.h"
#include "../RAM/RAM.h"

using reg = int32_t;

class CPU {
    public:
        CPU(RAM *ram);
        ~CPU();

        RAM* ram;

        // Fetch siguiente instrucción
        void fetch(uint32_t mem);
        void decode();
        uint32_t execute();

        reg registers[32];

        uint32_t pc = 0x00000000;  // Program counter
        uint32_t ir = pc;       // Instruction register

        Decoded instDecoded;

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

};