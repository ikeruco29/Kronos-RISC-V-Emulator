#pragma once
#include <cstdint>
#include <vector>
//#include "decoder.h"

using Register = int32_t;

class CPU {
    public:
        CPU();
        ~CPU();

        enum FMT {
            R,
            I,
            S,
            B,
            U,
            J
        };

        // Instructions
        struct Instruction {
            FMT format;
            uint8_t funct3;
            uint8_t funct7;
            uint8_t opcode;
            void (CPU::*method)(void); // puntero a función de instrucción
            uint8_t cycles;
        };

        std::vector<Instruction> opcodes;

        // Fetch siguiente instrucción
        void fetch(uint32_t mem);
        Instruction decode();
        uint32_t execute();

        Register registers[32];
        // Registros
        /*
        Register ra = 0x00000;          // Return calls
        Register sp = 0x00000; 
        Register gp = 0x00000; 
        Register tp = 0x00000;

        Register t0 = 0x00000; Register t1 = 0x00000; Register t2 = 0x00000; Register t3 = 0x00000;
        Register t4 = 0x00000; Register t5 = 0x00000; Register t6 = 0x00000; Register fp = 0x00000; 

        Register s1 = 0x00000; Register s2 = 0x00000; Register s3 = 0x00000; Register s4 = 0x00000; 
        Register s5 = 0x00000; Register s6 = 0x00000; Register s7 = 0x00000; Register s8 = 0x00000; 
        Register s9 = 0x00000; Register s10 = 0x00000; Register s11 = 0x00000;

        Register a0 = 0x00000; Register a1 = 0x00000; Register a2 = 0x00000; Register a3 = 0x00000;
        Register a4 = 0x00000; Register a5 = 0x00000; Register a6 = 0x00000; Register a7 = 0x00000; 
        */

        uint32_t pc = 0x00000000;  // Program counter
        uint32_t ir = pc;       // Instruction register


        // INSTRUCTIONS
        // R format
        void ADD(); void SUB(); void XOR(); void OR(); void AND();
        void SLL(); void SRL(); void SRA(); void SLT(); void SLTU(); 

        // I format
        void ADDI(); void XORI(); void ORI(); void ANDI();
        void SLLI(); void SRLI(); void SRAI(); void SLTI(); void SLTUI(); 
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