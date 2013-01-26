#include <stdio.h>
#include <stdlib.h>
#include "main.h"

void print_word_size(){
    printf("Char : %u\n", sizeof(char));
    printf("Short : %u\n", sizeof(short));
    printf("Integer : %u\n", sizeof(int));
    printf("Long : %u\n", sizeof(long));
}

void float32_print(Float32 p){
    printf("[ Value    ]: %f\n", p.fword);
    debug("[ Hex Val  ]: %08x\n", p.lword);
    debug("[ Sign     ]: %u\n", (int)p.My.Sign);
    debug("[ Exponent ]: %u\n", (int)p.My.Exponent);
    debug("[ Mantissa ]: %08x\n\n", (int)p.My.Mantissa);
}

uint8_t float32_isZero(Float32 p){
    if(p.My.Exponent == 0 && p.My.Mantissa == 0)
       return TRUE;

    else
        return FALSE;
}

Float32 float32_multiply(Float32 a, Float32 b){
    uint8_t index;
    Float32 Am, Bm, Cr;

    // Test for Zero
    if(float32_isZero(a) || float32_isZero(b))
        return Cr;

    float32_print(a);
    float32_print(b);

    Am.lword = a.My.Mantissa;
    Bm.lword = b.My.Mantissa;
    Cr.lword = 0;

    // Calculate Sign
    Cr.My.Sign = a.My.Sign ^ b.My.Sign;

    // Insert Implicit Ones
    Am.byte[2] |= 0x80;
    Bm.byte[2] |= 0x80;

    debug("A > %08x\n", Am.lword);
    debug("B > %08x\n", Bm.lword);

    // Calculate Mantissa
    Cr.lword = Am.dbyte[0] * Bm.dbyte[0];
    debug("1er Mult = %08x\n", Cr.lword);
    Cr.dbyte[0] = Cr.dbyte[1];
    Cr.dbyte[1] = 0;
    debug("Shift 16 = %08x\n", Cr.lword);

    Cr.lword += Am.dbyte[0] * Bm.byte[2];
    debug("2da Mult = %08x\n", Cr.lword);

    Cr.lword += Bm.dbyte[0] * Am.byte[2];
    debug("3er Mult = %08x\n", Cr.lword);

    Cr.dbyte[1] += Am.byte[2] * Bm.byte[2];
    debug("4ta Mult = %08x\n", Cr.lword);

    for(index = 0; index < 3; index++)
        Cr.byte[index] = Cr.byte[index + 1];
    Cr.byte[3] = 0;
    debug("Shift 8  = %08x\n", Cr.lword);

    // Normalize (if needed)
    if(Cr.byte[2] & 0x80){
        Cr.My.Exponent = 1;
    }
    else{
        printf("Ajuste de mantiza!\n");
        Cr.byte[2] <<= 1;
        debug("Shift 1  = %08x\n", Cr.lword);
        Cr.byte[2] = (Cr.byte[1] & 0x80) ? (Cr.byte[2] | 0x01) : (Cr.byte[2] & 0xFE);
        debug("Acarreo  = %08x\n", Cr.lword);
        Cr.dbyte[0] <<= 1;
        debug("Ultimo   = %08x\n", Cr.lword);
        Cr.My.Exponent = 0;
    }

    // Calculate Exponent
    Cr.My.Exponent += (uint16_t)a.My.Exponent + b.My.Exponent - 127;
    return Cr;
}

int usage(){
    printf("Invalid Usage!! flotante.exe num1 num2\n");
    exit(1);
}

int main(int argc, char *argv[]) {
    printf("Argumentos : %u\n", argc);

    // Parse Cmdline Arguments
    if(argc != 3)
		usage();

    //

    Float32 T1, T2, T3;
    T1.lword = strtof(argv[1],NULL);
    T2.lword = strtof(argv[2],NULL);

    T3 = float32_multiply(T1, T2);
    float32_print(T3);

	return 0;
}
