#include "instructionsdialog.h"
#include "ui_instructionsdialog.h"

InstructionsDialog::InstructionsDialog(QWidget *parent, Computer *pComp)
    : QDialog(parent)
    , ui(new Ui::InstructionsDialog)
    , pComputer(pComp)
{
    ui->setupUi(this);
    ui->addNumText->setText(QString::number(pComputer->cpu.ciclosTotales[ADD]));
    ui->subNumText->setText(QString::number(pComputer->cpu.ciclosTotales[SUB]));
    ui->xorNumText->setText(QString::number(pComputer->cpu.ciclosTotales[XOR]));
    ui->orNumText->setText(QString::number(pComputer->cpu.ciclosTotales[OR]));
    ui->andNumText->setText(QString::number(pComputer->cpu.ciclosTotales[AND]));
    ui->sllNumText->setText(QString::number(pComputer->cpu.ciclosTotales[SLL]));
    ui->srlNumText->setText(QString::number(pComputer->cpu.ciclosTotales[SRL]));
    ui->sraNumText->setText(QString::number(pComputer->cpu.ciclosTotales[SRA]));
    ui->sltNumText->setText(QString::number(pComputer->cpu.ciclosTotales[SLT]));
    ui->sltuNumText->setText(QString::number(pComputer->cpu.ciclosTotales[SLTU]));

    ui->addiNumText->setText(QString::number(pComputer->cpu.ciclosTotales[ADDI]));
    ui->xoriNumText->setText(QString::number(pComputer->cpu.ciclosTotales[XORI]));
    ui->oriNumText->setText(QString::number(pComputer->cpu.ciclosTotales[ORI]));
    ui->andiNumText->setText(QString::number(pComputer->cpu.ciclosTotales[ANDI]));
    ui->slliNumText->setText(QString::number(pComputer->cpu.ciclosTotales[SLLI]));
    ui->srliNumText->setText(QString::number(pComputer->cpu.ciclosTotales[SRLI]));
    ui->sraiNumText->setText(QString::number(pComputer->cpu.ciclosTotales[SRAI]));
    ui->sltiNumText->setText(QString::number(pComputer->cpu.ciclosTotales[SLTI]));
    ui->sltiuNumText->setText(QString::number(pComputer->cpu.ciclosTotales[SLTIU]));

    ui->lbNumText->setText(QString::number(pComputer->cpu.ciclosTotales[LB]));
    ui->lhNumText->setText(QString::number(pComputer->cpu.ciclosTotales[LH]));
    ui->lwNumText->setText(QString::number(pComputer->cpu.ciclosTotales[LW]));
    ui->lbuNumText->setText(QString::number(pComputer->cpu.ciclosTotales[LBU]));
    ui->lhuNumText->setText(QString::number(pComputer->cpu.ciclosTotales[LHU]));

    ui->sbNumText->setText(QString::number(pComputer->cpu.ciclosTotales[SB]));
    ui->shNumText->setText(QString::number(pComputer->cpu.ciclosTotales[SH]));
    ui->swNumText->setText(QString::number(pComputer->cpu.ciclosTotales[SW]));

    ui->beqNumText->setText(QString::number(pComputer->cpu.ciclosTotales[BEQ]));
    ui->bneNumText->setText(QString::number(pComputer->cpu.ciclosTotales[BNE]));
    ui->bltNumText->setText(QString::number(pComputer->cpu.ciclosTotales[BLT]));
    ui->bgeNumText->setText(QString::number(pComputer->cpu.ciclosTotales[BGE]));
    ui->bltuNumText->setText(QString::number(pComputer->cpu.ciclosTotales[BLTU]));
    ui->bgeuNumText->setText(QString::number(pComputer->cpu.ciclosTotales[BGEU]));

    ui->jalNumText->setText(QString::number(pComputer->cpu.ciclosTotales[JAL]));
    ui->jalrNumText->setText(QString::number(pComputer->cpu.ciclosTotales[JALR]));

    ui->luiNumText->setText(QString::number(pComputer->cpu.ciclosTotales[LUI]));
    ui->auipcNumText->setText(QString::number(pComputer->cpu.ciclosTotales[AUIPC]));

    ui->ecallNumText->setText(QString::number(pComputer->cpu.ciclosTotales[ECALL]));
    ui->ebreakNumText->setText(QString::number(pComputer->cpu.ciclosTotales[EBREAK]));
}

InstructionsDialog::~InstructionsDialog()
{
    delete ui;
}
