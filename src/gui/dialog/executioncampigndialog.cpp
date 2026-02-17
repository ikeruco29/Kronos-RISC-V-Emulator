#include "gui/dialog/executioncampigndialog.h"
#include "ui/dialog/ui_executioncampigndialog.h"

ExecutionCampignDialog::ExecutionCampignDialog(QWidget *parent, int maxInstructions)
    : QDialog(parent)
    , ui(new Ui::ExecutionCampignDialog)
    , finishValue(maxInstructions)
{
    ui->setupUi(this);
    ui->progressBar->setMaximum(finishValue);
    ui->progressBar->setValue(0);
}

ExecutionCampignDialog::~ExecutionCampignDialog()
{
    delete ui;
}

void ExecutionCampignDialog::updateProgressBar(int value){
    ui->progressBar->setValue(value);
}
