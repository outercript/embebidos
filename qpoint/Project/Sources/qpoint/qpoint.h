#ifndef QPOINT_LIB
#define QPOINT_LIB

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
}Qpoint;



typedef struct{
  char op;
  Qpoint op1;
  Qpoint op2;
}Qpoint_eqn;


// Function Prototypes
Qpoint qpoint_shiftL8(Qpoint x);
Qpoint qpoint_shiftR8(Qpoint x);
uint8_t qpoint_isZero(Qpoint p);
Qpoint qpoint_multiply(Qpoint a, Qpoint b);
Qpoint qpoint_addition_substraction(Qpoint x, Qpoint y, uint8_t operation);
Qpoint qpoint_divide(Qpoint x, Qpoint y);

#endif
