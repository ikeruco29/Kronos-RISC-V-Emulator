#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "compilingprocessdialog.h"
#include "computer.h"
#include "executioncampigndialog.h"
#include "statsdialog.h"

#include "highlighter.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

struct EditorConfig {
    int tabsize;
    QString font;
    int fontsize;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr, Computer *comp = nullptr, EditorConfig sEditorConfig = {});
    ~MainWindow();
    Computer *computer;
    uint pageToView = 0;

    // Variables de configuración
    QString disassemblyFileRoute;
    QString ramFileRoute;
    QString campaignGeneratorRoute;

    std::vector<int> campaignResults;
    int injectionNumber = 0;

    uint32_t FINISH_LOCATION, RESULT_LOCATION;
    bool updateRamInRealTime;

    struct EditorConfig editorConfig;

    Highlighter *highlighter;


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
    void iterationCampaign();

    void runLoopIterationCampaign();
    void on_loadCampaignButton_clicked();

    void onCampaignComplete();
    void onFinishIter();
    void on_actionOpen_File_triggered();

    void on_actionSave_file_triggered();

    void on_actionNew_triggered();

    void on_languageSelector_currentIndexChanged(int index);

signals:
    void runProgram();
    void runProgramCompleted();

    void runCampaignIter();

    void campaignIterComplete();
    void campaignComplete();

private:
    Ui::MainWindow *ui;
    StatsDialog *statsDialog;
    ExecutionCampignDialog *campaignDialog;
    compilingProcessDialog *compilingDialog;

    void resetInterface();

    bool stopExec;
    bool isExecutingBeforeCampaign;

    void UpdateInterface();

    void loadCampaign();
    void updateCampaignAfterProgramExecution();

    void UpdateTerminal();
};
#endif // MAINWINDOW_H
