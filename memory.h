#ifndef MEMORY_H
#define MEMORY_H

#include <cstdint>

class Memory {
public:
    Memory(uint32_t MEMORY_SIZE);
    ~Memory();

    uint32_t iMemorySize;
    uint32_t iRomSize, iRomStartAddr;
    int iDataSize;

    uint8_t *memory;
    uint32_t pIo = 1024*1024; // 1500 son los caracteres que caben en la pantalla

    void writeByte(uint32_t addr, int8_t data);
    void writeHalf(uint32_t addr, int16_t data);
    void writeWord(uint32_t addr, int32_t data);

    // Lee un byte de memoria
    uint8_t readByte(uint32_t addr);
    // Lee 16 bits de la memoria y lo devuelve
    uint16_t readHalf(uint32_t addr);
    // Lee 32 bits de la memoria y lo devuelve como uint32_t
    uint32_t readWord(uint32_t addr);

    void reset();
    void resetMemorySection(uint32_t inicio, uint32_t fin);

    void resetIOMemory();
};

#endif // MEMORY_H
