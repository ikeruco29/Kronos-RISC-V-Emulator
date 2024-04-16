// Emulador-RISC-V.cpp: define el punto de entrada de la aplicación.
//

#include "Emulador-RISC-V.h"

using namespace std;

#include "Computer/Computer.h"
#include <iostream>

int main() {
    uint32_t RAM_SIZE = 0x80003021;
    bool DEBUG;


    // Recoger configuración
    cout << "Leyendo configuración..." << endl;
    //try {
    //    YAML::Node config = YAML::LoadFile("config.yml");

    //    cout << "Aplicando configuración..." << endl;

    //    RAM_SIZE = config["RAM_SIZE"].as<int>();
    //    DEBUG = config["DEBUG"].as<bool>();

    //    // Imprimir los datos leídos
    //    cout << "DEBUG: " << DEBUG << endl;

    //}
    //catch (const YAML::Exception& e) {
    //    cerr << "Error al leer el archivo YAML: " << e.what() << endl;
    //}

    Computer computer = Computer(RAM_SIZE);

    if(computer.LoadProgram("C:/Users/ikeru/Downloads/SHA_RV32I.bin") == 1){
        exit(-1);
    };

    computer.On(0);
    computer.ram.showRam();

    return 0;
}