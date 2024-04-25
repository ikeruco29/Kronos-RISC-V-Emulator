#include "computer.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

Computer::Computer(int RAM_SIZE) : ram(RAM(RAM_SIZE)), cpu(CPU(&ram)), ram_size(RAM_SIZE) {};
Computer::Computer(int RAM_SIZE, QTextEdit *termb)
    : ram(RAM(RAM_SIZE)), cpu(CPU(&ram)), ram_size(RAM_SIZE)
    , terminalBox(termb)
{};

Computer::~Computer() {}

void Computer::On(bool *pasoapaso) {

    if(*pasoapaso == true){
        while (*pasoapaso != true) {
            cpu.fetch(ram.readWord(cpu.pc));
            cpu.decode();
            cpu.execute();

            cpu.pc += 4;

            //cycles -= 1;
        }
    } else {
        while (*pasoapaso != true) {
            cpu.fetch(ram.readWord(cpu.pc));
            cpu.decode();
            cpu.execute();

            cpu.pc += 4;

            //cycles -= 1;
        }
    }
}

//void Computer::

int Computer::LoadProgram(std::string filename) {

    // Abrir el archivo en modo binario
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo: " << filename << std::endl;
        return 1;
    }

    // Leer el archivo byte por byte
    uint8_t byte;
    int i = 0;
    while (file.read(reinterpret_cast<char*>(&byte), sizeof(uint8_t))) {
        ram.writeByte(i, byte);
        i++;
    }

    file.close();
    return 0;
}

int Computer::LoadCampaign(std::string filename) {
    // Abrir el archivo JSON
    QFile file(QString::fromStdString(filename));
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "No se pudo abrir el archivo JSON";
        return 1;
    }

    // Leer todo el contenido del archivo JSON
    QByteArray jsonData = file.readAll();
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
    QString programPath = jsonObj["program"].toString();
    int expectedResult = jsonObj["expected_result"].toInt();
    int expectedInstructions = jsonObj["expected_instructions"].toInt();
    QJsonArray injectionsArray = jsonObj["injections"].toArray();

    // Almacenar las inyecciones en un vector de vectores de enteros
    std::vector<std::vector<int>> injections;
    for (const auto& injection : injectionsArray) {
        std::vector<int> injectionVec;
        QJsonArray injectionArray = injection.toArray();
        for (const auto& value : injectionArray) {
            injectionVec.push_back(value.toInt());
        }
        injections.push_back(injectionVec);
    }

    // Imprimir los valores extraídos
    qDebug() << "Programa:" << programPath;
    qDebug() << "Resultado Esperado:" << expectedResult;
    qDebug() << "Instrucciones Esperadas:" << expectedInstructions;
    qDebug() << "Inyecciones:" << injections;

    return 0;
}

std::string Computer::exportRam(){
    std::stringstream ss;
    ss << "          00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\r\n\n";
    ss << "00000000  ";
    int count = 1;
    for(int i = 0; i < ram_size; i++){
        ss << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(ram.readByte(i)) << " ";

        if (count == 16) {
            ss << "\r\n";
            ss << std::hex << std::setw(8) << std::setfill('0') << i+1;
            ss << "  ";
            count = 0;
        }

        count++;
    }

    rambox->setText(QString::fromStdString(ss.str()));

    return ss.str();
}
