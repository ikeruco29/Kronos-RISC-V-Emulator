/* linker.ld */
ENTRY(_start)

MEMORY {
    ROM (rx) : ORIGIN = 0x00000000, LENGTH = 64K
    RAM (rwx): ORIGIN = 0x00010000, LENGTH = 64K
}

SECTIONS {
    .text : {
        *(.text.start)   /* asegura que _start va primero */
        *(.text*)
    } > ROM

    .data : {
        *(.data*)
        *(.rodata*)
    } > RAM

    .bss : {
        *(.bss*)
        *(COMMON)
    } > RAM
}
