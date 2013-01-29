#ifndef FLOAT_LIB
#define FLOAT_LIB
#define TRUE  1
#define FALSE 0
#define LITTLE_ENDIAN 1
#endif

typedef unsigned char           uint8_t;
typedef unsigned int            uint16_t;
typedef unsigned long int       uint32_t;

typedef union {
    uint8_t  byte[4];
    uint16_t dbyte[2];
    uint32_t lword;
    float    fword;
    struct {
        unsigned int Mantissa: 23;
        unsigned int Exponent: 8;
        unsigned int Sign: 1;
    } My;
}Float32;

void float32_print(Float32 p);
Float32 float32_shiftL8(Float32 x);
Float32 float32_shiftR8(Float32 x);
Float32 float32_divide(Float32 x, Float32 y);