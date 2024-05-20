#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "computer.h"
#include "statsdialog.h"

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
    QString campaignGeneratorRoute;

    uint32_t FINISH_LOCATION, RESULT_LOCATION;

private slots:
    void on_actionCargar_programa_triggered();

    void on_actionCargar_campa_a_triggered();

    void on_actionSalir_triggered();

    int on_runButton_clicked();

    void on_stopButton_clicked();

    void on_runPasoButton_clicked();

    void on_searchBox_editingFinished();

    void on_openConfigButton_clicked();

    void runLoopIteration();

    void on_generateStatsButton_clicked();

    void on_exportDisButton_clicked();

    void on_exportRamButton_clicked();

    void on_pauseButton_clicked();

    void on_actionGenerar_campa_a_aleatoria_triggered();

    void on_executeCampaignButton_clicked();

    void runLoopIterationCampaign();
    void on_loadCampaignButton_clicked();

signals:
    void runProgram();
    void runProgramCompleted();

private:
    Ui::MainWindow *ui;
    StatsDialog *statsDialog;

    void resetInterface();

    bool stopExec;
    bool isExecutingBeforeCampaign;

    void UpdateInterface();

    void loadCampaign();
    void updateCampaignAfterProgramExecution();

    void UpdateTerminal();
};
#endif // MAINWINDOW_H
