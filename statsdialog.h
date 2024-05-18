#ifndef STATSDIALOG_H
#define STATSDIALOG_H

#include <QDialog>
#include "computer.h"

namespace Ui {
class StatsDialog;
}

class StatsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StatsDialog(QWidget *parent = nullptr, Computer *pComputer = nullptr, uint32_t resultLocation = 0);
    ~StatsDialog();

private:
    Ui::StatsDialog *ui;
    Computer *pComputer;

public:
    uint32_t resLocation;
private slots:
    void on_pushButton_clicked();
};

#endif // STATSDIALOG_H
