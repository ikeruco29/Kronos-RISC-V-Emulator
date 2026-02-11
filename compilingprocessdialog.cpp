#include "compilingprocessdialog.h"
#include "ui_compilingprocessdialog.h"

compilingProcessDialog::compilingProcessDialog(QWidget *parent, const char *cmdString)
    : QDialog(parent)
    , ui(new Ui::compilingProcessDialog)
{
    ui->setupUi(this);
    ui->label->setText("cmdString");
}

compilingProcessDialog::~compilingProcessDialog()
{
    delete ui;
}
