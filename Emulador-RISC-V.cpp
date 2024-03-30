// Emulador-RISC-V.cpp: define el punto de entrada de la aplicación.
//

#include "Emulador-RISC-V.h"

using namespace std;

#include "Computer/Computer.h"
#include <iostream>

int main() {
    int RAM_SIZE = 64, SCREEN_HEIGHT, SCREEN_WIDTH;
    bool DEBUG;


    // Recoger configuración
    cout << "Leyendo configuración..." << endl;
    //try {
    //    YAML::Node config = YAML::LoadFile("config.yml");

    //    cout << "Aplicando configuración..." << endl;

    //    RAM_SIZE = config["RAM_SIZE"].as<int>();
    //    SCREEN_WIDTH = config["SCREEN_WIDTH"].as<int>();
    //    SCREEN_HEIGHT = config["SCREEN_HEIGHT"].as<int>();
    //    DEBUG = config["DEBUG"].as<bool>();

    //    // Imprimir los datos leídos
    //    cout << "DEBUG: " << DEBUG << endl;

    //}
    //catch (const YAML::Exception& e) {
    //    cerr << "Error al leer el archivo YAML: " << e.what() << endl;
    //}

    Computer computer = Computer(RAM_SIZE);

    computer.ram.write(0, 0x03200193);
    
    computer.ram.write(1, 0xfb500113);
    
    computer.ram.write(2, 0x00310233);

    computer.ram.write(3, 0x00400283);
    computer.ram.write(4, 0x0000FFFF);

    computer.On(4);
    // cout << hex << computer.ram.read(0x000000) << endl;



    return 0;
}