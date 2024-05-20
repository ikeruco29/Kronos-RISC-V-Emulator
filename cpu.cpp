#include "cpu.h"
#include "endian.h"
#include <iostream>
#include <sstream>


CPU::CPU(Memory *ram){
    // Inicialización de los registros
    for (int i = 0; i < 32; i++)
    {
        registers[i] = 0x00000000;
    }

    registers[2] = ram->iMemorySize - ram->pIo - 1; // Puntero stack

    this->ram = ram;

    instDecoded.inmediate = 0;
    instDecoded.op = -1;

    for (int i = 0; i < 40; i++) {
        ciclosTotales[i] = 0;
    }

    for (int i = 0; i < 6; i++) {
        ciclosTipo[i] = 0;
    }


    // Inicialización del vector de funciones
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

    // Formato I
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

    // Formato S
    vFunctionMap[Operation::SB] = &CPU::SB;
    vFunctionMap[Operation::SH] = &CPU::SH;
    vFunctionMap[Operation::SW] = &CPU::SW;

    // Formato B
    vFunctionMap[Operation::BEQ] = &CPU::BEQ;
    vFunctionMap[Operation::BNE] = &CPU::BNE;
    vFunctionMap[Operation::BLT] = &CPU::BLT;
    vFunctionMap[Operation::BGE] = &CPU::BGE;
    vFunctionMap[Operation::BLTU] = &CPU::BLTU;
    vFunctionMap[Operation::BGEU] = &CPU::BGEU;

    // Formato J
    vFunctionMap[Operation::JAL] = &CPU::JAL;

    // Formato U
    vFunctionMap[Operation::LUI] = &CPU::LUI;
    vFunctionMap[Operation::AUIPC] = &CPU::AUIPC;

    vFunctionMap[Operation::NOP] = &CPU::NOP;
}

CPU::~CPU(){}

void CPU::clock(){
    fetch();

    decode();

    if(execute() == 0)
        pc += 4;

    cycles++;
}

void CPU::reset(){
    // Inicialización de los registros
    for (int i = 0; i < 32; i++)
    {
        registers[i] = 0x00000000;
    }

    registers[2] = ram->iMemorySize - ram->pIo - 1; // Puntero stack

    instDecoded.inmediate = 0;
    instDecoded.op = -1;

    for (int i = 0; i < 39; i++) {
        ciclosTotales[i] = 0;
    }

    for (int i = 0; i < 6; i++) {
        ciclosTipo[i] = 0;
    }

    disassembly.clear();

    pc = ram->iRomStartAddr;
    ir = 0;

    cycles = 0;

    bEbreak = 0;
}

void CPU::fetch(){
    ir = FlipWord(ram->readWord(pc));
}

void CPU::decode() {
    std::stringstream instDisassembled;
    // Recoge el opcode (últimos 7 bits)
    uint32_t opcode = ir & 0x7F;

    switch (opcode)
    {
    case 0b00110011:    // R
        instDecoded = decode_R(ir); // Recoge la instrucción decodificada y la guarda en la variable global de la clase


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

    disassembly.push_back(instDisassembled.str());

    instDisassembled.clear();
}

int CPU::execute() {

    // Se ejecuta la función almacenada en instDecoded
    int iIsJump = (this->*vFunctionMap[instDecoded.op])();

    ciclosTotales[instDecoded.op]++;

    return iIsJump;
}




//===================================================
//                  INSTRUCCIONES
//===================================================

// R format
int CPU::ADD(){
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    uint8_t rs2 = instDecoded.registers[2];
    registers[rd] = registers[rs1] + registers[rs2];

    return 0;
}
int CPU::SUB() {
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
// TODO: PENSAR SI, POR EJEMPLO, EN LA POSICIÓN 1 HAY EL NUMERO 0X0000FFFF,
//       SI RECOJO UN BYTE, ES DECIR FF, QUÉ SUCEDE CON LOS OTROS 3 BYTES,
//       SE RELLENAN A 1 PORQUE TOMA COMO SIGNO EL BIT NÚMERO 8 O NO?
int CPU::LB() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    int32_t inmediate = instDecoded.inmediate;

    /* La lógica de esto es la siguiente:

        Con la primera línea se obtienen los
        primeros 8 bits, pero el número resultante es
        0x000000FF. Esto si se guarda directamente en una variable
        de tipo int32_t, se queda tal cual representando el número
        255.

        Pero lo que queremos es que ese número se interprete
        como -1, que es el que realmente es si solo interpretamos los primeros
        8 bits, por lo que lo guardamos en una variable int8_t para que lo interprete como
        -1 y luego lo guardamos en el registro, que es un int32_t, pero como parte de un
        -1, extiende automáticamente el signo sin tener que hcer operaciones.
        Esta misma lógica aplica al resto de operaciones LX.
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

    return 0;
    registers[rd] = half;
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

    // En este caso, al ser un unsigned, con hacer el AND ya
    // saca el valor adecuado

    registers[rd] = (ram->readByte(registers[rs1] + inmediate)) & 0xFF;

    return 0;
}
int CPU::LHU() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    int32_t inmediate = instDecoded.inmediate;

    // En este caso, al ser un unsigned, con hacer el AND ya
    // saca el valor adecuado

    registers[rd] = FlipHalf(ram->readHalf(registers[rs1] + inmediate));

    return 0;
}

int CPU::JALR() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    uint32_t inmediate = instDecoded.inmediate;

    // La dirección que tocaría si no se hiciera el salto se guarda en rd
    // para saltar más adelante de vuelta

    registers[rd] = pc + 4;
    pc = registers[rs1] + inmediate;

    return 1;
}

int CPU::ECALL() {
    // Detener ejecución de programa

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

    if (static_cast<uint32_t>(registers[rs1]) >= static_cast<uint32_t>(registers[rs2])){ // estaba <=
        pc += inmediate;
        return 1;
    } else
        return 0;
    }

// J format
int CPU::JAL() {
    uint8_t rd = instDecoded.registers[0];
    uint32_t inmediate = instDecoded.inmediate;

    // La dirección que tocaría si no se hiciera el salto se guarda en rd
    // para saltar más adelante de vuelta

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
