#include "CPU.h"
#include <iostream>


CPU::CPU(RAM *ram){
    // Inicialización de los registros
    for (int i = 0; i < 32; i++)
    {
        registers[i] = 0x00000000;
    }
    this->ram = ram;
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
        case Operation::SB:
            SB();
            break;
        case Operation::SH:
            SH();
            break;
        case Operation::SW:
            SW();
            break;

        //Formato B
        case Operation::BEQ:
            BEQ();
            break;
        case Operation::BNE:
            BNE();
            break;
        case Operation::BLT:
            BLT();
            break;
        case Operation::BGE:
            BGE();
            break;
        case Operation::BLTU:
            BLTU();
            break;
        case Operation::BGEU:
            BGEU();
            break;

        // Formato U
        case Operation::LUI:
            LUI();
            break;
        case Operation::AUIPC:
            AUIPC();
            break;

        // Formato J
        case Operation::JAL:
            JAL();
            break;

        default:
            break;
    }
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
    registers[rd] = (static_cast<uint32_t>(registers[rs1]) < static_cast<uint32_t>(registers[rs1])) ? 1 : 0;
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
    int8_t byte = (ram->read(registers[rs1] + inmediate)) & 0xFF;

    registers[rd] = byte;
}
void CPU::LH() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    int32_t inmediate = instDecoded.inmediate;
    int16_t half = (ram->read(registers[rs1] + inmediate)) & 0xFFFF;
    registers[rd] = half;
}
void CPU::LW() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    int32_t inmediate = instDecoded.inmediate;
    registers[rd] = ram->read(registers[rs1] + inmediate);
}
void CPU::LBU() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    int32_t inmediate = instDecoded.inmediate;

    // En este caso, al ser un unsigned, con hacer el AND ya 
    // saca el valor adecuado

    registers[rd] = (ram->read(registers[rs1] + inmediate)) & 0xFF;
}
void CPU::LHU() {
    uint8_t rd = instDecoded.registers[0];
    uint8_t rs1 = instDecoded.registers[1];
    int32_t inmediate = instDecoded.inmediate;

    // En este caso, al ser un unsigned, con hacer el AND ya 
    // saca el valor adecuado

    registers[rd] = (ram->read(registers[rs1] + inmediate)) & 0xFFFF;
}

void CPU::JALR() {}

void CPU::ECALL() {}
void CPU::EBREAK() {}

// S format
void CPU::SB() {}
void CPU::SH() {}
void CPU::SW() {}

// B format
void CPU::BEQ() {}
void CPU::BNE() {}
void CPU::BLT() {}
void CPU::BGE() {}
void CPU::BLTU() {}
void CPU::BGEU() {}

// J format
void CPU::JAL() {}

// U format
void CPU::LUI() {}
void CPU::AUIPC() {}
