#ifndef LOADER_H
#define LOADER_H

#include <cstdint>

typedef uint64_t Elf64_Addr;
typedef uint64_t Elf64_Off;
typedef uint16_t Elf64_Half;
typedef uint32_t Elf64_Word;
typedef uint64_t Elf64_Xword;

#pragma pack(push, 1)
struct Elf64_Ehdr {
    unsigned char e_ident[16];
    Elf64_Half    e_type;
    Elf64_Half    e_machine;
    Elf64_Word    e_version;
    Elf64_Addr    e_entry;    // <--- Entrypoint (PC)
    Elf64_Off     e_phoff;    // Offset of Program Headers
    Elf64_Off     e_shoff;    // Offset of Section Headers (ignored for this load)
    Elf64_Word    e_flags;
    Elf64_Half    e_ehsize;
    Elf64_Half    e_phentsize;
    Elf64_Half    e_phnum;    // number of segments
    Elf64_Half    e_shentsize;
    Elf64_Half    e_shnum;
    Elf64_Half    e_shstrndx;
};

struct Elf64_Phdr {
    Elf64_Word  p_type;     // if it's 1 (PT_LOAD), load to RAM
    Elf64_Word  p_flags;
    Elf64_Off   p_offset;   // Where the data lives on the file
    Elf64_Addr  p_vaddr;    // Virtual address
    Elf64_Addr  p_paddr;    // physical address (used on this emu)
    Elf64_Xword p_filesz;   // File size
    Elf64_Xword p_memsz;    // size in RAM (may be greater than p_filesz)
    Elf64_Xword p_align;
};
#pragma pack(pop)

class loader
{
public:
    loader();
};

#endif // LOADER_H
