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
        computer->LoadProgram(nombreArchivo.toStdString());
        ui->ramText->setPlainText(QString::fromStdString(computer->showRam()));
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
    pasoapaso = false;
    computer->executeProgram();
    ui->ramText->setPlainText(QString::fromStdString(computer->showRam()));
}


void MainWindow::on_stopButton_clicked()
{
    // computer.reset();
}

void MainWindow::on_runPasoButton_clicked()
{
    pasoapaso = true;
    computer->cpu.clock();
    ui->ramText->setPlainText(QString::fromStdString(computer->showRam(pageToView)));
    ui->registerText->setPlainText(QString::fromStdString(computer->showRegisters()));
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

