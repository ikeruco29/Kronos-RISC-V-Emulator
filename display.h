#ifndef DISPLAY_H
#define DISPLAY_H

#include <QMainWindow>
#include "computer.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Display;
}
QT_END_NAMESPACE

class Display : public QMainWindow
{
    Q_OBJECT

public:
    Display(QWidget *parent = nullptr, Computer *computer = nullptr);
    ~Display();
    Computer *computer;

private slots:
    void on_actionSalir_triggered();

    void on_actionCargar_programa_triggered();

    void on_actionCargar_campa_a_triggered();

    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:
    Ui::Display *ui;
};
#endif // DISPLAY_H
