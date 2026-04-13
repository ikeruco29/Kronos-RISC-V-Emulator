#ifndef LOADER_H
#define LOADER_H

#include <QFile>
#include <QString>
#include "core/cpu.h"
#include "memory/memory.h"

#include <cstdint>

typedef uint64_t Elf64_Addr;
typedef uint64_t Elf64_Off;
typedef uint16_t Elf64_Half;
typedef uint32_t Elf64_Word;
typedef uint64_t Elf64_Xword;

#pragma pack(push, 1)
struct Elf32_Ehdr {
    unsigned char e_ident[16];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint32_t e_entry;   // <--- Entrypoint (PC)
    uint32_t e_phoff;   // Offset of Program Headers
    uint32_t e_shoff;   // Offset of Section Headers (ignored for this load)
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;   // number of segments
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
};
struct Elf32_Phdr {
    uint32_t p_type;        // if it's 1 (PT_LOAD), load to RAM
    uint32_t p_offset;      // Where the data lives on the file
    uint32_t p_vaddr;       // Virtual address
    uint32_t p_paddr;       // physical address (used on this emu)
    uint32_t p_filesz;      // File size
    uint32_t p_memsz;       // size in RAM (may be greater than p_filesz)
    uint32_t p_flags;
    uint32_t p_align;
};

#pragma pack(pop)

class Loader
{
public:
    Loader(Memory *comp_ram, CPU *comp_cpu, uint32_t comp_ramsize);
    int readELF(QString filename);

private:
    Memory *ram;
    CPU *cpu;
    uint32_t ram_size;

    bool isValidElf(const Elf32_Ehdr& ehdr);
    void loadSegment(QFile &file, const Elf32_Phdr &phdr);
};

#endif // LOADER_H
