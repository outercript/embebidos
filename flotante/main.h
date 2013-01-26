#ifndef FLOAT_LIB
#define FLOAT_LIB

// your declarations here
enum{
    FALSE,
    TRUE
};

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

#endif
