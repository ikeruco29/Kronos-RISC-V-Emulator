#ifndef INSTRUCTIONSDIALOG_H
#define INSTRUCTIONSDIALOG_H

#include <QDialog>
#include "computer.h"

namespace Ui {

enum Operation
{
    // Formato R
    ADD, SUB, XOR, OR, AND, SLL, SRL, SRA, SLT, SLTU,

    // Formato I
    ADDI, XORI, ORI, ANDI,
    SLLI, SRLI, SRAI, SLTI, SLTIU,
    LB, LH, LW, LBU, LHU,

    JALR,
    ECALL, EBREAK,

    // Formato S
    SB, SH, SW,

    // Formato B
    BEQ, BNE, BLT, BGE, BLTU,
    BGEU,

    // Formato J
    JAL,

    // Formato U
    LUI, AUIPC,

    NOP
};

class InstructionsDialog;
}

class InstructionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InstructionsDialog(QWidget *parent = nullptr, Computer *pComp = nullptr);
    ~InstructionsDialog();

private:
    Ui::InstructionsDialog *ui;
    Computer *pComputer;
};

#endif // INSTRUCTIONSDIALOG_H
