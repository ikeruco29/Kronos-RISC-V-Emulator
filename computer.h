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

    int ram_size;

    void On(bool pasoapaso = false);
    int LoadProgram(std::string filename);
    std::string exportRam();
};

#endif // COMPUTER_H
