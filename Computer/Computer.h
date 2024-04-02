#pragma once
#include "../CPU/CPU.h"
#include "../RAM/RAM.h"
#include <fstream>
#include <iostream>

class Computer {
    public:
        Computer(int RAM_SIZE);
        ~Computer();

        CPU cpu;
        RAM ram;

        void On(int cycles);
        int LoadProgram(std::string filename);
};