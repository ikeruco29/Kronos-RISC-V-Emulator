#include "gui/mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

const QString CONFIG_FILE = "./config.json";

uint ramSize, finish_location, result_location, romAddrAlloc;
bool updateRamInRealTime;
QString disassemblyRouteFile, ramRouteFile, campaignRoute, linkerRoute;

// EDITOR CONFIG
int tabsize, fontsize;
QString font;


int readConfigFile();

int main(int argc, char *argv[])
{
    readConfigFile();

    QApplication a(argc, argv);


    int fontId = QFontDatabase::addApplicationFont(":/resources/fonts/mononoki Bold Nerd Font Complete.ttf");
    if (fontId == -1) {
        qWarning() << "Font could not be loaded";
    } else {
        QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
        QFont font(fontFamily);
        a.setFont(font);
    }


    MainWindow w = MainWindow(nullptr, nullptr, {tabsize, font, fontsize});

    Computer computer = Computer(ramSize);

    computer.ram.iRomStartAddr = romAddrAlloc;  // Location of ROM

    w.computer = &computer;
    w.disassemblyFileRoute = disassemblyRouteFile;
    w.ramFileRoute = ramRouteFile;
    w.campaignGeneratorRoute = campaignRoute;
    w.linkerRoute = linkerRoute;

    // Control locations to the final result and to finish the program execution
    w.RESULT_LOCATION = result_location;
    w.FINISH_LOCATION = finish_location;

    w.updateRamInRealTime = updateRamInRealTime;

    w.show();

    return a.exec();
}

int readConfigFile(){

    QFile file(CONFIG_FILE);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "No se pudo abrir el archivo JSON";
        return 1;
    }

    QByteArray jsonData = file.readAll();

    file.close();

    // Create a JSON document with all extracted data
    QJsonParseError error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &error);

    if (jsonDoc.isNull()) {
        qWarning() << "Error while trying to parse json:" << error.errorString();
        return 1;
    }

    if (!jsonDoc.isObject()) {
        qWarning() << "File doesn't have a JSON object";
        return 1;
    }

    QJsonObject jsonObj = jsonDoc.object(); // json root

    // Extract JSON object values
    ramSize = jsonObj["ramSize"].toInt() * 1024 * 1024 * 1024;
    disassemblyRouteFile = jsonObj["disassemblyFileRoute"].toString();
    ramRouteFile = jsonObj["ramFileRoute"].toString();
    campaignRoute = jsonObj["campaignGeneratorRoute"].toString();
    result_location = jsonObj["resultRamLocation"].toString().toUInt(nullptr, 16);
    finish_location = jsonObj["finishRamLocation"].toString().toUInt(nullptr, 16);
    romAddrAlloc = jsonObj["romAddressAllocation"].toString().toUInt(nullptr, 16);
    updateRamInRealTime = jsonObj["updateRamInRealTime"].toBool();
    linkerRoute = jsonObj["linkerRoute"].toString();

    QJsonObject editorObj = jsonObj["editor"].toObject();
    tabsize = editorObj["tabsize"].toInt();
    font = editorObj["font"].toString();
    fontsize = editorObj["fontSize"].toInt();


    qDebug() << "Ram Size:" << ramSize / 1024 / 1024 / 1024 << "GB";
    qDebug() << "Ram file:" << ramRouteFile;
    qDebug() << "disassembly file:" << disassemblyRouteFile;
    qDebug() << "Linker file:" << linkerRoute;
    qDebug() << "campaign route:" << campaignRoute;
    qDebug() << "Result location:" << result_location;
    qDebug() << "Finish location:" << finish_location;
    qDebug() << "Update RAM in real time: " << updateRamInRealTime;
    qDebug() << "\n" << "Font size: " << editorObj;

    return 0;
}
