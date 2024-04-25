#ifndef COMPUTER_H
#define COMPUTER_H

#include "cpu.h"
#include "ram.h"
#include <fstream>
#include <iostream>
#include <QTextEdit>
#include <QPlainTextEdit>

struct Campaign {
    QString programPath;
    int expectedResult;
    int expectedInstructions;
    std::vector<std::vector<int>> injections;
};

class Computer {
public:
    Computer(int RAM_SIZE);
    Computer(int RAM_SIZE, QTextEdit *termb);
    ~Computer();

    CPU cpu;
    RAM ram;

    // Variables para las campañas
    Campaign campaign;

    QTextEdit *terminalBox;
    QPlainTextEdit *ramBox;

    int ram_size;

    void executeProgram();
    int LoadProgram(std::string filename);
    int LoadCampaign(std::string filename);
    int executeCampaign();
    std::string showRam(int page = 0);
    std::string showRegisters();
};

#endif // COMPUTER_H
