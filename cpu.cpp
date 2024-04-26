#include "cpu.h"
#include "endian.h"
#include <iostream>
#include <sstream>


CPU::CPU(RAM *ram){
    // Inicialización de los registros
    for (int i = 0; i < 32; i++)
    {
        registers[i] = 0x00000000;
    }
    this->ram = ram;

    instDecoded.inmediate = 0;
    instDecoded.op = -1;

    for (int i = 0; i < 39; i++) {
        ciclosTotales[i] = 0;
    }
}

CPU::~CPU(){}

void CPU::clock(){
    fetch(ram->readWord(pc));
    decode();
    execute();

    pc += 4;
    cycles++;
}

void CPU::reset(){
    // Inicialización de los registros
    for (int i = 0; i < 32; i++)
    {
        registers[i] = 0x00000000;
    }

    instDecoded.inmediate = 0;
    instDecoded.op = -1;

    for (int i = 0; i < 39; i++) {
        ciclosTotales[i] = 0;
    }

    disassembly.clear();

    pc = 0;
    ir = 0;

    cycles = 0;

    bEbreak = 0;
}

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
        instDecoded = decode_I(ir, 0);
        break;
    case 0b00000011:    // I
        instDecoded = decode_I(ir, 1);
        break;
    case 0b00100011:    // S
        instDecoded = decode_S(ir);
        break;
    case 0b01100011:    // B
        instDecoded = decode_B(ir);
        break;
    case 0b01101111:    // J
        instDecoded = decode_J(ir);
        break;
    case 0b01100111:    // I
        instDecoded = decode_I(ir, 2);
        break;
    case 0b00110111:    // U
        instDecoded = decode_U(ir, 0);
        break;
    case 0b00010111:    // U
        instDecoded = decode_U(ir, 1);
        break;
    case 0b01110011:    // I
        instDecoded = decode_I(ir, 3);
        break;
    default:
        break;
    }
}

uint32_t CPU::execute(){
    std::stringstream instDisassembled;
    switch (instDecoded.op)
    {
    // Formato R
    case Operation::ADD:
        ADD();
        instDisassembled << "ADD   X" << instDecoded.registers[0] << ", X" << instDecoded.registers[1] << ", X" << instDecoded.registers[2];
        disassembly.push_back(instDisassembled.str());
        ciclosTotales[Operation::ADD]++;
        break;
    case Operation::SUB:
        SUB();
        instDisassembled << "SUB   X" << instDecoded.registers[0] << ", X" << instDecoded.registers[1] << ", X" << instDecoded.registers[2];
        disassembly.push_back(instDisassembled.str());
        ciclosTotales[Operation::SUB]++;
        break;
    case Operation::XOR:
        XOR();
        instDisassembled << "XOR   X" << instDecoded.registers[0] << ", X" << instDecoded.registers[1] << ", X" << instDecoded.registers[2];
        disassembly.push_back(instDisassembled.str());
        ciclosTotales[Operation::XOR]++;
        break;
    case Operation::OR:
        OR();
        instDisassembled << "OR    X" << instDecoded.registers[0] << ", X" << instDecoded.registers[1] << ", X" << instDecoded.registers[2];
        disassembly.push_back(instDisassembled.str());
        ciclosTotales[Operation::OR]++;
        break;
    case Operation::AND:
        AND();
        instDisassembled << "AND   X" << instDecoded.registers[0] << ", X" << instDecoded.registers[1] << ", X" << instDecoded.registers[2];
        disassembly.push_back(instDisassembled.str());
        ciclosTotales[Operation::AND]++;
        break;
    case Operation::SLL:
        SLL();
        instDisassembled << "SLL   X" << instDecoded.registers[0] << ", X" << instDecoded.registers[1] << ", X" << instDecoded.registers[2];
        disassembly.push_back(instDisassembled.str());
        ciclosTotales[Operation::SLL]++;
        break;
    case Operation::SRL:
        SRL();
        instDisassembled << "SRL   X" << instDecoded.registers[0] << ", X" << instDecoded.registers[1] << ", X" << instDecoded.registers[2];
        disassembly.push_back(instDisassembled.str());
        ciclosTotales[Operation::SRL]++;
        break;
    case Operation::SRA:
        SRA();
        instDisassembled << "SRA   X" << instDecoded.registers[0] << ", X" << instDecoded.registers[1] << ", X" << instDecoded.registers[2];
        disassembly.push_back(instDisassembled.str());
        ciclosTotales[Operation::SRA]++;
        break;
    case Operation::SLT:
        SLT();
        instDisassembled << "SLT   X" << instDecoded.registers[0] << ", X" << instDecoded.registers[1] << ", X" << instDecoded.registers[2];
        disassembly.push_back(instDisassembled.str());
        ciclosTotales[Operation::SLT]++;
        break;
    case Operation::SLTU:
        SLTU();
        instDisassembled << "SLTU  X" << instDecoded.registers[0] << ", X" << instDecoded.registers[1] << ", X" << instDecoded.registers[2];
        disassembly.push_back(instDisassembled.str());
        ciclosTotales[Operation::SLTU]++;
        break;

        // Formato I
    case Operation::ADDI:
        ADDI();
        instDisassembled << "ADDI  X" << instDecoded.registers[0] << ", X" << instDecoded.registers[1] << ", " << instDecoded.inmediate;
        disassembly.push_back(instDisassembled.str());
        ciclosTotales[Operation::ADDI]++;
        break;
    case Operation::SLTI:
        SLTI();
        instDisassembled << "SLTI  X" << instDecoded.registers[0] << ", X" << instDecoded.registers[1] << ", " << instDecoded.inmediate;
        disassembly.push_back(instDisassembled.str());
        ciclosTotales[Operation::SLTI]++;
        break;
    case Operation::SLTIU:
        SLTIU();
        instDisassembled << "SLTIU X" << instDecoded.registers[0] << ", X" << instDecoded.registers[1] << ", " << instDecoded.inmediate;
        disassembly.push_back(instDisassembled.str());
        ciclosTotales[Operation::SLTIU]++;
        break;
    case Operation::XORI:
        XORI();
        instDisassembled << "XORI  X" << instDecoded.registers[0] << ", X" << instDecoded.registers[1] << ", " << instDecoded.inmediate;
        disassembly.push_back(instDisassembled.str());
        ciclosTotales[Operation::XORI]++;
        break;
    case Operation::ORI:
        ORI();
        instDisassembled << "ORI   X" << instDecoded.registers[0] << ", X" << instDecoded.registers[1] << ", " << instDecoded.inmediate;
        disassembly.push_back(instDisassembled.str());
        ciclosTotales[Operation::ORI]++;
        break;
    case Operation::ANDI:
        ANDI();
        instDisassembled << "ANDI  X" << instDecoded.registers[0] << ", X" << instDecoded.registers[1] << ", " << instDecoded.inmediate;
        disassembly.push_back(instDisassembled.str());
        ciclosTotales[Operation::ANDI]++;
        break;
    case Operation::SLLI:
        SLLI();
        instDisassembled << "SLLI  X" << instDecoded.registers[0] << ", X" << instDecoded.registers[1] << ", " << instDecoded.inmediate;
        disassembly.push_back(instDisassembled.str());
        ciclosTotales[Operation::SLLI]++;
        break;
    case Operation::SRLI:
        SRLI();
        instDisassembled << "SRLI  X" << instDecoded.registers[0] << ", X" << instDecoded.registers[1] << ", " << instDecoded.inmediate;
        disassembly.push_back(instDisassembled.str());
        ciclosTotales[Operation::SRLI]++;
        break;
    case Operation::SRAI:
        SRAI();
        instDisassembled << "SRAI  X" << instDecoded.registers[0] << ", X" << instDecoded.registers[1] << ", " << instDecoded.inmediate;
        disassembly.push_back(instDisassembled.str());
        ciclosTotales[Operation::SRAI]++;
        break;
    case Operation::LB:
        LB();
        instDisassembled << "LB    X" << instDecoded.registers[0] << ", " << instDecoded.inmediate << "(X" << instDecoded.registers[1] << ")";
        disassembly.push_back(instDisassembled.str());
        ciclosTotales[Operation::LB]++;
        break;
    case Operation::LH:
        LH();
        instDisassembled << "LH    X" << instDecoded.registers[0] << ", " << instDecoded.inmediate << "(X" << instDecoded.registers[1] << ")";
        disassembly.push_back(instDisassembled.str());
        ciclosTotales[Operation::LH]++;
        break;
    case Operation::LW:
        LW();
        instDisassembled << "LW    X" << instDecoded.registers[0] << ", " << instDecoded.inmediate << "(X" << instDecoded.registers[1] << ")";
        disassembly.push_back(instDisassembled.str());
        ciclosTotales[Operation::LW]++;
        break;
    case Operation::LBU:
        LBU();
        instDisassembled << "LBU   X" << instDecoded.registers[0] << ", " << instDecoded.inmediate << "(X" << instDecoded.registers[1] << ")";
        disassembly.push_back(instDisassembled.str());
        ciclosTotales[Operation::LBU]++;
        break;
    case Operation::LHU:
        LHU();
        instDisassembled << "LHU   X" << instDecoded.registers[0] << ", " << instDecoded.inmediate << "(X" << instDecoded.registers[1] << ")";
        disassembly.push_back(instDisassembled.str());
        ciclosTotales[Operation::LHU]++;
        break;
    case Operation::JALR:
        JALR();
        instDisassembled << "JALR  X" << instDecoded.registers[0] << ", " << instDecoded.inmediate << "(X" << instDecoded.registers[1] << ")";
        disassembly.push_back(instDisassembled.str());
        ciclosTotales[Operation::JALR]++;
        break;
    case Operation::ECALL:
        ECALL();
        instDisassembled << "ECALL";
        disassembly.push_back(instDisassembled.str());
        ciclosTotales[Operation::ECALL]++;
        break;
    case Operation::EBREAK:
        EBREAK();
        instDisassembled << "EBREAK";
        disassembly.push_back(instDisassembled.str());
        ciclosTotales[Operation::EBREAK]++;
        break;

        // Formato S
    case Operation::SB:
        SB();
        instDisassembled << "SB    X" << instDecoded.registers[1] << ", " << instDecoded.inmediate << "(X" << instDecoded.registers[0] << ")";
        disassembly.push_back(instDisassembled.str());
        ciclosTotales[Operation::SB]++;
        break;
    case Operation::SH:
        SH();
        instDisassembled << "SH    X" << instDecoded.registers[1] << ", " << instDecoded.inmediate << "(X" << instDecoded.registers[0] << ")";
        disassembly.push_back(instDisassembled.str());
        ciclosTotales[Operation::SH]++;
        break;
    case Operation::SW:
        SW();
        instDisassembled << "SW    X" << instDecoded.registers[1] << ", " << instDecoded.inmediate << "(X" << instDecoded.registers[0] << ")";
        disassembly.push_back(instDisassembled.str());
        ciclosTotales[Operation::SW]++;
        break;

        //Formato B
    case Operation::BEQ:
        BEQ();
        instDisassembled << "BEQ   X" << instDecoded.registers[0] << ", X" << instDecoded.registers[1] << ", " << instDecoded.inmediate;
        disassembly.push_back(instDisassembled.str());
        ciclosTotales[Operation::BEQ]++;
        break;
    case Operation::BNE:
        BNE();
        instDisassembled << "BNE   X" << instDecoded.registers[0] << ", X" << instDecoded.registers[1] << ", " << instDecoded.inmediate;
        disassembly.push_back(instDisassembled.str());
        ciclosTotales[Operation::BNE]++;
        break;
    case Operation::BLT:
        BLT();
        instDisassembled << "BLT   X" << instDecoded.registers[0] << ", X" << instDecoded.registers[1] << ", " << instDecoded.inmediate;
        disassembly.push_back(instDisassembled.str());
        ciclosTotales[Operation::BLT]++;
        break;
    case Operation::BGE:
        BGE();
        instDisassembled << "BGE   X" << instDecoded.registers[0] << ", X" << instDecoded.registers[1] << ", " << instDecoded.inmediate;
        disassembly.push_back(instDisassembled.str());
        ciclosTotales[Operation::BGE]++;
        break;
    case Operation::BLTU:
        BLTU();
        instDisassembled << "BLTU  X" << instDecoded.registers[0] << ", X" << instDecoded.registers[1] << ", " << instDecoded.inmediate;
        disassembly.push_back(instDisassembled.str());
        ciclosTotales[Operation::BLTU]++;
        break;
    case Operation::BGEU:
        BGEU();
        instDisassembled << "BGEU  X" << instDecoded.registers[0] << ", X" << instDecoded.registers[1] << ", " << instDecoded.inmediate;
        disassembly.push_back(instDisassembled.str());
        ciclosTotales[Operation::BGEU]++;
        break;

        // Formato U
    case Operation::LUI:
        LUI();
        instDisassembled << "LUI   X" << instDecoded.registers[0] << ", " << instDecoded.inmediate;
        disassembly.push_back(instDisassembled.str());
        ciclosTotales[Operation::LUI]++;
        break;
    case Operation::AUIPC:
        AUIPC();
        instDisassembled << "AUIPC X" << instDecoded.registers[0] << ", " << instDecoded.inmediate;
        disassembly.push_back(instDisassembled.str());
        ciclosTotales[Operation::AUIPC]++;
        break;

        // Formato J
    case Operation::JAL:
        JAL();
        instDisassembled << "JAL   X" << instDecoded.registers[0] << ", " << instDecoded.inmediate;
        disassembly.push_back(instDisassembled.str());
        ciclosTotales[Operation::JAL]++;
        break;

    default:
        break;
    }
    instDisassembled.clear();
    return 0;
}

// R format
void CPU::ADD(){
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    uint8_t rs2 = instDecoded.registers[2];
    registers[rd] = registers[rs1] + registers[rs2];
}
void CPU::SUB() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    uint8_t rs2 = instDecoded.registers[2];
    registers[rd] = registers[rs1] - registers[rs2];
}
void CPU::XOR() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    uint8_t rs2 = instDecoded.registers[2];
    registers[rd] = registers[rs1] ^ registers[rs2];
}
void CPU::OR() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    uint8_t rs2 = instDecoded.registers[2];
    registers[rd] = registers[rs1] | registers[rs2];
}
void CPU::AND() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    uint8_t rs2 = instDecoded.registers[2];
    registers[rd] = registers[rs1] & registers[rs2];
}
void CPU::SLL() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    uint8_t rs2 = instDecoded.registers[2];
    registers[rd] = static_cast<uint32_t>(registers[rs1]) << registers[rs2];
}
void CPU::SRL() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    uint8_t rs2 = instDecoded.registers[2];
    registers[rd] = static_cast<uint32_t>( registers[rs1] ) >> registers[rs2];
}
void CPU::SRA() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    uint8_t rs2 = instDecoded.registers[2];
    registers[rd] = registers[rs1] >> registers[rs2];
}
void CPU::SLT() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    uint8_t rs2 = instDecoded.registers[2];
    registers[rd] = (registers[rs1] < registers[rs2]) ? 1 : 0;
}
void CPU::SLTU() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    uint8_t rs2 = instDecoded.registers[2];
    registers[rd] = (static_cast<uint32_t>(registers[rs1]) < static_cast<uint32_t>(registers[rs2])) ? 1 : 0;
}

// I format
void CPU::ADDI() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    int32_t inmediate = instDecoded.inmediate;

    registers[rd] = registers[rs1] + inmediate;
}
void CPU::XORI() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    int32_t inmediate = instDecoded.inmediate;

    registers[rd] = registers[rs1] ^ inmediate;
}
void CPU::ORI() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    int32_t inmediate = instDecoded.inmediate;

    registers[rd] = registers[rs1] | inmediate;
}
void CPU::ANDI() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    int32_t inmediate = instDecoded.inmediate;

    registers[rd] = registers[rs1] & inmediate;
}
void CPU::SLLI() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    int32_t inmediate = instDecoded.inmediate;

    registers[rd] = static_cast<uint32_t>(registers[rs1]) << (static_cast<uint32_t>(inmediate) & 0b11111);
}
void CPU::SRLI() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    int32_t inmediate = instDecoded.inmediate;

    registers[rd] = static_cast<uint32_t>(registers[rs1]) >> (static_cast<uint32_t>(inmediate) & 0b11111);
}
void CPU::SRAI() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    int32_t inmediate = instDecoded.inmediate;

    registers[rd] = registers[rs1] >> (inmediate & 0b11111);
}
void CPU::SLTI() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    int32_t inmediate = instDecoded.inmediate;
    registers[rd] = (registers[rs1] < inmediate) ? 1 : 0;
}
void CPU::SLTIU() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    uint8_t inmediate = instDecoded.inmediate;
    registers[rd] = (static_cast<uint32_t>(registers[rs1]) < static_cast<uint32_t>(inmediate)) ? 1 : 0;
}
// TODO: PENSAR SI, POR EJEMPLO, EN LA POSICIÓN 1 HAY EL NUMERO 0X0000FFFF,
//       SI RECOJO UN BYTE, ES DECIR FF, QUÉ SUCEDE CON LOS OTROS 3 BYTES,
//       SE RELLENAN A 1 PORQUE TOMA COMO SIGNO EL BIT NÚMERO 8 O NO?
void CPU::LB() {
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
}
void CPU::LH() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    int32_t inmediate = instDecoded.inmediate;
    int16_t half = FlipHalf(ram->readHalf(registers[rs1] + inmediate));
    registers[rd] = half;
}
void CPU::LW() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    int32_t inmediate = instDecoded.inmediate;
    registers[rd] = FlipWord(ram->readWord(registers[rs1] + inmediate));
}
void CPU::LBU() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    int32_t inmediate = instDecoded.inmediate;

    // En este caso, al ser un unsigned, con hacer el AND ya
    // saca el valor adecuado

    registers[rd] = (ram->readByte(registers[rs1] + inmediate)) & 0xFF;
}
void CPU::LHU() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    int32_t inmediate = instDecoded.inmediate;

    // En este caso, al ser un unsigned, con hacer el AND ya
    // saca el valor adecuado

    registers[rd] = FlipHalf(ram->readHalf(registers[rs1] + inmediate));
}

void CPU::JALR() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    uint32_t inmediate = instDecoded.inmediate;

    // La dirección que tocaría si no se hiciera el salto se guarda en rd
    // para saltar más adelante de vuelta

    registers[rd] = pc;
    pc = registers[rs1] + inmediate;
}

void CPU::ECALL() {
    // Detener ejecución de programa
}
void CPU::EBREAK() {
    bEbreak = true;
}

// S format
void CPU::SB() {
    uint8_t rs1 = instDecoded.registers[0];
    uint8_t rs2 = instDecoded.registers[1];
    uint32_t inmediate = instDecoded.inmediate;

    uint8_t toStore = (registers[rs2] & 0xFF);

    ram->writeByte(registers[rs1] + inmediate, toStore);
}
void CPU::SH() {
    uint8_t rs1 = instDecoded.registers[0];
    uint8_t rs2 = instDecoded.registers[1];
    uint32_t inmediate = instDecoded.inmediate;

    uint16_t toStore = (registers[rs2] & 0xFFFF);
    toStore = FlipHalf(toStore);

    ram->writeHalf(registers[rs1] + inmediate, toStore);
}
void CPU::SW() {
    uint8_t rs1 = instDecoded.registers[0];
    uint8_t rs2 = instDecoded.registers[1];
    uint32_t inmediate = instDecoded.inmediate;

    uint32_t toStore = FlipWord(registers[rs2]);

    ram->writeWord(registers[rs1] + inmediate, toStore);
}

// B format
void CPU::BEQ() {
    uint8_t rs1 = instDecoded.registers[0];
    uint8_t rs2 = instDecoded.registers[1];
    uint32_t inmediate = instDecoded.inmediate;

    if (registers[rs1] == registers[rs2])
        pc += inmediate;
}
void CPU::BNE() {
    uint8_t rs1 = instDecoded.registers[0];
    uint8_t rs2 = instDecoded.registers[1];
    uint32_t inmediate = instDecoded.inmediate;

    if (registers[rs1] != registers[rs2])
        pc += inmediate;
}
void CPU::BLT() {
    uint8_t rs1 = instDecoded.registers[0];
    uint8_t rs2 = instDecoded.registers[1];
    uint32_t inmediate = instDecoded.inmediate;

    if (registers[rs1] < registers[rs2])
        pc += inmediate;
}
void CPU::BGE() {
    uint8_t rs1 = instDecoded.registers[0];
    uint8_t rs2 = instDecoded.registers[1];
    uint32_t inmediate = instDecoded.inmediate;

    if (registers[rs1] >= registers[rs2])
        pc += inmediate;
}
void CPU::BLTU() {
    uint8_t rs1 = instDecoded.registers[0];
    uint8_t rs2 = instDecoded.registers[1];
    uint32_t inmediate = instDecoded.inmediate;

    if (static_cast<uint32_t>(registers[rs1]) < static_cast<uint32_t>(registers[rs2]))
        pc += inmediate;
}
void CPU::BGEU() {
    uint8_t rs1 = instDecoded.registers[0];
    uint8_t rs2 = instDecoded.registers[1];
    uint32_t inmediate = instDecoded.inmediate;

    if (static_cast<uint32_t>(registers[rs1]) <= static_cast<uint32_t>(registers[rs2]))
        pc += inmediate;
}

// J format
void CPU::JAL() {
    uint8_t rd = instDecoded.registers[0];
    uint32_t inmediate = instDecoded.inmediate;

    // La dirección que tocaría si no se hiciera el salto se guarda en rd
    // para saltar más adelante de vuelta

    registers[rd] = pc + 4;
    pc += inmediate;
}

// U format
void CPU::LUI() {
    uint8_t rd = instDecoded.registers[0];
    uint32_t inmediate = instDecoded.inmediate;

    registers[rd] = inmediate << 12;
}
void CPU::AUIPC() {
    uint8_t rd = instDecoded.registers[0];
    uint32_t inmediate = instDecoded.inmediate;

    registers[rd] = pc + (inmediate << 12);
}
