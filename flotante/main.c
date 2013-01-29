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
    printf("[ Value    ]: %.8f\n", p.fword);
    debug("[ Hex Val  ]: 0x%08X\n", p.lword);
    debug("[ Sign     ]: %u\n", (int)p.My.Sign);
    debug("[ Exponent ]: %u\n", (int)p.My.Exponent);
    debug("[ Mantissa ]: %06x\n\n", (int)p.My.Mantissa);
}

uint8_t float32_isZero(Float32 p){
    if(p.My.Exponent == 0 && p.My.Mantissa == 0)
       return TRUE;
    else
        return FALSE;
}


/*********   Shift Left/Right 8 bits   **********/

Float32 float32_shiftL8(Float32 x){
    x.byte[3] = x.byte[2];
    x.byte[2] = x.byte[1];
    x.byte[1] = x.byte[0];
    x.byte[0] = 0;
    return x;
}

Float32 float32_shiftR8(Float32 x){
    x.byte[0] = x.byte[1];
    x.byte[1] = x.byte[2];
    x.byte[2] = x.byte[3];
    x.byte[3] = 0;
    return x;
}


/*********   Addition/Substraction Algorithm    ************/

Float32 float32_addition_substraction(Float32 x, Float32 y, uint8_t operation){
    Float32 tmp_x, tmp_y, tmp_r ,result;

    // for the uint8_t operation selector 0 means adding, 1 means substracting
    y.My.Sign     = (operation > 0) ^ y.My.Sign;

    tmp_x.lword = x.My.Mantissa;
    tmp_y.lword = y.My.Mantissa;

    tmp_x.lword = x.byte[2] | 0x80;
    tmp_y.lword = y.byte[2] | 0x80;

    if(x.My.Exponent < y.My.Exponent){
        //Shift the mantissa of the smallest operator to align with the biggest.
        tmp_x.lword >>= y.My.Exponent - x.My.Exponent;

        //Set the biggest exponent to the result exponent
        result.My.Exponent = y.My.Exponent;
    }
    else{
        //Shift the mantissa of the smallest operator to align with the biggest.
        tmp_y.lword >>= x.My.Exponent - y.My.Exponent;

        //Set the biggest exponent to the result exponent
        result.My.Exponent = x.My.Exponent;
    }

    if(tmp_x.lword >= tmp_y.lword){
        result.My.Sign = x.My.Sign;
        tmp_r.lword = x.My.Sign ^ y.My.Sign ? tmp_x.lword - tmp_y.lword : tmp_x.lword + tmp_y.lword;
    }
    else{
        result.My.Sign = y.My.Sign;
        tmp_r.lword = x.My.Sign ^ y.My.Sign ? tmp_y.lword - tmp_x.lword : tmp_y.lword + tmp_x.lword;
    }

    //Adjust Exponent and Mantissa
    while((tmp_r.byte[2]&0x80) == 0 || tmp_r.byte[3] > 0){
        if(tmp_r.byte[3] > 0){
            debug("[ ADJUST ] Cocient: Shift Right\n");
            ++result.My.Exponent;
            tmp_r.lword >>= 1;
        }
        else{
            debug("[ ADJUST ] Cocient: Shift left\n");
            --result.My.Exponent;
            tmp_r.lword <<= 1;
        }
    }

    result.My.Mantissa = tmp_r.My.Mantissa;
    return result;
}


/*********   Multiplication Algorithm    ************/

Float32 float32_multiply(Float32 a, Float32 b){
    uint8_t index;
    Float32 Am, Bm, Cr;

    Cr.lword = 0;
    #ifdef DEBUG
        float32_print(a);
        float32_print(b);
    #endif

    // Test for Zero
    if(float32_isZero(a) || float32_isZero(b)){
        debug("Multiply by Zero\n");
        return Cr;
    }

    Am.lword = a.My.Mantissa;
    Bm.lword = b.My.Mantissa;

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

    Cr = float32_shiftL8(Cr);
    debug("Shift 8  = %08x\n", Cr.lword);

    // Normalize (if needed)
    if(Cr.byte[2] & 0x80){
        Cr.My.Exponent = 1;
    }
    else{
        debug("Ajuste de mantiza!\n");
        Cr.lword <<= 1;
        debug("Ultimo   = %08x\n", Cr.lword);
        Cr.My.Exponent = 0;
    }

    // Calculate Exponent and Sign
    Cr.My.Exponent += (uint16_t)a.My.Exponent + b.My.Exponent - 127;
    Cr.My.Sign = (a.My.Sign ^ b.My.Sign);

    return Cr;
}


/*********   Division Algorithm    ************/

Float32 float32_divide(Float32 x, Float32 y ){
    //Performs the x/y float division
    Float32 dividend, divisor, cocient;
    uint16_t tmp_exp;
    uint8_t  i;

    // Make Implicit 1, Explicit!
    dividend.lword = x.My.Mantissa;
    divisor.lword  = y.My.Mantissa;
    dividend.byte[2] |= 0x80;
    divisor.byte[2]  |= 0x80;
    cocient.lword  = 0;

    //Calculate Exponent
    tmp_exp = 126 + x.My.Exponent - y.My.Exponent;

    //Perform Division
    if(dividend.lword == divisor.lword){
        debug("[ SPECIAL ] Same Mantissa: %06x\n", divisor.lword);
        ++tmp_exp;
        cocient.byte[2] = 0x80;
    }
    else{
        for(i=3 ; i > 0 ; --i){
            dividend = float32_shiftL8(dividend);
            //Weak point here maybe
            cocient.lword  = float32_shiftL8(cocient).lword |(dividend.lword/divisor.lword);
            dividend.lword = dividend.lword % divisor.lword;
            if(dividend.lword == 0) //The job is done =)
                break;
        }
    }
    //Adjust Exponent and Mantissa
    while((cocient.byte[2]&0x80) == 0 || cocient.byte[3] > 0){
        if(cocient.byte[3] > 0){
            debug("[ ADJUST ] Cocient: Shift Right\n");
            ++tmp_exp;
            cocient.lword >>= 1;
        }
        else{
            debug("[ ADJUST ] Cocient: Shift left\n");
            --tmp_exp;
            cocient.lword <<= 1;
        }
    }

    //Place It All Together
    cocient.My.Exponent = tmp_exp;
    cocient.My.Sign     = x.My.Sign ^ y.My.Sign;
    return cocient;
}


int usage(){
    printf("Invalid Usage!! flotante.exe num1 num2\n");
    return 1;
}

#ifdef STANDALONE
int main(int argc, char *argv[]) {
    printf("Argumentos : %u\n", argc);

    // Parse Cmdline Arguments
    if(argc != 3)
		usage();

    Float32 T1, T2, T3;
    T1.fword = strtof(argv[1], NULL);
    T2.fword = strtof(argv[2], NULL);

    T3 = float32_multiply(T1, T2);
    float32_print(T3);

	return 0;
}
#endif
