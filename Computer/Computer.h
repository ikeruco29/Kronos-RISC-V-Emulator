#pragma once
#include "../CPU/CPU.h"
#include "../RAM/RAM.h"

class Computer {
    public:
        Computer(int RAM_SIZE);
        ~Computer();

        CPU cpu;
        RAM ram;
};