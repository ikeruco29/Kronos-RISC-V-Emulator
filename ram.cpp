#include "ram.h"

RAM::RAM(uint32_t RAM_SIZE){
    iRamSize = RAM_SIZE;
    ram = new uint8_t[RAM_SIZE];

    for(uint32_t i = 0; i < RAM_SIZE; i++){
        ram[i] = 0xFF;
    }

    this->uStartAddr = 0;
};


RAM::~RAM(){};

void RAM::writeByte(uint32_t addr, int8_t data) {
    ram[addr - uStartAddr] = data;
}
void RAM::writeHalf(uint32_t addr, int16_t data) {
    ram[addr - uStartAddr] = data >> 8;
    ram[addr - uStartAddr + 1] = data;
}
void RAM::writeWord(uint32_t addr, int32_t data){

    // 00 00 00 00
    ram[addr - uStartAddr] = data >> 24;
    ram[addr - uStartAddr + 1] = data >> 16;
    ram[addr - uStartAddr + 2] = data >> 8;
    ram[addr - uStartAddr + 3] = data;
};

uint8_t RAM::readByte(uint32_t addr) {
    if(addr <= iRamSize && addr >= 0)
        return ram[addr - uStartAddr];
    else
        return 0;
}
uint16_t RAM::readHalf(uint32_t addr) {
    if(addr <= iRamSize && addr >= 0){
        uint16_t half = (ram[addr - uStartAddr + 2] << 8) | ram[addr - uStartAddr + 3];
        return half;
    }else{
        return 0;
    }

}
uint32_t RAM::readWord(uint32_t addr){
    if(addr <= iRamSize && addr >= 0){
        uint32_t word = (ram[addr - uStartAddr] << 24) | (ram[addr - uStartAddr + 1] << 16) | (ram[addr - uStartAddr + 2] << 8) | ram[addr - uStartAddr + 3];
        return word;
    }else
        return 0;
};

void RAM::reset(){
    for(uint32_t i = 0; i < iRamSize; i++){
        ram[i] = 0xFF;
    }
}

