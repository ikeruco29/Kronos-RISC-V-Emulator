#include "statsdialog.h"
#include "ui_statsdialog.h"
#include <QFileInfo>

StatsDialog::StatsDialog(QWidget *parent, Computer *pComputer, uint32_t resultLocation)
    : QDialog(parent)
    , ui(new Ui::StatsDialog)
    , pComputer(pComputer)
    , resLocation(resultLocation)
{
    ui->setupUi(this);

    QFileInfo fileInfo(QString::fromStdString(pComputer->programName));

    ui->filenameText->setText("Programa: " + fileInfo.fileName());
    ui->cyclesText->setText("Número de ciclos ejecutados: " + QString::number(pComputer->cpu.cycles));
    ui->arquitecturaText->setText("Arquitectura: RV32I");
    ui->resultText->setText("Resultado del programa: " + QString::number(pComputer->ram.readByte(resLocation)));

    int ciclosR, ciclosI, ciclosS, ciclosB, ciclosU, ciclosJ;

    ui->tipoRText->setText("Tipo R: " + QString::number(pComputer->cpu.ciclosTipo[0]));
    ui->tipoIText->setText("Tipo I: " + QString::number(pComputer->cpu.ciclosTipo[1]));
    ui->tipoSText->setText("Tipo S: " + QString::number(pComputer->cpu.ciclosTipo[2]));
    ui->tipoBText->setText("Tipo B: " + QString::number(pComputer->cpu.ciclosTipo[3]));
    ui->tipoUText->setText("Tipo U: " + QString::number(pComputer->cpu.ciclosTipo[4]));
    ui->tipoJText->setText("Tipo J: " + QString::number(pComputer->cpu.ciclosTipo[5]));
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
