#ifndef COMPUTER_H
#define COMPUTER_H

#include "cpu.h"
#include "memory.h"
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
    Memory ram;

    // Variables para las campañas
    Campaign campaign;
    std::string programName;

    QTextEdit *terminalBox;
    QPlainTextEdit *ramBox;

    uint32_t ram_size;

    void reset();
    int LoadProgram(std::string filename);
    int LoadCampaign(std::string filename);
    int executeCampaign();
    std::string showRam(int page = 0);
    std::string showRegisters();
    std::string showDisassembly();
    std::string exportDisassembly();

    QString showVRAMLine(int line);


};

#endif // COMPUTER_H
