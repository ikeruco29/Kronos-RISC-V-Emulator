#include "gui/mainwindow.h"
#include "ui/ui_mainwindow.h"
#include <cstdlib>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDesktopServices>
#include <QTimer>
#include <QDateTime>
#include <QInputDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

#include "gui/editor/codeeditor.h"


enum CampaignResult{
    NO_EFFECT,
    SDC,
    SED,
    DUE
};

CodeEditor *editor;

MainWindow::MainWindow(QWidget *parent, Computer *comp, struct EditorConfig sEditorConfig)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , computer(comp)
    , editorConfig(sEditorConfig)
{
    // This is to stablish certain interface values
    // For instance: "User can't use execute method at the start of the program"
    ui->setupUi(this);
    QString stsheet = "QPushButton:disabled {background-color: rgba(255, 255, 255, 0.1); }";
    ui->centralwidget->setStyleSheet(stsheet);
    ui->runButton->setEnabled(false);
    ui->runPasoButton->setEnabled(false);
    ui->pauseButton->setEnabled(false);
    ui->executeCampaignButton->setEnabled(false);
    ui->generateStatsButton->setEnabled(false);

    // ================ CODE EDITOR SETUP ==================

    editor = new CodeEditor(ui->CodeEditor->parentWidget(), sEditorConfig.tabsize);

    // Copy geometry and politics
    editor->setGeometry(ui->CodeEditor->geometry());
    editor->setSizePolicy(ui->CodeEditor->sizePolicy());

    QLayout *layout = ui->CodeEditor->parentWidget()->layout();
    layout->replaceWidget(ui->CodeEditor, editor);

    ui->CodeEditor->deleteLater();

    QFont font ("Consolas");
    font.setStyleHint(QFont::Monospace);
    font.setPointSize(sEditorConfig.fontsize);

    editor->setTabStopDistance(QFontMetrics(font).horizontalAdvance(' ') * sEditorConfig.tabsize);
    editor->setFont(font);

    highlighter = new Highlighter(editor->document());

    // =====================================================

    isExecutingBeforeCampaign = false;

    // In QT you can link a method to another. This other method is called "signal",
    // and you don't have to implement it. At the moment you "emit" them by using "emit signalName()",
    // Connected method is called in a separate thread, so the main thread doens't block (which is
    // the one that's managing the interface), and the user can use other buttons such as
    // pause or stop.
    connect(this, &MainWindow::runProgram, this, &MainWindow::on_runButton_clicked);
    connect(this, &MainWindow::runProgramCompleted, this, &MainWindow::updateCampaignAfterProgramExecution);
    connect(this, &MainWindow::runCampaignIter, this, &MainWindow::iterationCampaign);
    connect(this, &MainWindow::campaignComplete, this, &MainWindow::onCampaignComplete);
    connect(this, &MainWindow::campaignIterComplete, this, &MainWindow::onFinishIter);
    connect(editor->document(), &QTextDocument::modificationChanged, this, &MainWindow::updateFilenameText);

    QAction *saveAction = new QAction("Save", this);
    saveAction->setShortcut(QKeySequence("Ctrl+S"));
    connect(saveAction, &QAction::triggered, this, &MainWindow::on_actionSave_file_triggered);
    this->addAction(saveAction);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// adds an '*' if the text has been modified
void MainWindow::updateFilenameText(bool isChanged){
    QString pfn = QFileInfo(programFileName).fileName();

    if(isChanged && !ui->filenameText->text().contains("*")){
        pfn += " *";
        ui->filenameText->setText(pfn);
    } else if(ui->filenameText->text().contains("*")) {
        resetFilenameText(pfn);
    }
}

void MainWindow::resetFilenameText(QString text){
    ui->filenameText->setText(text.isEmpty() ? QFileInfo(programFileName).fileName() : text);
}

// Load desired program
void MainWindow::on_actionCargar_programa_triggered()
{

    // Opening file explorer to select a specific binary
    QString nombreArchivo = QFileDialog::getOpenFileName(this, "Select file", "", "*.bin *.o");
    if (!nombreArchivo.isEmpty()) {

        qDebug() << "File selected:" << nombreArchivo;   // debug
        QFileInfo fileInfo(nombreArchivo);
        QString filename = fileInfo.fileName();

        computer->reset();
        computer->LoadProgram(nombreArchivo.toStdString());

        resetInterface();

        pageToView = computer->ram.iRomStartAddr;   // For the RAM loader:
                                                    // I don't want to show all RAM
                                                    // just 8 rows inside textbox.

        ui->ramText->setPlainText(QString::fromStdString(computer->showRam(pageToView)));
        resetFilenameText(filename);


        // On program load, enable all execution buttons
        ui->runButton->setEnabled(true);
        ui->runPasoButton->setEnabled(true);
        ui->pauseButton->setEnabled(true);

    } else {
        qDebug() << "No file selected....";
    }

    stopExec = false;
}

// Load Campaign
void MainWindow::on_actionCargar_campa_a_triggered()
{
    loadCampaign();
}

// Close app
void MainWindow::on_actionSalir_triggered()
{
    qApp->quit();
}

// Complete execution buton
int MainWindow::on_runButton_clicked()
{
    // If the program loaded is not an executable program...
    if(!ui->filenameText->text().contains(".bin")){
        QFileInfo fileinfo = QFileInfo(programFileName);
        std::stringstream cmd;

        std::stringstream binPath;

        // If its an assembly file...
        if(ui->filenameText->text().contains(".s")){
            binPath << fileinfo.path().toStdString() << "/a.bin";
            cmd << "clang --target=riscv32 -march=rv32i -mabi=ilp32 -x assembler -c " << programFileName.toStdString() << " -o " << binPath.str();

        // If its a C file...
        }else if(ui->filenameText->text().contains(".c")){
            binPath << fileinfo.path().toStdString() << "/a.bin";
            cmd << "clang --target=riscv32 -march=rv32i -mabi=ilp32 -O0 -g -nostdlib -nostartfiles -fuse-ld=lld -Wl,-T,linker.ld " << programFileName.toStdString() << " -o " << binPath.str();

        }

        compilingDialog = new compilingProcessDialog(nullptr, cmd.str().c_str());
        compilingDialog->show();

        qDebug() << "Absolute path" << fileinfo.path();
        qDebug() << "CLANG Command: " << cmd.str().c_str();
        qDebug() << "Compiling...";

        int commandCode = std::system(cmd.str().c_str());

        compilingDialog->close();

        if( commandCode != 0) {
            QMessageBox::critical(nullptr, "Error: Could not compile the file", "An error ocurred while trying to compile the file");
            return 1;
        }


        computer->reset();
        computer->LoadProgram(binPath.str()); // Load program to computer's memory

        pageToView = computer->ram.iRomStartAddr;
        ui->ramText->setPlainText(QString::fromStdString(computer->showRam(pageToView)));
    }

    stopExec = false;

    // Todo esto del QTimer es porque si utilizo un bucle que haga
    // estas cuatro instrucciones:
    //
    //      computer->cpu.clock();
    //      ui->ramText->setPlainText(QString::fromStdString(computer->showRam(pageToView)));
    //      ui->registerText->setPlainText(QString::fromStdString(computer->showRegisters()));
    //      ui->codeDisassemblyText->appendPlainText(QString::fromStdString(computer->showDisassembly()));
    //
    // No le da tiempo a renderizar, así que necesito que espere a que termine la iteración
    // con el renderizado.
    // Realmente no pasa nada porque el emulador no tenga una velocidad vertiginosa
    // (que la tiene), ya que lo importante es que lleve bien el número de instrucciones

    QTimer *timer = new QTimer(this);

    // Conectar el timeout del QTimer al slot para ejecutar una iteración del bucle
    connect(timer, &QTimer::timeout, this, &MainWindow::runLoopIteration);

    timer->start();

    return 0;
}


// Do just one clock cycle (fetch, decode y execute)
void MainWindow::runLoopIteration()
{

    // When ECALL instruction is called, it means the program has finished
    if (computer->cpu.bEcall == true || stopExec) {

        sender()->deleteLater(); // Delete QTimer at the end of the loop

        // In case it is executing a campaign, continue executing
        if(this->isExecutingBeforeCampaign)
            emit runProgramCompleted();

        else if(computer->cpu.bEcall == true){

            ui->ramText->setPlainText(QString::fromStdString(computer->showRam(pageToView)));   // Update ramBox
            ui->generateStatsButton->setEnabled(true);
            QMessageBox::information(nullptr, "Execution completed", "The program execution has finished");

        }
        return;
    }

    // Clock cpu
    computer->cpu.clock();

    if(!this->isExecutingBeforeCampaign)    // Only update interface if its not a campaign
        this->UpdateInterface();
}

// Do one campaign cycle
void MainWindow::runLoopIterationCampaign()
{
    qDebug() << computer->cpu.cycles;

    if (computer->cpu.bEcall == true) {

        // When a 0 is written at FINISH_LOCATION, stop program execution
        if(computer->ram.readByte(RESULT_LOCATION) != computer->campaign.expectedResult){

            // Final result: Silent Data Corruption (SDC)
            this->campaignResults.push_back(SDC);

        }
        else if(computer->campaign.expectedInstructions > computer->cpu.cycles){

            // Final result: Single Event Delay (SED)
            this->campaignResults.push_back(SED);

        } else {

            // Final result: NO EFFECT
            this->campaignResults.push_back(NO_EFFECT);

        }

        this->injectionNumber++;    // current injection index

        sender()->deleteLater(); // Delete QTimer at the end of the loop
        emit campaignIterComplete();
        return;
    }


    // If the program didn't hold (x2 in total instructions executed), continue
    if(computer->campaign.expectedInstructions * 2 > computer->cpu.cycles){

        if(computer->cpu.cycles == this->injectionNumber){
            int inst = computer->cpu.cycles;    // instruction number
            int reg = computer->campaign.injections[inst][1];   // Register to change
            computer->cpu.registers[reg] ^= (1 << computer->campaign.injections[inst][2]); // Invert bit using XOR
        }

    }else{
        // Final result: Detected Unrecovery Error (DUE)
        this->campaignResults.push_back(DUE);

        this->injectionNumber++;

        sender()->deleteLater(); // Delete QTimer at the end of the loop

        emit campaignIterComplete();
        return;
    }

    // Execute one loop interation
    computer->cpu.clock();
}



// Reset button
void MainWindow::on_stopButton_clicked()
{
    stopExec = true;    // If there is a program execution going on, stop it
    computer->reset();
    resetInterface();
}

// Pause execution
void MainWindow::on_pauseButton_clicked()
{
    stopExec = true;
}

// One-step execution
void MainWindow::on_runPasoButton_clicked()
{
    if (computer->ram.readByte(FINISH_LOCATION) != 0) {
        computer->cpu.clock();
        this->UpdateInterface();
    } else {
        ui->ramText->setPlainText(QString::fromStdString(computer->showRam(pageToView)));   // Update ramBox
        QMessageBox::information(nullptr, "Execution completed", "The program execution has finished");
        ui->generateStatsButton->setEnabled(true);  // Enable Generate Statistics button
    }
}

// This method is executed when something is written inside the RAM search box
void MainWindow::on_searchBox_editingFinished()
{
    QString memoryToView = ui->searchBox->text();

    int searchBoxInt = memoryToView.toUInt(nullptr, 16);

    pageToView = searchBoxInt;

    int byteMem = pageToView & 0xF;
    int rowMem = (pageToView >> 4) % 8; // Take the row (% 8 cause it only shows 8 rows)

    pageToView = (pageToView & 0xFFFFFF80); // Cause there are only 8 rows...

    ui->ramText->setPlainText(QString::fromStdString(computer->showRam(pageToView)));

    // Lo pongo en un if para que si no escribe nada, no pinte de rojo la primera posición de la RAM
    // It is put inside an if so it doesn't colour on red the first position in RAM if nothing is written
    if(searchBoxInt != 0){

        QTextCursor cursor(ui->ramText->document());

        cursor.setPosition(11 + byteMem * 3);   // 8 of the first numbers, 2 spaces after them + 1 so it starts on the first
                                                // hexadecimal number. byteMem * 3 is because it is 2 numbers and 1 space that
                                                // separates a number from the next one

        for (int i = 0; i < rowMem; ++i) {  // Go down to the line indicated before ()0x2A: line 2
            cursor.movePosition(QTextCursor::Down);
        }

        cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 2); // Select the following 2 characters

        QTextCharFormat format;

        format.setBackground(Qt::red); // Red background
        cursor.setCharFormat(format);

    }


    qDebug() << pageToView;
}


void MainWindow::on_openConfigButton_clicked()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile("./config.json"));
}



void MainWindow::on_generateStatsButton_clicked()
{
    statsDialog = new StatsDialog(nullptr, computer, RESULT_LOCATION);
    statsDialog->exec();
}


void MainWindow::on_exportDisButton_clicked()
{
    std::string programName =  ui->filenameText->text().toStdString();

    // Search for the file extension
    uint pos = programName.find('.');
    QString programNameWithoutExtension  = QString::fromStdString(programName.substr(0, pos));

    QFile file(disassemblyFileRoute + "/disassembly_" + programNameWithoutExtension + ".txt");

    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        // To write on the file
        QTextStream out(&file);

        QString header = "PROGRAM NAME: " + QString::fromStdString(programName) + "\r\n";
        int headerSize = header.size();

        for(int i = 0; i < headerSize; i++){
            header += "-";
        }
        header += "\r\n\r\n";

        out << header;
        out << QString::fromStdString(computer->exportDisassembly());

        file.close();

        QMessageBox::information(nullptr, "Export completed", "Disassembly exported. File in: " + disassemblyFileRoute);
    } else {
        qDebug() << "Cannot open the file:" << file.errorString();
    }
}


void MainWindow::on_exportRamButton_clicked()
{
    std::string programName =  ui->filenameText->text().toStdString();

    // Get file extension
    uint pos = programName.find('.');
    QString programNameWithoutExtension  = QString::fromStdString(programName.substr(0, pos));

    QFile file(ramFileRoute + "/ram_" + programNameWithoutExtension + ".hex");

    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {

        // To write on the file
        QDataStream out(&file);

        // Write data
        for (size_t i = 0; i < computer->ram_size; i+=4) {
            out << static_cast<quint32>(computer->ram.readWord(i)); // Write each byte as an unsigned 8 bit number
        }

        file.close();

        QMessageBox::information(nullptr, "Export completed", "RAM exported. file in: " + ramFileRoute);
    } else {
        qDebug() << "Cannot open the file:" << file.errorString();
        QMessageBox::critical(nullptr, "Export failed", "An error has ocurred while trying to export RAM");
    }
}


void MainWindow::resetInterface(){
    ui->runButton->setEnabled(false);
    ui->runPasoButton->setEnabled(false);
    ui->pauseButton->setEnabled(false);
    ui->executeCampaignButton->setEnabled(false);
    ui->generateStatsButton->setEnabled(false);

    ui->codeDisassemblyText->clear();
    ui->ramText->setPlainText(QString::fromStdString(computer->showRam(pageToView)));
    ui->registerText->setPlainText(QString::fromStdString(computer->showRegisters()));
    ui->filenameText->clear();
    programFileName = "";
    ui->campaignNameText->clear();

    editor->clear();
}


void MainWindow::UpdateInterface()
{
    UpdateTerminal();
    if(updateRamInRealTime)
        ui->ramText->setPlainText(QString::fromStdString(computer->showRam(pageToView)));   // Update ramBox
    ui->registerText->setPlainText(QString::fromStdString(computer->showRegisters()));
    ui->codeDisassemblyText->appendPlainText(QString::fromStdString(computer->showDisassembly()));
}

void MainWindow::UpdateTerminal(){
    /*ui->terminalBox->setPlainText("");
    for(int i = 0; i < 20; i++){
        ui->terminalBox->appendPlainText(computer->showVRAMLine(i));
    }*/
}



void MainWindow::on_actionGenerar_campa_a_aleatoria_triggered()
{
    QMessageBox::information(nullptr, "Select a file", "Please, select the file that will be bounded to the campaign");
    QString program = QFileDialog::getOpenFileName(nullptr, "Select file", "", "Files (*.bin *.o)");

    if(!program.isEmpty()){



        QDateTime currentDateTime = QDateTime::currentDateTime();

        QString currentDay = currentDateTime.toString("dddd").remove('"');

        QString currentTime = currentDateTime.toString("hh-mm-ss").remove('"');

        qDebug() << "Current day: " << currentDay;
        qDebug() << "Current hour: " << currentTime;

        QString programName =  "campaign_" + currentDay + "_" + currentTime;


        int instructions = 0;

        QJsonObject jsonObject;

        jsonObject["program"] = program;

        jsonObject["expectedResult"] = 0;
        jsonObject["expectedInstructions"] = 0;

        // JSONARRAY for inyections
        QJsonArray injectionsArr;

        for (int i = 0; i < 1000; ++i) {
            int inst = i;
            int reg = std::rand() % 32;
            int bit = std::rand() % 8;

            QJsonArray injection;
            injection.append(inst);
            injection.append(reg);
            injection.append(bit);

            injectionsArr.append(injection);
        }

        jsonObject["injections"] = injectionsArr;

        QJsonDocument jsonDocument(jsonObject);

        // Formatted json string
        QByteArray jsonData = jsonDocument.toJson(QJsonDocument::Indented);

        // Write on file
        QFile jsonFile(campaignGeneratorRoute + "/" + programName + ".json");
        if (jsonFile.open(QFile::WriteOnly | QFile::Truncate)) {
            jsonFile.write(jsonData);
            jsonFile.close();
            qDebug() << "Campaign generated successfully.";

            QMessageBox::information(nullptr, "Info", "Campaign successfully generated in: " + campaignGeneratorRoute);
        } else {
            qDebug() << "Error while trying to generate campaign.";
        }

    } else {
        qDebug() << "Error opening the file";
    }
}


void MainWindow::on_executeCampaignButton_clicked()
{
    // If the campaign doesn't have a program specified,
    // it should load the program, execute one time
    // and take note of the instructions required to be
    // executed.
    if(computer->campaign.expectedInstructions == 0){

        isExecutingBeforeCampaign = true;

        computer->LoadProgram(computer->campaign.programPath.toStdString());

        emit runProgram();

    } else {
        campaignDialog = new ExecutionCampignDialog(nullptr, computer->campaign.injections.size());
        campaignDialog->show();

        emit runCampaignIter();
    }

}

void MainWindow::iterationCampaign(){
    computer->reset();
    computer->LoadProgram(computer->campaign.programPath.toStdString());

    QTimer *timerCampaign = new QTimer(this);

    // Connect QTimer's timeout to the slot to execute one loop iteration.
    // Use lambda to capture and pass the parameter
    connect(timerCampaign, &QTimer::timeout, this, &MainWindow::runLoopIterationCampaign);

    timerCampaign->start();

    qDebug() << "Campaign iteration end";
}

void MainWindow::onFinishIter(){
    campaignDialog->updateProgressBar(injectionNumber);

    if(this->injectionNumber == computer->campaign.injections.size()-1)
        emit campaignComplete();
    else
        emit runCampaignIter();
}

// When campaign execution is completed,
// calls the method to write the final statistics
void MainWindow::onCampaignComplete(){

    QString str = "";
    float noeffect = 0, sdc = 0, sed = 0, due = 0;

    int hundred = campaignResults.size();

    foreach (int res, campaignResults) {
        switch (res) {
        case NO_EFFECT:
            noeffect++;
            break;
        case SDC:
            sdc++;
            break;
        case SED:
            sed++;
            break;
        case DUE:
            due++;
            break;
        }
    }

    // Percentage calculation
    noeffect = (noeffect * 100) / hundred;
    sdc = (sdc * 100) / hundred;
    sed = (sed * 100) / hundred;
    due = (due * 100) / hundred;

    str = QString("Results of the campaign:\nNo effect: %1%\nSDC: %2%\nSED: %3%\nDUE: %4%")
                  .arg(noeffect, 0, 'f', 2)
                  .arg(sdc, 0, 'f', 2)
                  .arg(sed, 0, 'f', 2)
                  .arg(due, 0, 'f', 2);


    campaignDialog->close();

    QMessageBox::information(nullptr, "Information about campaign", str);
    return;
}

// This method stores the program execution statistics for the campaign
void MainWindow::updateCampaignAfterProgramExecution(){

    isExecutingBeforeCampaign = false;

    int instruccionesEsperadas = computer->cpu.cycles;
    int resultEsperado = computer->ram.readByte(RESULT_LOCATION);

    qDebug() << "Instructions:" << instruccionesEsperadas;
    qDebug() << "Expected result:" << resultEsperado;

    computer->campaign.expectedInstructions = computer->cpu.cycles;
    computer->campaign.expectedResult = resultEsperado;

    campaignDialog = new ExecutionCampignDialog(nullptr, computer->campaign.injections.size());
    campaignDialog->show();

    emit runCampaignIter();

}


void MainWindow::on_loadCampaignButton_clicked()
{
    loadCampaign();
}

// Loads a campaign into memory
void MainWindow::loadCampaign(){

    // Opens file explorer so the user can select a specific json file
    QString campaignFileName = QFileDialog::getOpenFileName(this, "Select file", "", "*.json");

    if (!campaignFileName.isEmpty()) {

        resetInterface();

        QFileInfo fileInfo(campaignFileName);
        QString filename = fileInfo.fileName();

        qDebug() << "Selected file:" << campaignFileName;
        computer->reset();
        computer->LoadCampaign(campaignFileName.toStdString());
        ui->campaignNameText->setText(filename);

        QFileInfo programInfo(computer->campaign.programPath);
        QString programName = programInfo.fileName();

        ui->filenameText->setText(programName);

        QMessageBox::information(nullptr, "Info", "Campaign loaded");

        ui->executeCampaignButton->setEnabled(true);

    } else {
        qDebug() << "No file selected.";
    }
}


void MainWindow::on_actionOpen_File_triggered()
{
    programFileName = QFileDialog::getOpenFileName(this, "Select file", "", "*.c *.s");
    if (!programFileName.isEmpty()) {

        qDebug() << "File selected:" << programFileName;
        QFileInfo fileInfo(programFileName);
        QString filename = fileInfo.fileName();

        QFile file(programFileName);

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "Error",
                                 "Could not open the file");
            return;
        }

        QTextStream in(&file);
        editor->setPlainText(in.readAll());

        file.close();

        ui->filenameText->setText(filename);

        ui->languageSelector->setCurrentIndex(programFileName.contains(".c") ? 1 : 0);

        // On program load, enable execution buttons
        ui->runButton->setEnabled(true);
        ui->runPasoButton->setEnabled(true);
        ui->pauseButton->setEnabled(true);

    } else {
        qDebug() << "No file selected....";
    }
}


void MainWindow::on_actionSave_file_triggered()
{
    // In case there wasn't any program loaded (he is programming in a new file)
    if(programFileName == ""){

        programFileName = QFileDialog::getSaveFileName(this);
        if(!programFileName.contains(".c") && ui->languageSelector->currentText() == "C")
            programFileName.append(".c");
        else if(!programFileName.contains(".s") && ui->languageSelector->currentText() == "Assembly")
            programFileName.append(".s");

        if (!programFileName.isEmpty()) {
            qDebug() << "File saving as:" << programFileName;
        }
        else {
            qDebug() << "No file selected...";
            return;
        }
    }

    QFile file(programFileName);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error",
                             "Couldn't save the file");
        return;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);

    out << editor->toPlainText();

    if(ui->filenameText->text().isEmpty()){
        QFileInfo fileinfo(programFileName);
        resetFilenameText(fileinfo.fileName());
    } else {
        updateFilenameText(false);
    }

    file.close();
}


void MainWindow::on_actionNew_triggered()
{
    resetInterface();
}


void MainWindow::on_languageSelector_currentIndexChanged(int index)
{
    qDebug() << "Language: " << index;
    highlighter = new Highlighter(editor->document(), index);
}



