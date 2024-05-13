#include "computer.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

Computer::Computer(int RAM_SIZE) : ram(Memory(RAM_SIZE)), cpu(CPU(&ram)), ram_size(RAM_SIZE) {};
Computer::Computer(int RAM_SIZE, QTextEdit *termb)
    : ram(Memory(RAM_SIZE)), cpu(CPU(&ram)), ram_size(RAM_SIZE)
    , terminalBox(termb)
{};

Computer::~Computer() {}

void Computer::executeProgram() {

}

void Computer::reset(){
    cpu.reset();
    ram.reset();

    campaign.expectedInstructions = 0;
    campaign.expectedResult = 0;
    campaign.programPath = "";
}

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

    Campaign camp;
    camp.expectedInstructions = expectedInstructions;
    camp.programPath = programPath;
    camp.expectedResult = expectedResult;
    camp.injections = injections;

    return 0;
}

std::string Computer::showRam(int page){
    std::stringstream ss;

    for(int i = 0; i < (16 * 8); i++){

        if (i % 16 == 0) {
            ss << "\r\n";
            ss << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << i + (page);
            ss << "  ";
        }

        ss << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<int>(ram.readByte(i + (page))) << " ";
    }

    //ramBox->setPlainText(QString::fromStdString(ss.str()));

    return ss.str();
}
std::string Computer::showRegisters(){
    std::stringstream ss;

    ss << "PC:  " << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << cpu.pc;
    ss << " ";
    ss << "IR:  " << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << cpu.ir << "\r\n\r\n";

    for(int i = 0; i < 16; i++){
        if(i < 10)
            ss << "X" << i << ":  ";
        else
            ss << "X" << std::dec << i << ": ";

        ss << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << cpu.registers[i] << " ";
        ss << "X" << std::dec << (i + 16) << ": ";
        ss << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << cpu.registers[i + 16] << "\r\n";
    }
    return ss.str();
}

std::string Computer::showDisassembly(){
    std::string str = cpu.disassembly.back();
    return str;
}

std::string Computer::exportDisassembly(){
    std::vector<std::string> st = cpu.disassembly;

    std::stringstream ss;

    for (size_t i = 0; i < st.size(); ++i) {
        ss << st[i] << std::endl;
    }

    return ss.str();
}


int Computer::executeCampaign(){
    LoadProgram(campaign.programPath.toStdString());
}
