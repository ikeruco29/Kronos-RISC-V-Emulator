#include "core/cpu.h"
#include "core/endian.h"
#include <iostream>
#include <sstream>

// Constructor
CPU::CPU(Memory *ram){
    // Register init
    for (int i = 0; i < 32; i++)
    {
        registers[i] = 0x00000000;
    }

    registers[2] = ram->iMemorySize - ram->pIo - 1; // Stack pointer

    this->ram = ram;    // RAM pointer

    instDecoded.inmediate = 0;
    instDecoded.op = -1;

    for (int i = 0; i < 40; i++) {
        ciclosTotales[i] = 0;
    }

    for (int i = 0; i < 6; i++) {
        ciclosTipo[i] = 0;
    }


    // Functions vector init
    vFunctionMap[Operation::ADD] = &CPU::ADD;
    vFunctionMap[Operation::SUB] = &CPU::SUB;
    vFunctionMap[Operation::XOR] = &CPU::XOR;
    vFunctionMap[Operation::OR] = &CPU::OR;
    vFunctionMap[Operation::AND] = &CPU::AND;
    vFunctionMap[Operation::SLL] = &CPU::SLL;
    vFunctionMap[Operation::SRL] = &CPU::SRL;
    vFunctionMap[Operation::SRA] = &CPU::SRA;
    vFunctionMap[Operation::SLT] = &CPU::SLT;
    vFunctionMap[Operation::SLTU] = &CPU::SLTU;

    // I format
    vFunctionMap[Operation::ADDI] = &CPU::ADDI;
    vFunctionMap[Operation::XORI] = &CPU::XORI;
    vFunctionMap[Operation::ORI] = &CPU::ORI;
    vFunctionMap[Operation::ANDI] = &CPU::ANDI;
    vFunctionMap[Operation::SLLI] = &CPU::SLLI;
    vFunctionMap[Operation::SRLI] = &CPU::SRLI;
    vFunctionMap[Operation::SRAI] = &CPU::SRAI;
    vFunctionMap[Operation::SLTI] = &CPU::SLTI;
    vFunctionMap[Operation::SLTIU] = &CPU::SLTIU;
    vFunctionMap[Operation::LB] = &CPU::LB;
    vFunctionMap[Operation::LH] = &CPU::LH;
    vFunctionMap[Operation::LW] = &CPU::LW;
    vFunctionMap[Operation::LBU] = &CPU::LBU;
    vFunctionMap[Operation::LHU] = &CPU::LHU;

    vFunctionMap[Operation::JALR] = &CPU::JALR;
    vFunctionMap[Operation::ECALL] = &CPU::ECALL;
    vFunctionMap[Operation::EBREAK] = &CPU::EBREAK;

    // S format
    vFunctionMap[Operation::SB] = &CPU::SB;
    vFunctionMap[Operation::SH] = &CPU::SH;
    vFunctionMap[Operation::SW] = &CPU::SW;

    // B format
    vFunctionMap[Operation::BEQ] = &CPU::BEQ;
    vFunctionMap[Operation::BNE] = &CPU::BNE;
    vFunctionMap[Operation::BLT] = &CPU::BLT;
    vFunctionMap[Operation::BGE] = &CPU::BGE;
    vFunctionMap[Operation::BLTU] = &CPU::BLTU;
    vFunctionMap[Operation::BGEU] = &CPU::BGEU;

    // J format
    vFunctionMap[Operation::JAL] = &CPU::JAL;

    // U format
    vFunctionMap[Operation::LUI] = &CPU::LUI;
    vFunctionMap[Operation::AUIPC] = &CPU::AUIPC;

    vFunctionMap[Operation::NOP] = &CPU::NOP;
}

CPU::~CPU(){}

void CPU::clock(){
    fetch();

    decode();

    if(execute() == 0)
        pc += 4;        // If its not a jump, PC + 4

    cycles++;
}


void CPU::reset(){
    // Register init
    for (int i = 0; i < 32; i++)
    {
        registers[i] = 0x00000000;
    }

    registers[2] = ram->iMemorySize - ram->pIo - 1; // Stack pointer

    instDecoded.inmediate = 0;
    instDecoded.op = -1;

    for (int i = 0; i < 39; i++) {
        ciclosTotales[i] = 0;
    }

    for (int i = 0; i < 6; i++) {
        ciclosTipo[i] = 0;
    }

    disassembly.clear();    // Empties all dissasembly

    pc = ram->iRomStartAddr;    // PC points to the program start
    ir = 0; // IR reset

    cycles = 0;

    bEbreak = 0;
    bEcall = false;
}

void CPU::fetch(){
    ir = FlipWord(ram->readWord(pc));
}

void CPU::decode() {
    std::stringstream instDisassembled;

    // opcode = last 7 bits
    uint32_t opcode = ir & 0x7F;

    switch (opcode)
    {
    case 0b00110011:    // R
        instDecoded = decode_R(ir); // Decoded instruction

        // This is so it doesnt print registers on a non-recognized instruction
        if(instDecoded.op != Operation::NOP){
            instDisassembled << formatDissasembly(instDecoded);
            instDisassembled << instDecoded.registers[0] << ", X" << instDecoded.registers[1] << ", X" << instDecoded.registers[2];
            ciclosTipo[0]++;
        }

        break;
    case 0b00010011:    // I
        instDecoded = decode_I(ir, 0);

        if(instDecoded.op != Operation::NOP){
            instDisassembled << formatDissasembly(instDecoded);
            instDisassembled << instDecoded.registers[0] << ", X" << instDecoded.registers[1] << ", " << instDecoded.inmediate;
            ciclosTipo[1]++;
        }

        break;
    case 0b00000011:    // I
        instDecoded = decode_I(ir, 1);

        if(instDecoded.op != Operation::NOP){
            instDisassembled << formatDissasembly(instDecoded);
            instDisassembled << instDecoded.registers[0] << ", X" << instDecoded.registers[1] << ", " << instDecoded.inmediate;
            ciclosTipo[1]++;
        }


        break;
    case 0b00100011:    // S
        instDecoded = decode_S(ir);

        if(instDecoded.op != Operation::NOP){
            instDisassembled << formatDissasembly(instDecoded);
            instDisassembled << instDecoded.registers[1] << ", " << instDecoded.inmediate << "(X" << instDecoded.registers[0] << ")";
            ciclosTipo[2]++;
        }

        break;
    case 0b01100011:    // B
        instDecoded = decode_B(ir);

        if(instDecoded.op != Operation::NOP){
            instDisassembled << formatDissasembly(instDecoded);
            instDisassembled << instDecoded.registers[0] << ", X" << instDecoded.registers[1] << ", " << instDecoded.inmediate;
            ciclosTipo[3]++;
        }

        break;
    case 0b01101111:    // J
        instDecoded = decode_J(ir);

        if(instDecoded.op != Operation::NOP){
            instDisassembled << formatDissasembly(instDecoded);
            instDisassembled << instDecoded.registers[0] << ", " << instDecoded.inmediate;
            ciclosTipo[5]++;
        }


        break;
    case 0b01100111:    // I
        instDecoded = decode_I(ir, 2);

        if(instDecoded.op != Operation::NOP){
            instDisassembled << formatDissasembly(instDecoded);
            instDisassembled << instDecoded.registers[0] << ", X" << instDecoded.registers[1] << ", " << instDecoded.inmediate;
            ciclosTipo[1]++;
        }

        break;
    case 0b00110111:    // U
        instDecoded = decode_U(ir, 0);

        if(instDecoded.op != Operation::NOP){
            instDisassembled << formatDissasembly(instDecoded);
            instDisassembled << instDecoded.registers[0] << ", " << instDecoded.inmediate;
            ciclosTipo[4]++;
        }

        break;
    case 0b00010111:    // U
        instDecoded = decode_U(ir, 1);

        if(instDecoded.op != Operation::NOP){
            instDisassembled << formatDissasembly(instDecoded);
            instDisassembled << instDecoded.registers[0] << ", " << instDecoded.inmediate;
            ciclosTipo[4]++;
        }

        break;
    case 0b01110011:    // I
        instDecoded = decode_I(ir, 3);

        if(instDecoded.op != Operation::NOP){
            instDisassembled << formatDissasembly(instDecoded);
            instDisassembled << instDecoded.registers[0] << ", X" << instDecoded.registers[1] << ", " << instDecoded.inmediate;
            ciclosTipo[1]++;
        }

        break;
    default:
        instDisassembled << "NOP";
        instDecoded.mnemonic = "NOP";
        instDecoded.op = Operation::NOP;
        break;
    }

    disassembly.push_back(instDisassembled.str());  // Stores instruction dissasembly

    instDisassembled.clear();
}

int CPU::execute() {

    // Executes the function stored in instDecoded
    int iIsJump = (this->*vFunctionMap[instDecoded.op])();

    ciclosTotales[instDecoded.op]++;

    return iIsJump;
}




//===================================================
//                  INSTRUCCIONES
//===================================================

// R format
int CPU::ADD(){
    // Add 2 vars and stores in rd
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    uint8_t rs2 = instDecoded.registers[2];
    registers[rd] = registers[rs1] + registers[rs2];

    return 0;
}
int CPU::SUB() {
    // substract 2 vars and stores in RD
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    uint8_t rs2 = instDecoded.registers[2];
    registers[rd] = registers[rs1] - registers[rs2];

    return 0;
}
int CPU::XOR() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    uint8_t rs2 = instDecoded.registers[2];
    registers[rd] = registers[rs1] ^ registers[rs2];

    return 0;
}
int CPU::OR() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    uint8_t rs2 = instDecoded.registers[2];
    registers[rd] = registers[rs1] | registers[rs2];

    return 0;
}
int CPU::AND() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    uint8_t rs2 = instDecoded.registers[2];
    registers[rd] = registers[rs1] & registers[rs2];

    return 0;
}
int CPU::SLL() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    uint8_t rs2 = instDecoded.registers[2];
    registers[rd] = static_cast<uint32_t>(registers[rs1]) << registers[rs2];

    return 0;
}
int CPU::SRL() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    uint8_t rs2 = instDecoded.registers[2];
    registers[rd] = static_cast<uint32_t>( registers[rs1] ) >> registers[rs2];

    return 0;
}
int CPU::SRA() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    uint8_t rs2 = instDecoded.registers[2];
    registers[rd] = registers[rs1] >> registers[rs2];

    return 0;
}
int CPU::SLT() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    uint8_t rs2 = instDecoded.registers[2];
    registers[rd] = (registers[rs1] < registers[rs2]) ? 1 : 0;

    return 0;
}
int CPU::SLTU() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    uint8_t rs2 = instDecoded.registers[2];
    registers[rd] = (static_cast<uint32_t>(registers[rs1]) < static_cast<uint32_t>(registers[rs2])) ? 1 : 0;

    return 0;
}

// I format
int CPU::ADDI() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    int32_t inmediate = instDecoded.inmediate;

    registers[rd] = registers[rs1] + inmediate;

    return 0;
}
int CPU::XORI() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    int32_t inmediate = instDecoded.inmediate;

    registers[rd] = registers[rs1] ^ inmediate;

    return 0;
}
int CPU::ORI() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    int32_t inmediate = instDecoded.inmediate;

    registers[rd] = registers[rs1] | inmediate;

    return 0;
}
int CPU::ANDI() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    int32_t inmediate = instDecoded.inmediate;

    registers[rd] = registers[rs1] & inmediate;

    return 0;
}
int CPU::SLLI() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    int32_t inmediate = instDecoded.inmediate;

    registers[rd] = static_cast<uint32_t>(registers[rs1]) << (static_cast<uint32_t>(inmediate) & 0b11111);

    return 0;
}
int CPU::SRLI() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    int32_t inmediate = instDecoded.inmediate;

    registers[rd] = static_cast<uint32_t>(registers[rs1]) >> (static_cast<uint32_t>(inmediate) & 0b11111);

    return 0;
}
int CPU::SRAI() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    int32_t inmediate = instDecoded.inmediate;

    registers[rd] = registers[rs1] >> (inmediate & 0b11111);

    return 0;
}
int CPU::SLTI() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    int32_t inmediate = instDecoded.inmediate;
    registers[rd] = (registers[rs1] < inmediate) ? 1 : 0;

    return 0;
}
int CPU::SLTIU() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    uint8_t inmediate = instDecoded.inmediate;
    registers[rd] = (static_cast<uint32_t>(registers[rs1]) < static_cast<uint32_t>(inmediate)) ? 1 : 0;

    return 0;
}

int CPU::LB() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    int32_t inmediate = instDecoded.inmediate;

    /*
        The core logic is the following:

        Obtain from the first line the first 8 bits, but the finalnumber is 0x000000FF.
        If it is stored like that, it will represet 255, but we want to be treated as a signed
        number (-1), cause its just 8 bits in theory.

        So we create a int8_t var and then store in int32_t so it adapt the number to a 32bit name.
        This same logic is applied to every LX operation.
    */

    int8_t byte = (ram->readByte(registers[rs1] + inmediate));

    registers[rd] = byte;

    return 0;
}
int CPU::LH() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    int32_t inmediate = instDecoded.inmediate;
    int16_t half = FlipHalf(ram->readHalf(registers[rs1] + inmediate));

    registers[rd] = half;

    return 0;
}
int CPU::LW() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    int32_t inmediate = instDecoded.inmediate;
    registers[rd] = FlipWord(ram->readWord(registers[rs1] + inmediate));

    return 0;
}
int CPU::LBU() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    int32_t inmediate = instDecoded.inmediate;

    // In this case, cause is an unsigned, just using AND will be ok.

    registers[rd] = (ram->readByte(registers[rs1] + inmediate)) & 0xFF;

    return 0;
}
int CPU::LHU() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    int32_t inmediate = instDecoded.inmediate;

    // In this case, cause is an unsigned, just using AND will be ok.

    registers[rd] = FlipHalf(ram->readHalf(registers[rs1] + inmediate));

    return 0;
}

int CPU::JALR() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    uint32_t inmediate = instDecoded.inmediate;

    // The direction if the jump were not made is stored in rd so it
    // can be brought back later.

    if(rd != 0)
        registers[rd] = pc + 4;
    pc = registers[rs1] + inmediate;

    return 1;
}

int CPU::ECALL() {
    // Stop program execution
    bEcall = true;
    return 0;
}
int CPU::EBREAK() {
    bEbreak = true;

    return 0;
}

// S format
int CPU::SB() {
    uint8_t rs1 = instDecoded.registers[0];
    uint8_t rs2 = instDecoded.registers[1];
    uint32_t inmediate = instDecoded.inmediate;

    uint8_t toStore = (registers[rs2] & 0xFF);

    ram->writeByte(registers[rs1] + inmediate, toStore);

    return 0;
}
int CPU::SH() {
    uint8_t rs1 = instDecoded.registers[0];
    uint8_t rs2 = instDecoded.registers[1];
    uint32_t inmediate = instDecoded.inmediate;

    uint16_t toStore = (registers[rs2] & 0xFFFF);
    toStore = FlipHalf(toStore);

    ram->writeHalf(registers[rs1] + inmediate, toStore);

    return 0;
}
int CPU::SW() {
    uint8_t rs1 = instDecoded.registers[0];
    uint8_t rs2 = instDecoded.registers[1];
    uint32_t inmediate = instDecoded.inmediate;

    uint32_t toStore = FlipWord(registers[rs2]);

    ram->writeWord(registers[rs1] + inmediate, toStore);

    return 0;
}

// B format
int CPU::BEQ() {
    uint8_t rs1 = instDecoded.registers[0];
    uint8_t rs2 = instDecoded.registers[1];
    uint32_t inmediate = instDecoded.inmediate;

    if (registers[rs1] == registers[rs2]){
        pc += inmediate;
        return 1;
    } else
        return 0;
}
int CPU::BNE() {
    uint8_t rs1 = instDecoded.registers[0];
    uint8_t rs2 = instDecoded.registers[1];
    uint32_t inmediate = instDecoded.inmediate;

    if (registers[rs1] != registers[rs2]){
        pc += inmediate;
        return 1;
    } else
        return 0;
}
int CPU::BLT() {
    uint8_t rs1 = instDecoded.registers[0];
    uint8_t rs2 = instDecoded.registers[1];
    uint32_t inmediate = instDecoded.inmediate;

    if (registers[rs1] < registers[rs2]){
        pc += inmediate;
        return 1;
    } else
        return 0;
}
int CPU::BGE() {
    uint8_t rs1 = instDecoded.registers[0];
    uint8_t rs2 = instDecoded.registers[1];
    uint32_t inmediate = instDecoded.inmediate;

    if (registers[rs1] >= registers[rs2]){
        pc += inmediate;
        return 1;
    } else
        return 0;
}
int CPU::BLTU() {
    uint8_t rs1 = instDecoded.registers[0];
    uint8_t rs2 = instDecoded.registers[1];
    uint32_t inmediate = instDecoded.inmediate;

    if (static_cast<uint32_t>(registers[rs1]) < static_cast<uint32_t>(registers[rs2])){
        pc += inmediate;
        return 1;
    } else
        return 0;
}
int CPU::BGEU() {
    uint8_t rs1 = instDecoded.registers[0];
    uint8_t rs2 = instDecoded.registers[1];
    uint32_t inmediate = instDecoded.inmediate;

    if (static_cast<uint32_t>(registers[rs1]) >= static_cast<uint32_t>(registers[rs2])){
        pc += inmediate;
        return 1;
    } else
        return 0;
    }

// J format
int CPU::JAL() {
    uint8_t rd = instDecoded.registers[0];
    uint32_t inmediate = instDecoded.inmediate;

    // The direction if the jump were not made is stored in rd so it
    // can be brought back later.

    if(rd != 0)
        registers[rd] = pc + 4;
    pc += inmediate;

    return 1;
}

// U format
int CPU::LUI() {
    uint8_t rd = instDecoded.registers[0];
    uint32_t inmediate = instDecoded.inmediate;

    registers[rd] = inmediate << 12;

    return 0;
}
int CPU::AUIPC() {
    uint8_t rd = instDecoded.registers[0];
    uint32_t inmediate = instDecoded.inmediate;

    registers[rd] = pc + (inmediate << 12);

    return 0;
}

int CPU::NOP(){
    return 0;
}


// Format disassembly for pretty print
std::string CPU::formatDissasembly(Decoded inst){
    std::stringstream st;
    st << inst.mnemonic;
    if(inst.mnemonic.length() == 2)
        st <<  "    X";
    else if(inst.mnemonic.length() == 3)
        st << "   X";
    else if(inst.mnemonic.length() == 4)
        st << "  X";
    else
        st << " X";

    return st.str();
}
