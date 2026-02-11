ENTRY(_start)

MEMORY
{
    RAM (rwx) : ORIGIN = 0x00000000, LENGTH = 128K
}

SECTIONS
{
    /* Dirección actual empieza en RAM */
    . = ORIGIN(RAM);

    /* Código */
    .text : 
    {
        *(.text*)
    } > RAM

    /* Constantes */
    .rodata :
    {
        *(.rodata*)
    } > RAM

    /* Datos inicializados */
    .data :
    {
        *(.data*)
    } > RAM

    /* Datos no inicializados */
    .bss :
    {
        *(.bss*)
        *(COMMON)
    } > RAM

    /* Alinear a 16 bytes */
    . = ALIGN(16);

    /* Definir tope de stack */
    _stack_top = ORIGIN(RAM) + LENGTH(RAM);
}
