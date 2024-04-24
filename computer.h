#ifndef COMPUTER_H
#define COMPUTER_H

#include "cpu.h"
#include "ram.h"
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

#endif // COMPUTER_H
