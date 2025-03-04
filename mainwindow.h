#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "computer.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr, Computer *comp = nullptr);
    ~MainWindow();
    int RAM_SIZE;
    Computer *computer;

private slots:
    void on_actionCargar_programa_triggered();

    void on_actionCargar_campa_a_triggered();

    void on_actionSalir_triggered();

    void on_runButton_clicked();

    void on_stopButton_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
