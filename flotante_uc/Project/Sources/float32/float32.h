#ifndef FLOAT_LIB
#define FLOAT_LIB
#endif

typedef unsigned char  uint8_t ;
typedef unsigned short uint16_t;
typedef unsigned long  uint32_t;

typedef struct{
  uint8_t b3;
  uint8_t b2;
  uint8_t b1;
  uint8_t b0;
}byte_wa;
typedef struct{
  uint16_t b1;
  uint16_t b0;
}dbyte_wa;


typedef union {
    byte_wa  byte;
    dbyte_wa dbyte;
    uint32_t lword;
}Float32;



typedef struct{
  char op;
  Float32 op1;
  Float32 op2;
}Float32_eqn;


// Function Prototypes
Float32 float32_shiftL8(Float32 x);
Float32 float32_shiftR8(Float32 x);
uint8_t float32_isZero(Float32 p);
Float32 float32_multiply(Float32 a, Float32 b);
Float32 float32_addition_substraction(Float32 x, Float32 y, uint8_t operation);
Float32 float32_divide(Float32 x, Float32 y);

//Freescale WA stuff
#define getSign(x) ((x.byte.b3&0x80)>0)
uint8_t  getExponent(Float32 a);
uint32_t getMantissa(Float32 a);

void setSign(Float32 *a, uint8_t value);
void setExponent(Float32 *a, uint8_t  value);
void setMantissa(Float32 *a, uint32_t value);
