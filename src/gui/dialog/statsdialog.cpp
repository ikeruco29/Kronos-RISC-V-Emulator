#include "gui/dialog/statsdialog.h"
#include "ui/dialog/ui_statsdialog.h"
#include <QFileInfo>

StatsDialog::StatsDialog(QWidget *parent, Computer *pComputer, uint32_t resultLocation)
    : QDialog(parent)
    , ui(new Ui::StatsDialog)
    , pComputer(pComputer)
    , resLocation(resultLocation)
{
    ui->setupUi(this);

    QFileInfo fileInfo(QString::fromStdString(pComputer->programName));

    ui->filenameText->setText("Program: " + fileInfo.fileName());
    ui->cyclesText->setText("Cycles executed: " + QString::number(pComputer->cpu.cycles));
    ui->arquitecturaText->setText("Architecture: RV32I");
    ui->resultText->setText("Program Result: " + QString::number(pComputer->ram.readByte(resLocation)));

    int ciclosR, ciclosI, ciclosS, ciclosB, ciclosU, ciclosJ;

    ui->tipoRText->setText("Type R: " + QString::number(pComputer->cpu.ciclosTipo[0]));
    ui->tipoIText->setText("Type I: " + QString::number(pComputer->cpu.ciclosTipo[1]));
    ui->tipoSText->setText("Type S: " + QString::number(pComputer->cpu.ciclosTipo[2]));
    ui->tipoBText->setText("Type B: " + QString::number(pComputer->cpu.ciclosTipo[3]));
    ui->tipoUText->setText("Type U: " + QString::number(pComputer->cpu.ciclosTipo[4]));
    ui->tipoJText->setText("Type J: " + QString::number(pComputer->cpu.ciclosTipo[5]));
}

StatsDialog::~StatsDialog()
{
    delete ui;
}

void StatsDialog::on_pushButton_clicked()
{
    pInstDialog = new InstructionsDialog(nullptr, pComputer);
    pInstDialog->exec();
}
