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
    ui->runButton->setEnabled(false);
    ui->runPasoButton->setEnabled(false);
    isCampaign = false;
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


void MainWindow::on_runButton_clicked()
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

    if(isCampaign){
        // Conectar el timeout del QTimer al slot para ejecutar una iteración del bucle
        connect(timer, &QTimer::timeout, this, &MainWindow::runLoopIterationCampaign);
    } else {
        // Conectar el timeout del QTimer al slot para ejecutar una iteración del bucle
        connect(timer, &QTimer::timeout, this, &MainWindow::runLoopIteration);
    }


    // Configurar el intervalo del timer en milisegundos
    timer->setInterval(5);

    timer->start();
}


void MainWindow::runLoopIteration()
{
    // TODO: CAMBIAR CONDICIÓN DEL BUCLE            ESTA VARIABLE NO!!!
    if (computer->ram.readByte(0x80003020) == 0 || stopExec) {

        sender()->deleteLater(); // Eliminar el QTimer después de terminar el bucle
        return;
    }

    // Ejecutar una iteración del bucle
    computer->cpu.clock();
    this->UpdateInterface();
}

int MainWindow::runLoopIterationCampaign()
{
    // TODO: CAMBIAR CONDICIÓN DEL BUCLE
    if (computer->ram.readByte(0x80003020) == 0) {

        // TODO: COMPARAR EL RESULTADO CON LA POSICIÓN DE MEMORIA CONOCIDA

        if(computer->campaign.expectedInstructions > computer->cpu.cycles){
            QMessageBox::information(nullptr, "Información sobre la campaña", "La campaña ha finalizado. Resultado final: Single Event Delay (SED)");
        } else {
            QMessageBox::information(nullptr, "Información sobre la campaña", "La campaña ha finalizado. Resultado final: NO EFFECT");
        }


        sender()->deleteLater(); // Eliminar el QTimer después de terminar el bucle
        return 0;
    }


    // Si aún no ha tardado el doble en ejecuctarse, sigue ejecutándose.
    if(computer->campaign.expectedInstructions * 2 < computer->cpu.cycles){
        int inst = computer->cpu.cycles;    // número de instrucción
        int reg = computer->campaign.injections[inst][1];   // Registro a cambiar
        computer->cpu.registers[reg] ^= (1 << computer->campaign.injections[inst][2]); // invierte el bit
    }else{
        QMessageBox::information(nullptr, "Información sobre la campaña", "La campaña ha finalizado. Resultado final: Detected Unrecovery Error (DUE)");

        sender()->deleteLater(); // Eliminar el QTimer después de terminar el bucle
        return 1;
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
    computer->cpu.clock();
    this->UpdateInterface();
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

    ui->codeDisassemblyText->clear();
    ui->ramText->clear();
    ui->registerText->setPlainText(QString::fromStdString(computer->showRegisters()));
    ui->terminalBox->setPlainText("");
    ui->filenameText->clear();
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
    // Si la campaña ha sido generada, no hay programa introducido...
    if(computer->campaign.expectedInstructions == 0){
        // Carga del programa que hay asociado a la campaña
        computer->LoadProgram(computer->campaign.programPath.toStdString());

        // Ejecución del programa
        on_runButton_clicked();

        computer->campaign.expectedInstructions = computer->cpu.cycles;
    }

    isCampaign = true;  // Indica que es campaña para modificar instrucciones

    // Ejecución de la campaña
    on_runButton_clicked();

    isCampaign = false;

}


void MainWindow::on_loadCampaignButton_clicked()
{
    loadCampaign();
}

void MainWindow::loadCampaign(){
    QString nombreArchivo = QFileDialog::getOpenFileName(this, "Seleccionar archivo", "", "*.json");
    if (!nombreArchivo.isEmpty()) {

        QFileInfo fileInfo(nombreArchivo);
        QString filename = fileInfo.fileName();

        qDebug() << "Archivo seleccionado:" << nombreArchivo;
        computer->reset();
        computer->LoadCampaign(nombreArchivo.toStdString());
        ui->campaignNameText->setText(filename);

        QMessageBox::information(nullptr, "Información", "Campaña cargada");

    } else {
        qDebug() << "Ningún archivo seleccionado.";
    }
}

