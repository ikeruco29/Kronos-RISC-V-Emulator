#include "computer.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <fstream>
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>

// Constructor
Computer::Computer(int RAM_SIZE) : ram(Memory(RAM_SIZE)), cpu(CPU(&ram)), ram_size(RAM_SIZE) {};
Computer::Computer(int RAM_SIZE, QTextEdit *termb)
    : ram(Memory(RAM_SIZE)), cpu(CPU(&ram)), ram_size(RAM_SIZE)
    , terminalBox(termb)
{};

Computer::~Computer() {}

// Función que resetea el ordenador
void Computer::reset(){
    cpu.reset();
    ram.reset();
}

// Esta función carga el programa en memoria. En concreto
// donde indica la variable iRomAddrStart
int Computer::LoadProgram(std::string filename) {

    // Abrir el archivo en modo binario
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo: " << filename << std::endl;
        return 1;
    }

    this->programName = filename;

    // Leer el archivo byte por byte
    uint8_t byte;
    int i = ram.iRomStartAddr;  // Índice donde empieza el programa cargado
    while (file.read(reinterpret_cast<char*>(&byte), sizeof(uint8_t))) {
        ram.writeByte(i, byte);
        i++;
    }

    file.close();
    return 0;
}

// Esta función carga una campaña de inyección de errores
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

    // Obtener el objeto JSON completo
    QJsonObject jsonObj = jsonDoc.object();

    // Extraer valores del objeto JSON
    QString programPath = jsonObj["program"].toString();
    int expectedResult = jsonObj["expectedResult"].toInt();
    int expectedInstructions = jsonObj["expectedInstructions"].toInt();
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
    qDebug() << "Campaña cargada";

    // Iguala las variables a la campaña de la clase
    campaign.expectedInstructions = expectedInstructions;
    campaign.programPath = programPath;
    campaign.expectedResult = expectedResult;
    campaign.injections = injections;

    return 0;
}

// Esta función genera en un string el contenido de la RAM para poder
// imprimirlo en la terminal
std::string Computer::showRam(int page){
    std::stringstream ss;

    for(int i = 0; i < (16 * 8); i++){

        if (i % 16 == 0) {  // Para que salte de línea cada 16 valores
            ss << "\r\n";
            ss << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << i + (page);    // Para imprimirlo en hexadecimal
            ss << "  ";
        }
        // Lee de la memoria y lo guarda en el string. Todos los métodos std:: que aparecen son para
        // que mantenga el valor en hexadecimal
        ss << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<int>(ram.readByte(i + (page))) << " ";
    }

    return ss.str();
}

// Esta genera un string igual que el anterior método, pero en este caso con los
// registro para imprimirlos luego en la caja de registros de la
// interfaz
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

// Genera otro string para el desensamblado. El desensamblado está en la
// variable cpu.disassembly.
std::string Computer::showDisassembly(){
    std::string str = cpu.disassembly.back();   // Pop de una pila
    return str;
}

// Función que exporta el desensamblado en un archivo de texto plano.
// Esta función solo genera la cadena. La generación del archivo se hace en el archivo
// mainwindow.cpp
std::string Computer::exportDisassembly(){
    std::vector<std::string> st = cpu.disassembly;

    std::stringstream ss;

    for (size_t i = 0; i < st.size(); ++i) {
        ss << st[i] << std::endl;
    }

    return ss.str();
}

// Esta función genera el texto que renderiza la terminal. La lógica es la siguiente:
//      1. En cada línea de la terminal caben 75 caracteres.
//      2. En la terminal, caben 20 lineas.
//      3. La memoría I/O, que es la que lee la terminal, ocupa 1500 (guardada en ram.pIo)
QString Computer::showVRAMLine(int line)
{
    QString lineString;
    uint start = (line * 75) + (ram.iMemorySize - ram.pIo);
    uint end = start + 75;


    for (uint i = start; i < end; ++i) {
        // Append a la variable lineString de lo que haya en memoria en esa posición
        lineString += QChar(ram.readByte(i));
    }


    return lineString;
}
