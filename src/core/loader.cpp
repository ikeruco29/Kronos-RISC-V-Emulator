#pragma once

#include "core/loader.h"
#include <QDebug>

Loader::Loader(Memory *comp_ram, CPU *comp_cpu, uint32_t comp_ramsize) : ram(comp_ram), cpu(comp_cpu), ram_size(comp_ramsize)
{}

int Loader::readELF(QString filename) {
    Elf32_Ehdr ehdr;
    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Error: Couldn't open the file.";
    }

    if (file.read(reinterpret_cast<char*>(&ehdr), sizeof(ehdr)) != sizeof(ehdr)) {
        qDebug() << "Error: Could not read the ELF's header.";
    }

    if(isValidElf(ehdr)){
        qDebug() << "Error: File is not a supported ELF file.";
    }

    if (ehdr.e_machine != 243) { // 243 represents RISC V machines
        qDebug() << "Error: Target device is not RISC V.";
    }

    file.seek(ehdr.e_phoff);

    for (int i = 0; i < ehdr.e_phnum; i++) {
        Elf32_Phdr phdr;

        if (file.read(reinterpret_cast<char*>(&phdr), sizeof(phdr)) != sizeof(phdr)) {
            throw std::runtime_error("Error leyendo program header");
        }

        if (phdr.p_type == 1) { // PT_LOAD should be 1
            loadSegment(file, phdr);
        }
    }

    // =======================
    // 4. Inicializar CPU
    // =======================
    cpu->pc = ehdr.e_entry;
    cpu->registers[1] = ram_size - 4; // setting stack pointer (register 2)

    return 0;
}

bool Loader::isValidElf(const Elf32_Ehdr& ehdr) {
    return (ehdr.e_ident[0] == 0x7F &&
            ehdr.e_ident[1] == 'E' &&
            ehdr.e_ident[2] == 'L' &&
            ehdr.e_ident[3] == 'F');
}

void Loader::loadSegment(QFile& file, const Elf32_Phdr& phdr) {
    // Limit validation
    if (phdr.p_vaddr + phdr.p_memsz > ram_size) {
        throw std::runtime_error("Segment outside of memory");
    }

    file.seek(phdr.p_offset);
    QByteArray data = file.read(phdr.p_filesz);

    if (data.size() != (int)phdr.p_filesz) {
        throw std::runtime_error("Error while trying to read file");
    }

    // Copy to mem
    for (uint32_t i = 0; i < phdr.p_filesz; i++) {
        ram->writeByte(phdr.p_vaddr + i, static_cast<uint8_t>(data[i]));
    }

    // Zero-fill (.bss)
    for (uint32_t i = phdr.p_filesz; i < phdr.p_memsz; i++) {
        ram->writeByte(phdr.p_vaddr + i, 0);
    }
}
