#ifndef INSTRUCTIONSDIALOG_H
#define INSTRUCTIONSDIALOG_H

#include <QDialog>
#include "computer.h"

namespace Ui {
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
