#ifndef DECODER_H
#define DECODER_H

/*
    Este programa contendra todas las funciones de decodificacion
    para tenerlo ordenado
*/
#include <cstdint>

enum Operation
{
    // Formato R
    ADD, SUB, XOR, OR, AND, SLL, SRL, SRA, SLT, SLTU,

    // Formato I
    ADDI, XORI, ORI, ANDI,
    SLLI, SRLI, SRAI, SLTI, SLTIU,
    LB, LH, LW, LBU, LHU,

    JALR,
    ECALL, EBREAK,

    // Formato S
    SB, SH, SW,

    // Formato B
    BEQ, BNE, BLT, BGE, BLTU,
    BGEU,

    // Formato J
    JAL,

    // Formato U
    LUI, AUIPC
};

struct Decoded
{
    int op;
    uint32_t* registers;
    int32_t inmediate;
};

Decoded decode_R(uint32_t ir);

Decoded decode_I(uint32_t ir, uint8_t op);

Decoded decode_S(uint32_t ir);

Decoded decode_B(uint32_t ir);

Decoded decode_U(uint32_t ir, uint8_t op);

Decoded decode_J(uint32_t ir);


#endif // DECODER_H
