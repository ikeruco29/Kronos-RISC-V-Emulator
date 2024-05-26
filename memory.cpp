#include "memory.h"
#include <thread>
#include <vector>

Memory::Memory(uint32_t MEMORY_SIZE){
    iMemorySize = MEMORY_SIZE;
    memory = new uint8_t[MEMORY_SIZE];

    this->reset();

    this->iRomStartAddr = 0;
};


Memory::~Memory(){};

void Memory::writeByte(uint32_t addr, int8_t data) {

    if(addr <= iMemorySize && addr >= 0){
        memory[addr] = data;
    }

}
void Memory::writeHalf(uint32_t addr, int16_t data) {

    if(addr <= iMemorySize && addr >= 0){
        memory[addr] = data >> 8;
        memory[addr + 1] = data;
    }


}
void Memory::writeWord(uint32_t addr, int32_t data){

    if(addr <= iMemorySize && addr >= 0){
        // 00 00 00 00
        memory[addr] = data >> 24;
        memory[addr + 1] = data >> 16;
        memory[addr + 2] = data >> 8;
        memory[addr + 3] = data;
    }

};

uint8_t Memory::readByte(uint32_t addr) {
    if(addr <= iMemorySize && addr >= 0)
        return memory[addr];
    else
        return 0;
}
uint16_t Memory::readHalf(uint32_t addr) {
    if(addr <= iMemorySize && addr >= 0){
        uint16_t half = (memory[addr + 2] << 8) | memory[addr + 3];
        return half;
    }else{
        return 0;
    }

}
uint32_t Memory::readWord(uint32_t addr){
    if(addr <= iMemorySize && addr >= 0){
        uint32_t word = (memory[addr] << 24) | (memory[addr + 1] << 16) | (memory[addr + 2] << 8) | memory[addr + 3];
        return word;
    }else
        return 0;
};


void Memory::reset(){

    int numOfThreads = 16;  // Número de hilos. Con 16 va bien, y es un buen rendimiento entre
                            // consumo de recursos y velocidad


    uint32_t subSectionSize = iMemorySize / numOfThreads;

    // Crear un vector para almacenar los hilos
    std::vector<std::thread> hilos;

    // Crear numOfThreads hilos, cada uno procesará un sexto del array
    for (int i = 0; i < numOfThreads; ++i) {
        uint32_t start = i * subSectionSize;
        uint32_t end = (i + 1) * subSectionSize;
        // Si es el último hilo, el rango final será el tamaño total del array
        if (i == numOfThreads - 1) {
            end = iMemorySize;
        }
        // Crear el hilo y pasarle la función resetMemorySection y los parámetros de inicio y fin
        hilos.emplace_back(&Memory::resetMemorySection, this, start, end);
    }

    // Esperar a que todos los hilos terminen
    for (auto& hilo : hilos) {
        hilo.join();
    }

    this->resetIOMemory();

}

// Función ejecutada por cada hilo
void Memory::resetMemorySection(uint32_t inicio, uint32_t fin) {
    // reset del array
    for (uint32_t i = inicio; i < fin; ++i) {
        this->memory[i] = 0xFF;
    }
}

void Memory::resetIOMemory(){
    for (uint32_t i = this->iMemorySize - this->pIo; i < this->iMemorySize; ++i) {
        this->memory[i] = 0x20; // Caracter de espacio en utf8
    }
}

