#ifndef FLOAT_LIB
#define FLOAT_LIB

//#define STANDALONE
//#define DEBUG

#ifdef DEBUG
    #define debug printf
#else
    #define debug( ... ) asm("nop")
#endif


// Custom data structures
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


// Function Prototypes
void print_word_size();
void float32_print(Float32 p);
uint8_t float32_isZero(Float32 p);
Float32 float32_multiply(Float32 a, Float32 b);

int usage();

#endif
