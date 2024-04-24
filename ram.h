#ifndef RAM_H
#define RAM_H

#include <cstdint>

class RAM {
public:
    RAM(uint32_t RAM_SIZE);
    ~RAM();

    uint8_t *ram;

    void writeByte(uint32_t addr, int8_t data);
    void writeHalf(uint32_t addr, int16_t data);
    void writeWord(uint32_t addr, int32_t data);

    // Lee un byte de memoria
    uint8_t readByte(uint32_t addr);
    // Lee 16 bits de la memoria y lo devuelve
    uint16_t readHalf(uint32_t addr);
    // Lee 32 bits de la memoria y lo devuelve como uint32_t
    uint32_t readWord(uint32_t addr);

    //void showRam();
};

#endif // RAM_H
