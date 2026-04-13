#pragma once
#include <QString>
#include <cstdint>
#include "memory/memory.h"

// Forward declaration de tu clase RAM

// Cabecera ELF32 (32 bytes de e_ident + campos)
struct Elf32_Ehdr {
    uint8_t  e_ident[16];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint32_t e_entry;
    uint32_t e_phoff;
    uint32_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
};

// Program header ELF32
struct Elf32_Phdr {
    uint32_t p_type;
    uint32_t p_offset;
    uint32_t p_vaddr;
    uint32_t p_paddr;
    uint32_t p_filesz;
    uint32_t p_memsz;
    uint32_t p_flags;
    uint32_t p_align;
};

// Valores relevantes
static constexpr uint32_t PT_LOAD      = 1;
static constexpr uint16_t EM_RISCV     = 243;
static constexpr uint8_t  ELFCLASS32   = 1;
static constexpr uint8_t  ELFDATA2LSB  = 1;   // Little-endian

class Loader {
public:
    explicit Loader(Memory &ram);

    // Carga el ELF32 en la RAM emulada.
    // Devuelve 0 si OK, -1 en error.
    // Tras la carga, entryPoint contiene e_entry.
    int readELF(QString filename);

    uint32_t entryPoint() const { return m_entryPoint; }
    QString  programName() const { return m_programName; }

private:
    Memory   &m_ram;
    uint32_t m_entryPoint = 0;
    QString  m_programName;

    // Helpers de lectura little-endian desde buffer raw
    static uint16_t read16le(const uint8_t *p);
    static uint32_t read32le(const uint8_t *p);
};
