#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDesktopServices>

MainWindow::MainWindow(QWidget *parent, Computer *comp)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , computer(comp)
{
    ui->setupUi(this);
    ui->runButton->setEnabled(false);
    ui->runPasoButton->setEnabled(false);
    //computer->ramBox = ui->ramText;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionCargar_programa_triggered()
{
    QString nombreArchivo = QFileDialog::getOpenFileName(this, "Seleccionar archivo", "", "*.bin");
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
    computer->executeProgram();
    ui->ramText->setPlainText(QString::fromStdString(computer->showRam()));
}


void MainWindow::on_stopButton_clicked()
{
    computer->reset();
    resetInterface();
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

    pageToView = memoryToView.toInt(nullptr, 16);
    pageToView /= (1024 * 4);

    ui->ramText->setPlainText(QString::fromStdString(computer->showRam(pageToView)));

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
    ui->registerText->setPlainText("PC:  00000000 IR:  00000000\r\n\r\nX0:  00000000 X16: 00000000\r\nX1:  00000000 X17: 00000000\r\nX2:  00000000 X18: 00000000\r\nX3:  00000000 X19: 00000000\r\nX4:  00000000 X20: 00000000\r\nX5:  00000000 X21: 00000000\r\nX6:  00000000 X22: 00000000\r\nX7:  00000000 X23: 00000000\r\nX8:  00000000 X24: 00000000\r\nX9:  00000000 X25: 00000000\r\nX10: 00000000 X26: 00000000\r\nX11: 00000000 X27: 00000000\r\nX12: 00000000 X28: 00000000\r\nX13: 00000000 X29: 00000000\r\nX14: 00000000 X30: 00000000\r\nX15: 00000000 X31: 00000000");
    ui->filenameText->clear();
}

