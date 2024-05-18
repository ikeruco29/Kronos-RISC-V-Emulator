#include "instructionsdialog.h"
#include "ui_instructionsdialog.h"

InstructionsDialog::InstructionsDialog(QWidget *parent, Computer *pComp)
    : QDialog(parent)
    , ui(new Ui::InstructionsDialog)
    , pComputer(pComp)
{
    ui->setupUi(this);

}

InstructionsDialog::~InstructionsDialog()
{
    delete ui;
}
