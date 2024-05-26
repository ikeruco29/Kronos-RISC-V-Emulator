/*
    ESTE ARCHIVO SE ENCARGA DE GENERAR FUNCIONES QUE APLICAN
    EL LLAMADO ENDIANESS.
*/

#include "endian.h"

// Voltea 2 bytes
uint16_t FlipHalf(uint16_t half) {

    uint16_t byte1 = (half & 0xFF) << 8;
    uint8_t byte2 = half >> 8;

    return byte1 | byte2;
}

// Voltea 4 bytes
uint32_t FlipWord(uint32_t word) {
    // ff a0 bb aa
    // a0 ff aa bb -
    // aa bb a0 ff

    uint32_t half1 = FlipHalf(word) << 16;
    uint16_t half2 = FlipHalf(word >> 16);

    return half1 | half2;
}
