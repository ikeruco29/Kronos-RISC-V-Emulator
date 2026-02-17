#ifndef EXECUTIONCAMPIGNDIALOG_H
#define EXECUTIONCAMPIGNDIALOG_H

#include <QDialog>

namespace Ui {
class ExecutionCampignDialog;
}

class ExecutionCampignDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExecutionCampignDialog(QWidget *parent = nullptr, int maxInstructions = 0);
    ~ExecutionCampignDialog();

    void updateProgressBar(int value);

private:
    Ui::ExecutionCampignDialog *ui;
    int *value;
    int finishValue;
};

#endif // EXECUTIONCAMPIGNDIALOG_H
