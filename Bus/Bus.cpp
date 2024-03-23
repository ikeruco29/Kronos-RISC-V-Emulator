#include "Bus.h"

void Bus::memWrite(uint32_t addr, int32_t data){
    ram->write(addr, data);
}

uint32_t Bus::memRead(uint32_t addr){
    return ram->read(addr);
}