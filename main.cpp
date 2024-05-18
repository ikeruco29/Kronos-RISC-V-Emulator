#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

const QString CONFIG_FILE = "./config.json";

uint ramSize, finish_location, result_location;
QString disassemblyRouteFile, ramRouteFile, campaignRoute;

int readConfigFile();

int main(int argc, char *argv[])
{
    readConfigFile();

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    Computer computer = Computer(ramSize);

    w.computer = &computer;
    w.disassemblyFileRoute = disassemblyRouteFile;
    w.ramFileRoute = ramRouteFile;
    w.campaignGeneratorRoute = campaignRoute;

    w.RESULT_LOCATION = result_location;
    w.FINISH_LOCATION = finish_location;

    return a.exec();
}

int readConfigFile(){
    // Abrir el archivo JSON
    QFile file(CONFIG_FILE);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "No se pudo abrir el archivo JSON";
        return 1;
    }

    // Leer todo el contenido del archivo JSON
    QByteArray jsonData = file.readAll();

    // Cerrar el archivo
    file.close();

    // Crear un QJsonDocument a partir de los datos leídos
    QJsonParseError error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &error);

    // Verificar errores de parseo
    if (jsonDoc.isNull()) {
        qWarning() << "Error al parsear el archivo JSON:" << error.errorString();
        return 1;
    }

    // Verificar si el documento es un objeto JSON
    if (!jsonDoc.isObject()) {
        qWarning() << "El archivo JSON no contiene un objeto JSON";
        return 1;
    }

    // Obtener el objeto JSON raíz
    QJsonObject jsonObj = jsonDoc.object();

    // Extraer valores del objeto JSON
    ramSize = jsonObj["ramSize"].toInt() * 1024 * 1024 * 1024;
    disassemblyRouteFile = jsonObj["disassemblyFileRoute"].toString();
    ramRouteFile = jsonObj["ramFileRoute"].toString();
    campaignRoute = jsonObj["campaignGeneratorRoute"].toString();
    result_location = jsonObj["resultRamLocation"].toString().toUInt(nullptr, 16);
    finish_location = jsonObj["finishRamLocation"].toString().toUInt(nullptr, 16);


    // Imprimir los valores extraídos
    qDebug() << "Ram Size:" << ramSize / 1024 / 1024 / 1024 << "GB";
    qDebug() << "Ram file:" << ramRouteFile;
    qDebug() << "disassembly file:" << disassemblyRouteFile;
    qDebug() << "campaign route:" << campaignRoute;
    qDebug() << "Result location:" << result_location;
    qDebug() << "Finish location:" << finish_location;

    return 0;
}
