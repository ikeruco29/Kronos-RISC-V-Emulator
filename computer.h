#ifndef COMPUTER_H
#define COMPUTER_H

#include "cpu.h"
#include "ram.h"
#include <fstream>
#include <iostream>
#include <QTextEdit>

class Computer {
public:
    Computer(int RAM_SIZE);
    Computer(int RAM_SIZE, QTextEdit *termb);
    ~Computer();

    CPU cpu;
    RAM ram;

    QTextEdit *rambox;
    QTextEdit *disBox;
    QTextEdit *regBox;
    QTextEdit *terminalBox;

    int ram_size;

    void On(bool *pasoapaso);
    int LoadProgram(std::string filename);
    std::string exportRam();
};

#endif // COMPUTER_H
