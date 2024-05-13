#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDesktopServices>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent, Computer *comp)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , computer(comp)
{
    ui->setupUi(this);
    ui->runButton->setEnabled(false);
    ui->runPasoButton->setEnabled(false);
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
    QString nombreArchivo = QFileDialog::getOpenFileName(this, "Seleccionar archivo", "", "*.json");
    if (!nombreArchivo.isEmpty()) {
        // Aquí puedes cargar el archivo seleccionado
        qDebug() << "Archivo seleccionado:" << nombreArchivo;
        computer->reset();
        computer->LoadCampaign(nombreArchivo.toStdString());
    } else {
        qDebug() << "Ningún archivo seleccionado.";
    }
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

    // Conectar el timeout del QTimer al slot para ejecutar una iteración del bucle
    connect(timer, &QTimer::timeout, this, &MainWindow::runLoopIteration);

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
    ui->ramText->setPlainText(QString::fromStdString(computer->showRam(pageToView)));
    ui->registerText->setPlainText(QString::fromStdString(computer->showRegisters()));
    ui->codeDisassemblyText->appendPlainText(QString::fromStdString(computer->showDisassembly()));
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
    ui->ramText->setPlainText(QString::fromStdString(computer->showRam(pageToView)));
    ui->registerText->setPlainText(QString::fromStdString(computer->showRegisters()));
    ui->codeDisassemblyText->appendPlainText(QString::fromStdString(computer->showDisassembly()));
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

void MainWindow::resetInterface(){
    ui->runButton->setEnabled(false);
    ui->runPasoButton->setEnabled(false);

    ui->codeDisassemblyText->clear();
    ui->ramText->clear();
    ui->registerText->setPlainText(QString::fromStdString(computer->showRegisters()));
    ui->filenameText->clear();
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
    QFile file(disassemblyFileRoute + "/ram_" + programNameWithoutExtension + ".hex");

    // Intentar abrir el archivo en modo de escritura de texto
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        // Crear un objeto QTextStream para escribir en el archivo
        QDataStream out(&file);

        // Escribir los datos en el archivo
        for (size_t i = 0; i < computer->ram_size/4; i+=4) {
            out << static_cast<quint32>(computer->ram.readWord(i)); // Escribir cada byte como un entero de 8 bits sin signo
        }

        // Cerrar el archivo
        file.close();
    } else {
        // Si no se pudo abrir el archivo, mostrar un mensaje de error
        qDebug() << "No se pudo abrir el archivo:" << file.errorString();
    }
}




