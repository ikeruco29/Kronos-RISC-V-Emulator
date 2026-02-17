#ifndef COMPILINGPROCESSDIALOG_H
#define COMPILINGPROCESSDIALOG_H

#include <QDialog>
#include <qmessagebox.h>

namespace Ui {
class compilingProcessDialog;
}

class compilingProcessDialog : public QDialog
{
    Q_OBJECT

public:
    explicit compilingProcessDialog(QWidget *parent = nullptr, const char *cmdString = "");
    ~compilingProcessDialog();

private:
    Ui::compilingProcessDialog *ui;
};

#endif // COMPILINGPROCESSDIALOG_H
