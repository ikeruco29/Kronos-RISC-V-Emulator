#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "gui/dialog/compilingprocessdialog.h"
#include "computer.h"
#include "gui/dialog/executioncampigndialog.h"
#include "gui/dialog/statsdialog.h"

#include "gui/editor/highlighter.h"

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

    // Config vars
    QString disassemblyFileRoute;
    QString ramFileRoute;
    QString campaignGeneratorRoute;
    QString linkerRoute;

    std::vector<int> campaignResults;
    int injectionNumber = 0;

    uint32_t FINISH_LOCATION, RESULT_LOCATION;
    bool updateRamInRealTime;

    struct EditorConfig editorConfig;

    Highlighter *highlighter;


private slots:
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

    void on_actionLoadProgram_triggered();

    void on_actionLoadCampaign_triggered();

    void on_actionExit_triggered();

    void on_actionGenerateRandomCampaign_triggered();

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

    QString programFileName = "";

    void resetInterface();

    bool stopExec;
    bool isExecutingBeforeCampaign;

    void UpdateInterface();

    void loadCampaign();
    void updateCampaignAfterProgramExecution();

    void updateFilenameText(bool isChanged = false);
    void resetFilenameText(QString text = "");
};
#endif // MAINWINDOW_H
