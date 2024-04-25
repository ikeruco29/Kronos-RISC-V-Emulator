#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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
        //computer->LoadProgram(nombreArchivo.toStdString());
    } else {
        qDebug() << "Ningún archivo seleccionado.";
    }
}


void MainWindow::on_actionCargar_campa_a_triggered()
{
    QString nombreArchivo = QFileDialog::getOpenFileName(this, "Seleccionar archivo", "", "*.bin");
    if (!nombreArchivo.isEmpty()) {
        // Aquí puedes cargar el archivo seleccionado
        qDebug() << "Archivo seleccionado:" << nombreArchivo;
        //computer->LoadProgram(nombreArchivo.toStdString());
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
    //computer->On();
    //ui->ramBox->setText(QString::fromStdString(computer->exportRam()));
}


void MainWindow::on_stopButton_clicked()
{

}

