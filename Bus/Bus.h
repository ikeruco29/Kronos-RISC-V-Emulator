#pragma once

#include "../CPU/CPU.h"
#include "../RAM/RAM.h"

class Bus
{
    public:
        Bus();
        ~Bus();

        RAM *ram;
        CPU *cpu;

        void memWrite(uint32_t addr, int32_t data);
        uint32_t memRead(uint32_t addr);
};
