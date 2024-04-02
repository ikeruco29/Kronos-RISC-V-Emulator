#include "RAM.h"

RAM::RAM(int RAM_SIZE){
    ram = new uint8_t[RAM_SIZE * 1024];

    for(int i = 0; i < RAM_SIZE * 1024; i++){
        ram[i] = 0x00;
    }
};


RAM::~RAM(){};

void RAM::writeByte(uint32_t addr, int8_t data) {
    ram[addr] = data;
}
void RAM::writeHalf(uint32_t addr, int16_t data) {
    ram[addr] = data >> 8;
    ram[addr + 1] = data;
}
void RAM::writeWord(uint32_t addr, int32_t data){
    
    // 00 00 00 00
    ram[addr] = data >> 24;
    ram[addr + 1] = data >> 16;
    ram[addr + 2] = data >> 8;
    ram[addr + 3] = data;
};

uint8_t RAM::readByte(uint32_t addr) {
    return ram[addr];
}
uint16_t RAM::readHalf(uint32_t addr) {
    uint16_t half = (ram[addr + 2] << 8) | ram[addr + 3];
    return half;
}
uint32_t RAM::readWord(uint32_t addr){
    uint32_t word = (ram[addr] << 24) | (ram[addr + 1] << 16) | (ram[addr + 2] << 8) | ram[addr + 3];

    return word;
};

void RAM::showRam() {
    printf("         |  00 01 02 03\n-----------------------\n");
    printf("00000000 |  ");
    for (int i = 1; i <= 32 * 5; i++)
    {
        printf("%.2X ", readByte(i - 1));

        if (i % 4 == 0 && i != 0) {
            printf("\n");
            printf("%.8X |  ", 0x00000000 + i);
        }
    }
}