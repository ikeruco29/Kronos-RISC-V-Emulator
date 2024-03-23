#include "RAM.h"

RAM::RAM(int RAM_SIZE){
    ram = new uint32_t[RAM_SIZE * 1024];

    for(int i = 0; i < RAM_SIZE * 1024; i++){
        ram[i] = 0x00000;
    }
};


RAM::~RAM(){};

void RAM::write(uint32_t addr, int32_t data){
    ram[addr] = data;
};

uint32_t RAM::read(uint32_t addr){
    return ram[addr];
};