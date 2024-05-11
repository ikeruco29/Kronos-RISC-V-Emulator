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
    Computer *computer;
    uint pageToView = 0;

    // Variables de configuración
    QString disassemblyFileRoute;
    QString ramFileRoute;


private slots:
    void on_actionCargar_programa_triggered();

    void on_actionCargar_campa_a_triggered();

    void on_actionSalir_triggered();

    void on_runButton_clicked();

    void on_stopButton_clicked();

    void on_runPasoButton_clicked();

    void on_searchBox_editingFinished();

    void on_openConfigButton_clicked();

    void runLoopIteration();

    void on_generateStatsButton_clicked();

    void on_exportDisButton_clicked();

    void on_exportRamButton_clicked();

private:
    Ui::MainWindow *ui;

    void resetInterface();

    bool stopExec;


};
#endif // MAINWINDOW_H
