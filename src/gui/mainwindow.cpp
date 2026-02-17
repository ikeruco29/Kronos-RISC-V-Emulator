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
    // Esto es para establecer ciertos valores de la interfaz
    // Ejemplo: hasta que no cargue un programa, no puede usar el botón
    //          "ejecutar"
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

    // Copiar geometría y políticas
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

    // En QT, puedes vincular un método a otro. Este otro método se llama "signal"
    // y no tienen implementación. Al emitirlos mediante "emit nombreSignal()"
    // se llama al método conectado en un hilo a parte, lo que hace que el hilo principal
    // (que es el que contiene la interfaz) no se bloquee, y el usuario pueda pulsar otros
    // botones como el botón de pausa.
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

// Carga el programa seleccionado en memoria
void MainWindow::on_actionCargar_programa_triggered()
{

    // Esto abre el explorador de archivos para seleccionar un programa binario
    QString nombreArchivo = QFileDialog::getOpenFileName(this, "Select file", "", "*.bin *.o");
    if (!nombreArchivo.isEmpty()) {

        qDebug() << "File selected:" << nombreArchivo;   // debug
        QFileInfo fileInfo(nombreArchivo);
        QString filename = fileInfo.fileName(); // Para sacar solo el nombre

        computer->reset();  // Reset del ordenador
        computer->LoadProgram(nombreArchivo.toStdString()); // Carga el programa en memoria

        resetInterface();   // Reestablece la interfaz

        pageToView = computer->ram.iRomStartAddr;   // Esto es para el buscador de la RAM.
                                                    // No quiero se muestre la RAM al completo,
                                                    // si no solo 8 filas que es lo que cabe
                                                    // en la caja de texto.

        ui->ramText->setPlainText(QString::fromStdString(computer->showRam(pageToView)));
        resetFilenameText(filename);


        // Al cargar el programa, se habilitan los botones de control de ejecución
        ui->runButton->setEnabled(true);
        ui->runPasoButton->setEnabled(true);
        ui->pauseButton->setEnabled(true);

    } else {
        qDebug() << "No file selected....";
    }

    stopExec = false;
}

// Botón para cargar una campaña
void MainWindow::on_actionCargar_campa_a_triggered()
{
    loadCampaign();
}

// Cerrar la aplicación
void MainWindow::on_actionSalir_triggered()
{
    qApp->quit();
}

// Botón para realizar la ejecución completa del programa en memoria
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


        computer->reset();  // Reset computer
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


// Realiza un ciclo de ejecución (fetch, decode y execute)
void MainWindow::runLoopIteration()
{

    // When ECALL instruction is called, it means the program has finished
    if (computer->cpu.bEcall == true || stopExec) {

        sender()->deleteLater(); // Eliminar el QTimer después de terminar el bucle

        // Esto es para que, en caso de que se haya ejecutado por una campaña, siga con la campaña
        if(this->isExecutingBeforeCampaign)
            emit runProgramCompleted();

        else if(computer->cpu.bEcall == true){

            ui->ramText->setPlainText(QString::fromStdString(computer->showRam(pageToView)));   // Update ramBox
            ui->generateStatsButton->setEnabled(true);
            QMessageBox::information(nullptr, "Execution completed", "The program execution has finished");

        }
        return;
    }

    // Ejecutar una iteración del bucle
    computer->cpu.clock();

    if(!this->isExecutingBeforeCampaign)    // Para no mostrar la primera ejecución del programa en una campaña
        this->UpdateInterface();
}

// Realiza un ciclo de ejecución de la campaña
void MainWindow::runLoopIterationCampaign()
{
    qDebug() << computer->cpu.cycles;

    if (computer->cpu.bEcall == true) {

        // Al escribir en la posición FINISH_LOCATION un 0, para la ejecución del programa
        if(computer->ram.readByte(RESULT_LOCATION) != computer->campaign.expectedResult){

            // Resultado final: Silent Data Corruption (SDC)
            this->campaignResults.push_back(SDC);

        }
        else if(computer->campaign.expectedInstructions > computer->cpu.cycles){

            // Resultado final: Single Event Delay (SED)
            this->campaignResults.push_back(SED);

        } else {

            // Resultado final: NO EFFECT
            this->campaignResults.push_back(NO_EFFECT);

        }

        this->injectionNumber++;    // Inyección por la que va

        sender()->deleteLater(); // Eliminar el QTimer después de terminar el bucle
        emit campaignIterComplete();
        return;
    }


    // Si aún no ha tardado el doble en ejecuctarse, sigue ejecutándose.
    if(computer->campaign.expectedInstructions * 2 > computer->cpu.cycles){

        if(computer->cpu.cycles == this->injectionNumber){
            int inst = computer->cpu.cycles;    // número de instrucción
            int reg = computer->campaign.injections[inst][1];   // Registro a cambiar
            computer->cpu.registers[reg] ^= (1 << computer->campaign.injections[inst][2]); // invierte el bit utilizando XOR
        }

    }else{
        // Resultado final: Detected Unrecovery Error (DUE)
        this->campaignResults.push_back(DUE);

        this->injectionNumber++;

        sender()->deleteLater(); // Eliminar el QTimer después de terminar el bucle

        emit campaignIterComplete();
        return;
    }

    // Ejecutar una iteración del bucle
    computer->cpu.clock();
}



// Botón de reset
void MainWindow::on_stopButton_clicked()
{
    stopExec = true;    // Si hay una ejecución en marcha, se detiene
    computer->reset();  // Se resetea el ordenador
    resetInterface();   // Se resetea la interfaz
}

// Botón para parar la ejecución
void MainWindow::on_pauseButton_clicked()
{
    stopExec = true;
}

// Botón para ejecutar solo un paso del programa
void MainWindow::on_runPasoButton_clicked()
{
    if (computer->ram.readByte(FINISH_LOCATION) != 0) {
        computer->cpu.clock();
        this->UpdateInterface();
    } else {
        ui->ramText->setPlainText(QString::fromStdString(computer->showRam(pageToView)));   // Update ramBox
        QMessageBox::information(nullptr, "Execution completed", "The program execution has finished");
        ui->generateStatsButton->setEnabled(true);  // Se habilita el botón para generar estadísticas del emulador
    }
}

// Método que gestiona cuando se escribe algo para buscar en la memoria
void MainWindow::on_searchBox_editingFinished()
{
    QString memoryToView = ui->searchBox->text();

    int searchBoxInt = memoryToView.toUInt(nullptr, 16);

    pageToView = searchBoxInt;

    int byteMem = pageToView & 0xF;
    int rowMem = (pageToView >> 4) % 8; // Saca la fila (El modulo 8 es porque se muestran solo 8 filas)

    pageToView = (pageToView & 0xFFFFFF80); // Como son 8 filas

    ui->ramText->setPlainText(QString::fromStdString(computer->showRam(pageToView)));

    // Lo pongo en un if para que si no escribe nada, no pinte de rojo la primera posición de la RAM
    if(searchBoxInt != 0){

        QTextCursor cursor(ui->ramText->document());

        cursor.setPosition(11 + byteMem * 3);  // 8 de los primeros numeros, 2 espacios después de esos + 1 para que empiece en el primer número
            // hexadecimal. byteMem * 3 es porque son dos numeros y 1 espacio lo que separa a cada numero
            // del siguiente

        for (int i = 0; i < rowMem; ++i) {  // Baja a la línea que le indique, sacada anteriormente ()0x2A: línea 2
            cursor.movePosition(QTextCursor::Down);
        }

        cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 2); // Selecciona los siguientes dos caracteres

        QTextCharFormat format;

        format.setBackground(Qt::red); // Establece el fondo en rojo
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

    // Buscar la posición del último punto en el nombre del archivo
    uint pos = programName.find('.');
    QString programNameWithoutExtension  = QString::fromStdString(programName.substr(0, pos));

    // Crear un objeto QFile con la ruta especificada
    QFile file(disassemblyFileRoute + "/disassembly_" + programNameWithoutExtension + ".txt");

    // Intentar abrir el archivo en modo de escritura de texto
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        // Crear un objeto QTextStream para escribir en el archivo
        QTextStream out(&file);

        // Escribir el contenido en el archivo
        QString header = "PROGRAM NAME: " + QString::fromStdString(programName) + "\r\n";
        int headerSize = header.size();

        for(int i = 0; i < headerSize; i++){
            header += "-";
        }
        header += "\r\n\r\n";

        out << header;
        out << QString::fromStdString(computer->exportDisassembly());

        // Cerrar el archivo
        file.close();

        QMessageBox::information(nullptr, "Export completed", "Disassembly exported. File in: " + disassemblyFileRoute);
    } else {
        // Si no se pudo abrir el archivo, mostrar un mensaje de error
        qDebug() << "Cannot open the file:" << file.errorString();
    }
}


void MainWindow::on_exportRamButton_clicked()
{
    std::string programName =  ui->filenameText->text().toStdString();

    // Buscar la posición del último punto en el nombre del archivo
    uint pos = programName.find('.');
    QString programNameWithoutExtension  = QString::fromStdString(programName.substr(0, pos));

    // Crear un objeto QFile con la ruta especificada
    QFile file(ramFileRoute + "/ram_" + programNameWithoutExtension + ".hex");

    // Intentar abrir el archivo en modo de escritura de texto
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {

        // Crear un objeto QTextStream para escribir en el archivo
        QDataStream out(&file);

        // Escribir los datos en el archivo
        for (size_t i = 0; i < computer->ram_size; i+=4) {
            out << static_cast<quint32>(computer->ram.readWord(i)); // Escribir cada byte como un entero de 8 bits sin signo
        }

        // Cerrar el archivo
        file.close();

        QMessageBox::information(nullptr, "Export completed", "RAM exported. file in: " + ramFileRoute);
    } else {
        // Si no se pudo abrir el archivo, mostrar un mensaje de error
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
    UpdateTerminal();    // Update terminalBox
    if(updateRamInRealTime)
        ui->ramText->setPlainText(QString::fromStdString(computer->showRam(pageToView)));   // Update ramBox
    ui->registerText->setPlainText(QString::fromStdString(computer->showRegisters()));  // Update registerBox
    ui->codeDisassemblyText->appendPlainText(QString::fromStdString(computer->showDisassembly()));  // Update disassembly
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

        // Obteniendo el día actual
        QString currentDay = currentDateTime.toString("dddd").remove('"');

        // Obteniendo la hora actual
        QString currentTime = currentDateTime.toString("hh-mm-ss").remove('"');

        qDebug() << "Día actual: " << currentDay;
        qDebug() << "Hora actual: " << currentTime;

        QString programName =  "campaign_" + currentDay + "_" + currentTime;


        int instructions = 0;

        QJsonObject jsonObject;

        jsonObject["program"] = program;

        jsonObject["expectedResult"] = 0;
        jsonObject["expectedInstructions"] = 0;

        // JSONARRAY para las inyecciones
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




        // Convertir el objeto JSON en un documento JSON
        QJsonDocument jsonDocument(jsonObject);

        // Convertir el documento JSON en una cadena formateada
        QByteArray jsonData = jsonDocument.toJson(QJsonDocument::Indented);

        // Escribir la cadena JSON en un archivo
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
    // Si la campaña no tiene un programa configurado...
    if(computer->campaign.expectedInstructions == 0){

        isExecutingBeforeCampaign = true;

        // Carga del programa que hay asociado a la campaña
        computer->LoadProgram(computer->campaign.programPath.toStdString());

        emit runProgram();

    } else {
        campaignDialog = new ExecutionCampignDialog(nullptr, computer->campaign.injections.size());
        campaignDialog->show();

        emit runCampaignIter();
    }

}

void MainWindow::iterationCampaign(){
    // Ejecución de la campaña
    computer->reset();
    computer->LoadProgram(computer->campaign.programPath.toStdString());

    // Ejecución de la campaña
    QTimer *timerCampaign = new QTimer(this);

    // Conectar el timeout del QTimer al slot para ejecutar una iteración del bucle
    // Usar un lambda para capturar y pasar el parámetro
    connect(timerCampaign, &QTimer::timeout, this, &MainWindow::runLoopIterationCampaign);

    timerCampaign->start();

    qDebug() << "end";

    //computer->reset();
}

void MainWindow::onFinishIter(){
    campaignDialog->updateProgressBar(injectionNumber);

    if(this->injectionNumber == computer->campaign.injections.size()-1)
        emit campaignComplete();
    else
        emit runCampaignIter();
}

// Cuando se completa la ejecución de la campaña
// se llama a este método para imprimir las estadísticas
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

    // Cálculo de porcentajes
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

// Este método guarda las estadísticas de ejecución de un programa
// que necesita la campaña para ejecutarla
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
    // Esto abre el explorador de archivos para seleccionar un programa binario
    programFileName = QFileDialog::getOpenFileName(this, "Select file", "", "*.c *.s");
    if (!programFileName.isEmpty()) {

        qDebug() << "File selected:" << programFileName;   // debug
        QFileInfo fileInfo(programFileName);
        QString filename = fileInfo.fileName(); // Para sacar solo el nombre

        //resetInterface();   // Reestablece la interfaz

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
        // Al cargar el programa, se habilitan los botones de control de ejecución
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
            qDebug() << "File saving as:" << programFileName;   // debug
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



