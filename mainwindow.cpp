#include "mainwindow.h"
#include "./ui_mainwindow.h"
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




MainWindow::MainWindow(QWidget *parent, Computer *comp)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , computer(comp)
{
    ui->setupUi(this);
    //QCoreApplication::setAttribute(Qt::AA_UseStyleSheetPropagationInWidgetStyles, true);
    QString stsheet = "QPushButton:disabled {background-color: rgba(255, 255, 255, 0.1); }";
    ui->centralwidget->setStyleSheet(stsheet);
    ui->runButton->setEnabled(false);
    ui->runPasoButton->setEnabled(false);
    ui->pauseButton->setEnabled(false);
    ui->executeCampaignButton->setEnabled(false);
    ui->generateStatsButton->setEnabled(false);

    isExecutingBeforeCampaign = false;

    connect(this, &MainWindow::runProgram, this, &MainWindow::on_runButton_clicked);
    connect(this, &MainWindow::runProgramCompleted, this, &MainWindow::updateCampaignAfterProgramExecution);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionCargar_programa_triggered()
{
    QString nombreArchivo = QFileDialog::getOpenFileName(this, "Seleccionar archivo", "", "*.bin *.o");
    if (!nombreArchivo.isEmpty()) {
        // Aquí puedes cargar el archivo seleccionado
        qDebug() << "Archivo seleccionado:" << nombreArchivo;
        QFileInfo fileInfo(nombreArchivo);
        QString filename = fileInfo.fileName();

        computer->reset();
        computer->LoadProgram(nombreArchivo.toStdString());

        resetInterface();

        ui->ramText->setPlainText(QString::fromStdString(computer->showRam()));
        ui->filenameText->setText(filename);

        ui->runButton->setEnabled(true);
        ui->runPasoButton->setEnabled(true);
        ui->pauseButton->setEnabled(true);
    } else {
        qDebug() << "Ningún archivo seleccionado.";
    }

    stopExec = false;
}


void MainWindow::on_actionCargar_campa_a_triggered()
{
    loadCampaign();
}


void MainWindow::on_actionSalir_triggered()
{
    qApp->quit();
}


int MainWindow::on_runButton_clicked()
{
    stopExec = false;

    // Todo esto del QTimer es porque si utilizo un bucle que haga
    // estas cuatro instrucciones:
    //
    //      computer->cpu.clock();
    //      ui->ramText->setPlainText(QString::fromStdString(computer->showRam(pageToView)));
    //      ui->registerText->setPlainText(QString::fromStdString(computer->showRegisters()));
    //      ui->codeDisassemblyText->appendPlainText(QString::fromStdString(computer->showDisassembly()));
    //
    // No le da tiempo a renderizar, así que necesito que tenga cierto delay.
    // Realmente no pasa nada porque el procesador no tenga una velocidad vertiginosa
    // (que la tiene), ya que lo importante es que lleve bien el número de instrucciones

    QTimer *timer = new QTimer(this);

    // Conectar el timeout del QTimer al slot para ejecutar una iteración del bucle
    connect(timer, &QTimer::timeout, this, &MainWindow::runLoopIteration);

    timer->start();

    return 0;
}


void MainWindow::runLoopIteration()
{
    qDebug() << "Iteracion";

    // TODO: CAMBIAR CONDICIÓN DEL BUCLE            ESTA VARIABLE NO!!!
    if (computer->ram.readByte(FINISH_LOCATION) == 0 || stopExec) {

        sender()->deleteLater(); // Eliminar el QTimer después de terminar el bucle

        // Esto es para que, en caso de que se haya ejecutado por una campaña, siga con la campaña
        if(this->isExecutingBeforeCampaign)
            emit runProgramCompleted();

        else if(computer->ram.readByte(FINISH_LOCATION) == 0){

            ui->generateStatsButton->setEnabled(true);
            QMessageBox::information(nullptr, "Programa finalizado", "La ejecución del programa ha finalizado");

        }
        return;
    }

    // Ejecutar una iteración del bucle
    computer->cpu.clock();

    if(!this->isExecutingBeforeCampaign)    // Para no mostrar la primera ejecución del programa en una campaña
        this->UpdateInterface();
}

void MainWindow::runLoopIterationCampaign()
{
    qDebug() << computer->cpu.cycles;

    if (computer->ram.readByte(FINISH_LOCATION) == 0) {

        // TODO: COMPARAR EL RESULTADO CON LA POSICIÓN DE MEMORIA CONOCIDA
        if(computer->ram.readByte(RESULT_LOCATION) != computer->campaign.expectedResult){
            QMessageBox::information(nullptr, "Información sobre la campaña", "La campaña ha finalizado. Resultado final: Silent Data Corruption (SDC)");
        }
        else if(computer->campaign.expectedInstructions > computer->cpu.cycles){
            QMessageBox::information(nullptr, "Información sobre la campaña", "La campaña ha finalizado. Resultado final: Single Event Delay (SED)");
        } else {
            QMessageBox::information(nullptr, "Información sobre la campaña", "La campaña ha finalizado. Resultado final: NO EFFECT");
        }

        sender()->deleteLater(); // Eliminar el QTimer después de terminar el bucle
        ui->generateStatsButton->setEnabled(true);
        return;
    }


    // Si aún no ha tardado el doble en ejecuctarse, sigue ejecutándose.
    if(computer->campaign.expectedInstructions * 2 > computer->cpu.cycles){
        int inst = computer->cpu.cycles;    // número de instrucción
        int reg = computer->campaign.injections[inst][1];   // Registro a cambiar
        computer->cpu.registers[reg] ^= (1 << computer->campaign.injections[inst][2]); // invierte el bit
    }else{
        QMessageBox::information(nullptr, "Información sobre la campaña", "La campaña ha finalizado. Resultado final: Detected Unrecovery Error (DUE).\n Instrucciones esperadas:");

        sender()->deleteLater(); // Eliminar el QTimer después de terminar el bucle
        ui->generateStatsButton->setEnabled(true);
        return;
    }

    // Ejecutar una iteración del bucle
    computer->cpu.clock();
    this->UpdateInterface();
}




void MainWindow::on_stopButton_clicked()
{
    stopExec = true;
    computer->reset();
    resetInterface();
}

void MainWindow::on_pauseButton_clicked()
{
    stopExec = true;
}

void MainWindow::on_runPasoButton_clicked()
{
    if (computer->ram.readByte(FINISH_LOCATION) != 0) {
        computer->cpu.clock();
        this->UpdateInterface();
    } else {
        QMessageBox::information(nullptr, "Programa finalizado", "La ejecución del programa ha finalizado");
        ui->generateStatsButton->setEnabled(true);  // Se habilita el botón para generar estadísticas del emulador
    }
}


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

        QMessageBox::information(nullptr, "Exportación satisfactoria", "Desencamblado exportado. Archivo en: " + disassemblyFileRoute);
    } else {
        // Si no se pudo abrir el archivo, mostrar un mensaje de error
        qDebug() << "No se pudo abrir el archivo:" << file.errorString();
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

        QMessageBox::information(nullptr, "Exportación satisfactoria", "RAM exportado. Archivo en: " + ramFileRoute);
    } else {
        // Si no se pudo abrir el archivo, mostrar un mensaje de error
        qDebug() << "No se pudo abrir el archivo:" << file.errorString();
        QMessageBox::critical(nullptr, "Fallo en la exportación", "Ha habido un fallo inesperado al exportar la RAM");
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
    ui->terminalBox->setPlainText("");
    ui->filenameText->clear();
    ui->campaignNameText->clear();
}


void MainWindow::UpdateInterface()
{
    ui->terminalBox->setPlainText(computer->showVRAM());    // Update terminalBox
    ui->ramText->setPlainText(QString::fromStdString(computer->showRam(pageToView)));   // Update ramBox
    ui->registerText->setPlainText(QString::fromStdString(computer->showRegisters()));  // Update registerBox
    ui->codeDisassemblyText->appendPlainText(QString::fromStdString(computer->showDisassembly()));  // Update disassembly
}



void MainWindow::on_actionGenerar_campa_a_aleatoria_triggered()
{

    QString program = QFileDialog::getOpenFileName(nullptr, "Seleccionar archivo", "", "Archivos (*.bin *.o)");

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

        for (int i = 0; i < 10000; ++i) {
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
            qDebug() << "Campaña generada con éxito.";

            QMessageBox::information(nullptr, "Información", "Campaña generada con éxito en: " + campaignGeneratorRoute);
        } else {
            qDebug() << "Error al generar campaña para escritura.";
        }

    } else {
        qDebug() << "Error al abrir el archivo";
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

        // Ejecución de la campaña
        computer->reset();
        computer->LoadProgram(computer->campaign.programPath.toStdString());

        // Ejecución de la campaña
        QTimer *timerCampaign = new QTimer(this);

        // Conectar el timeout del QTimer al slot para ejecutar una iteración del bucle
        connect(timerCampaign, &QTimer::timeout, this, &MainWindow::runLoopIterationCampaign);

        timerCampaign->start();

        computer->reset();

    }



}

void MainWindow::updateCampaignAfterProgramExecution(){

    isExecutingBeforeCampaign = false;

    int instruccionesEsperadas = computer->cpu.cycles;
    int resultEsperado = computer->ram.readByte(RESULT_LOCATION);

    qDebug() << "Instrucciones:" << instruccionesEsperadas;
    qDebug() << "Resultado esperado:" << resultEsperado;

    computer->campaign.expectedInstructions = computer->cpu.cycles;
    computer->campaign.expectedResult = resultEsperado;

    computer->reset();
    computer->LoadProgram(computer->campaign.programPath.toStdString());

    // Ejecución de la campaña
    QTimer *timerCampaign = new QTimer(this);

    // Conectar el timeout del QTimer al slot para ejecutar una iteración del bucle
    connect(timerCampaign, &QTimer::timeout, this, &MainWindow::runLoopIterationCampaign);

    timerCampaign->start();


}


void MainWindow::on_loadCampaignButton_clicked()
{
    loadCampaign();
}

void MainWindow::loadCampaign(){
    QString nombreArchivo = QFileDialog::getOpenFileName(this, "Seleccionar archivo", "", "*.json");
    if (!nombreArchivo.isEmpty()) {

        resetInterface();

        QFileInfo fileInfo(nombreArchivo);
        QString filename = fileInfo.fileName();

        qDebug() << "Archivo seleccionado:" << nombreArchivo;
        computer->reset();
        computer->LoadCampaign(nombreArchivo.toStdString());
        ui->campaignNameText->setText(filename);

        QFileInfo programInfo(computer->campaign.programPath);
        QString programName = programInfo.fileName();

        ui->filenameText->setText(programName);

        QMessageBox::information(nullptr, "Información", "Campaña cargada");

        ui->executeCampaignButton->setEnabled(true);

    } else {
        qDebug() << "Ningún archivo seleccionado.";
    }
}

