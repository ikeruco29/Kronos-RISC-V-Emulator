#include "core/loader.h"

#include <QFile>
#include <QDebug>
#include <cstring>

Loader::Loader(Memory &ram) : m_ram(ram) {}

// ── helpers ──────────────────────────────────────────────────────────────────

uint16_t Loader::read16le(const uint8_t *p) {
    return static_cast<uint16_t>(p[0]) |
           (static_cast<uint16_t>(p[1]) << 8);
}

uint32_t Loader::read32le(const uint8_t *p) {
    return static_cast<uint32_t>(p[0])        |
           (static_cast<uint32_t>(p[1]) << 8)  |
           (static_cast<uint32_t>(p[2]) << 16) |
           (static_cast<uint32_t>(p[3]) << 24);
}

// ── carga principal ───────────────────────────────────────────────────────────

int Loader::readELF(QString filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qCritical() << "ElfLoader: no se puede abrir" << filename;
        return -1;
    }

    // ── 1. Leer y validar cabecera ELF ───────────────────────────────────────
    const int EHDR_SIZE = 52;   // sizeof(Elf32_Ehdr) en el ABI
    QByteArray ehdrBuf = file.read(EHDR_SIZE);
    if (ehdrBuf.size() < EHDR_SIZE) {
        qCritical() << "ElfLoader: archivo demasiado pequeño para cabecera ELF";
        return -1;
    }

    const uint8_t *e = reinterpret_cast<const uint8_t *>(ehdrBuf.constData());

    // Magic: 0x7F 'E' 'L' 'F'
    if (e[0] != 0x7F || e[1] != 'E' || e[2] != 'L' || e[3] != 'F') {
        qCritical() << "ElfLoader: magic ELF no encontrado";
        return -1;
    }
    if (e[4] != ELFCLASS32) {
        qCritical() << "ElfLoader: solo se admiten ELF32 (EI_CLASS =" << e[4] << ")";
        return -1;
    }
    if (e[5] != ELFDATA2LSB) {
        qCritical() << "ElfLoader: solo little-endian (EI_DATA =" << e[5] << ")";
        return -1;
    }

    Elf32_Ehdr hdr;
    std::memcpy(hdr.e_ident, e, 16);
    hdr.e_type      = read16le(e + 16);
    hdr.e_machine   = read16le(e + 18);
    hdr.e_version   = read32le(e + 20);
    hdr.e_entry     = read32le(e + 24);
    hdr.e_phoff     = read32le(e + 28);
    hdr.e_shoff     = read32le(e + 32);
    hdr.e_flags     = read32le(e + 36);
    hdr.e_ehsize    = read16le(e + 40);
    hdr.e_phentsize = read16le(e + 42);
    hdr.e_phnum     = read16le(e + 44);
    hdr.e_shentsize = read16le(e + 46);
    hdr.e_shnum     = read16le(e + 48);
    hdr.e_shstrndx  = read16le(e + 50);

    if (hdr.e_machine != EM_RISCV) {
        qWarning() << "ElfLoader: e_machine no es RISC-V (valor:" << hdr.e_machine << "), continuando de todas formas";
    }
    if (hdr.e_phnum == 0) {
        qCritical() << "ElfLoader: el ELF no tiene program headers";
        return -1;
    }

    m_programName = filename;
    m_entryPoint  = hdr.e_entry;

    qDebug() << "ElfLoader: entry=0x" + QString::number(hdr.e_entry, 16)
             << "phnum=" << hdr.e_phnum;

    // ── 2. Iterar program headers ─────────────────────────────────────────────
    const int PHDR_SIZE = 32;   // sizeof(Elf32_Phdr) en el ABI

    for (uint16_t i = 0; i < hdr.e_phnum; ++i) {
        qint64 phOffset = static_cast<qint64>(hdr.e_phoff) + i * PHDR_SIZE;
        if (!file.seek(phOffset)) {
            qCritical() << "ElfLoader: seek a program header" << i << "falló";
            return -1;
        }

        QByteArray phBuf = file.read(PHDR_SIZE);
        if (phBuf.size() < PHDR_SIZE) {
            qCritical() << "ElfLoader: program header" << i << "truncado";
            return -1;
        }

        const uint8_t *ph = reinterpret_cast<const uint8_t *>(phBuf.constData());
        Elf32_Phdr phdr;
        phdr.p_type   = read32le(ph +  0);
        phdr.p_offset = read32le(ph +  4);
        phdr.p_vaddr  = read32le(ph +  8);
        phdr.p_paddr  = read32le(ph + 12);
        phdr.p_filesz = read32le(ph + 16);
        phdr.p_memsz  = read32le(ph + 20);
        phdr.p_flags  = read32le(ph + 24);
        phdr.p_align  = read32le(ph + 28);

        // Solo nos interesan los segmentos de tipo PT_LOAD
        if (phdr.p_type != PT_LOAD)
            continue;

        qDebug() << QString("  PT_LOAD paddr=0x%1 filesz=%2 memsz=%3")
                        .arg(phdr.p_paddr, 8, 16, QChar('0'))
                        .arg(phdr.p_filesz)
                        .arg(phdr.p_memsz);

        // ── 3. Copiar p_filesz bytes del archivo a RAM[p_paddr] ───────────────
        if (phdr.p_filesz > 0) {
            if (!file.seek(static_cast<qint64>(phdr.p_offset))) {
                qCritical() << "ElfLoader: seek a segmento" << i << "falló";
                return -1;
            }

            QByteArray segData = file.read(static_cast<qint64>(phdr.p_filesz));
            if (static_cast<uint32_t>(segData.size()) < phdr.p_filesz) {
                qCritical() << "ElfLoader: segmento" << i << "truncado en disco";
                return -1;
            }

            const uint8_t *src = reinterpret_cast<const uint8_t *>(segData.constData());
            for (uint32_t b = 0; b < phdr.p_filesz; ++b) {
                m_ram.writeByte(static_cast<int>(phdr.p_paddr + b), src[b]);
            }
        }

        // ── 4. Rellenar con ceros el resto (sección BSS: memsz > filesz) ──────
        for (uint32_t b = phdr.p_filesz; b < phdr.p_memsz; ++b) {
            m_ram.writeByte(static_cast<int>(phdr.p_paddr + b), 0x00);
        }
    }

    file.close();
    qDebug() << "ElfLoader: carga completada, entry=0x"
                    + QString::number(m_entryPoint, 16);
    return 0;
}
