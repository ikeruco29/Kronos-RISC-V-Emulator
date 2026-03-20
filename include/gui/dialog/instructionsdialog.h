#ifndef INSTRUCTIONSDIALOG_H
#define INSTRUCTIONSDIALOG_H

#include <QDialog>
#include "computer.h"

namespace Ui {

enum Operation
{
    // R format
    ADD, SUB, XOR, OR, AND, SLL, SRL, SRA, SLT, SLTU,

    // I format
    ADDI, XORI, ORI, ANDI,
    SLLI, SRLI, SRAI, SLTI, SLTIU,
    LB, LH, LW, LBU, LHU,

    JALR,
    ECALL, EBREAK,

    // S format
    SB, SH, SW,

    // B format
    BEQ, BNE, BLT, BGE, BLTU,
    BGEU,

    // J format
    JAL,

    // U format
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
