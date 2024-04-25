#include "display.h"
#include "./ui_display.h"
#include <QFileDialog>

Display::Display(QWidget *parent, Computer *comp)
    : QMainWindow(parent)
    , ui(new Ui::Display)
    , computer(comp)
{
    ui->setupUi(this);
}

Display::~Display()
{
    delete ui;
}

void Display::on_actionSalir_triggered()
{
    qApp->quit();
}


void Display::on_actionCargar_programa_triggered()
{
    QString nombreArchivo = QFileDialog::getOpenFileName(this, "Seleccionar archivo", "", "*.bin");
    if (!nombreArchivo.isEmpty()) {
        // Aquí puedes cargar el archivo seleccionado
        qDebug() << "Archivo seleccionado:" << nombreArchivo;
        computer->LoadProgram(nombreArchivo.toStdString());
    } else {
        qDebug() << "Ningún archivo seleccionado.";
    }
}


void Display::on_actionCargar_campa_a_triggered()
{
    QString nombreArchivo = QFileDialog::getOpenFileName(this, "Seleccionar archivo", "", "*.bin");
    if (!nombreArchivo.isEmpty()) {
        // Aquí puedes cargar el archivo seleccionado
        qDebug() << "Archivo seleccionado:" << nombreArchivo;
        computer->LoadProgram(nombreArchivo.toStdString());
    } else {
        qDebug() << "Ningún archivo seleccionado.";
    }
}


void Display::on_pushButton_2_clicked()
{

}


void Display::on_pushButton_clicked()
{
    computer->On(0);
    ui->ramBox->setText(QString::fromStdString(computer->exportRam()));
}

