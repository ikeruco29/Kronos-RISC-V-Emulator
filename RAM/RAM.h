#pragma once
#include <cstdint>

class RAM {
    public:
        RAM(int RAM_SIZE);
        ~RAM();

        int32_t *ram;

        void write(uint32_t addr, int32_t data);
        int32_t read(uint32_t addr);
};