#include <stdio.h>
#include <stdlib.h>
#include "flotante.h"

void print_word_size(){
    printf("Char  : %u\n", (uint8_t)sizeof(char));
    printf("Short : %u\n", (uint8_t)sizeof(short));
    printf("Int   : %u\n", (uint8_t)sizeof(int));
    printf("Long  : %u\n", (uint8_t)sizeof(long));
}

void float32_print(Float32 p){
    printf("[ Value    ]: %.8f\n", p.fword);
    debug("[ Hex Val  ]: 0x%08X\n", p.lword);
    debug("[ Sign     ]: %u\n", (int)p.My.Sign);
    debug("[ Exponent ]: %u\n", (int)p.My.Exponent);
    debug("[ Mantissa ]: %06x\n\n", (int)p.My.Mantissa);
}

/*----------------------------------------------------
    WA for shitty freescale religious code thingys
-----------------------------------------------------*/

/* Getters */

uint8_t getExponent(Float32 a){
    a.byte[0] = (a.byte[3]<<1)|((a.byte[2]&0x80)>0);
    return a.byte[0];
}

uint32_t getMantissa(Float32 a){
    a.byte[3] = 0;
    a.byte[2] = a.byte[2]&0x7F;
    return a.lword;
}

/* Setters */

void setSign(Float32 *a, uint8_t value){
    (*a).byte[3] = value > 0 ? (*a).byte[3]|0x80 : (*a).byte[3]&0x7F ;
}

void setExponent(Float32 *a, uint8_t value){

    (*a).byte[3] = ((*a).byte[3]&0x80)|(value>>1);
    (*a).byte[2] = ((*a).byte[2]&0x7F)|(value<<7);

}

void setMantissa(Float32 *a, uint32_t value){
    Float32 tmp;
    tmp.lword = value;
    (*a).byte[2]  = (((*a).byte[2])&0x80)|(tmp.byte[2]&0x7F);
    (*a).dbyte[0] = tmp.dbyte[0];
}

/*----------------------------------------------------
    End of Freescale shitty religious code thingys WA
-----------------------------------------------------*/

uint8_t float32_isZero(Float32 p){
    if(getExponent(p) == 0 && getMantissa(p) == 0)
       return TRUE;
    else
        return FALSE;
}

uint8_t float32_compare(Float32 a, Float32 b){
    uint32_t diff;

    // Calculate the difference between the numbers
    diff = (a.lword > b.lword) ? a.lword - b.lword : b.lword - a.lword;

    // Check if difference is below the threshold
    if(diff < FLOAT32_DIFFERENCE_THRESHOLD){
        return FLOAT32_EQUAL;
    }

    else{
        // Give some verbose so we can trust this thing
        debug("Expected   : 0x%08x - [ %.8f ]\n", a.lword, a.fword);
        debug("Result     : 0x%08x - [ %.8f ]\n", b.lword, b.fword);
        debug("Difference : %u\n", diff);
        return FLOAT32_DIFFERENT;
    }
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
uint8_t float32_addition_check(uint32_t x, uint32_t y, uint32_t z){
    Float32 a, b, exp, res;

    a.lword = x;
    b.lword = y;
    exp.lword = z;

    res = float32_addition_substraction(a, b, 0);

    debug("Oper A - 0x%08X \t %e\n", a.lword, a.fword);
    debug("Oper B - 0x%08X \t %e\n", b.lword, b.fword);
    debug("Result - 0x%08X \t %e\n", res.lword, res.fword);

    return float32_compare(exp, res);
}

uint8_t float32_substraction_check(uint32_t x, uint32_t y, uint32_t z){
    Float32 a, b, exp, res;

    a.lword = x;
    b.lword = y;
    exp.lword = z;

    res = float32_addition_substraction(a, b, 1);

    debug("Oper A - 0x%08X \t %e\n", a.lword, a.fword);
    debug("Oper B - 0x%08X \t %e\n", b.lword, b.fword);
    debug("Result - 0x%08X \t %e\n", res.lword, res.fword);

    return float32_compare(exp, res);
}

Float32 float32_addition_substraction(Float32 x, Float32 y, uint8_t operation){
    Float32 tmp_x, tmp_y, tmp_r ,result;

    // for the uint8_t operation selector 0 means adding, 1 means substracting
    setSign( &y , operation ^ getSign(y) );

    tmp_x.lword = getMantissa(x);
    tmp_y.lword = getMantissa(y);

    tmp_x.byte[2] |= 0x80;
    tmp_y.byte[2] |= 0x80;

    tmp_x.lword = float32_shiftL8(tmp_x).lword >> 1;
	tmp_y.lword = float32_shiftL8(tmp_y).lword >> 1;

    if( getExponent(x) < getExponent(y) ){
        //Shift the mantissa of the smallest operator to align with the biggest.
        tmp_x.lword >>= getExponent(y) - getExponent(x);

        //Set the biggest exponent to the result exponent
        setExponent( &result, getExponent(y) );
    }
    else{
        //Shift the mantissa of the smallest operator to align with the biggest.
        tmp_y.lword >>= getExponent(x) - getExponent(y);

        //Set the biggest exponent to the result exponent
        setExponent( &result , getExponent(x) );
    }

    if( tmp_x.lword >= tmp_y.lword ){
        setSign( &result , getSign(x) );
        tmp_r.lword = getSign(x) ^ getSign(y) ? tmp_x.lword - tmp_y.lword : tmp_x.lword + tmp_y.lword;
    }
    else{
        setSign( &result , getSign(y) );
        tmp_r.lword = getSign(x) ^ getSign(y) ? tmp_y.lword - tmp_x.lword : tmp_y.lword + tmp_x.lword;
    }

	tmp_r.lword >>= 7;

    //Adjust Exponent and Mantissa
    while((tmp_r.byte[2]&0x80) == 0 || tmp_r.byte[3] > 0){
        if(tmp_r.byte[3] > 0){
            printf("[ ADJUST ] Cocient: Shift Right\n");
            setExponent( &result , (getExponent(result)+1) );
            tmp_r.lword >>= 1;
			debug("[ Hex Val  ]: 0x%08X\n", tmp_r.lword);
			system("sleep 5");
        }
        else{
            printf("[ ADJUST ] Cocient: Shift left\n");
            setExponent( &result , (getExponent(result)-1) );
            tmp_r.lword <<= 1;
			debug("[ Hex Val  ]: 0x%08X\n", tmp_r.lword);
			system("sleep 5");
        }
    }

    setMantissa( &result , getMantissa(tmp_r));
    printf("OUR RESULT %08x\n",result.lword);
    return result;
}


/*********   Multiplication Algorithm    ************/

Float32 float32_multiply(Float32 a, Float32 b){
    uint8_t tmp;
    Float32 Am, Bm, Cr;

    Cr.lword = 0;
    #ifdef DEBUG
        float32_print(a);
        float32_print(b);
    #endif

    // Test for Zero
    if(float32_isZero(a) || float32_isZero(b)){
        setSign( &Cr, getSign(a) ^ getSign(b) );
        debug("Multiply by Zero\n");
        return Cr;
    }

    Am.lword = getMantissa(a);
    Bm.lword = getMantissa(b);

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

    // Save the Less Significant Byte,
    // will need it when mantissa is adjusted
    tmp = Cr.byte[0];

    Cr = float32_shiftR8(Cr);

    debug("Shift 8  = %08x\n", Cr.lword);

    // Normalize (if needed)
    if(Cr.byte[2] & 0x80){
        setExponent( &Cr, 1 );
    }
    else{
        debug("Ajuste de mantiza!\n");
        Cr.lword <<= 1;
        Cr.byte[0] = (tmp & 0x80) ? Cr.byte[0] | 0x01 : Cr.byte[0] & 0xFE;
        debug("Ultimo   = %08x\n", Cr.lword);
        setExponent( &Cr, 0 );
    }

    // Calculate Exponent and Sign
    setExponent( &Cr, (uint16_t)getExponent(Cr) + getExponent(a) + getExponent(b) - 127 );
    setSign( &Cr, getSign(a) ^ getSign(b) );

    return Cr;
}


/*********   Division Algorithm    ************/

uint8_t float32_divide_check(uint32_t x, uint32_t y, uint32_t z){
    Float32 a, b, c, expected;
    a.lword = x;
    b.lword = y;
    c = float32_divide(a, b);
    expected.lword = z;
    debug("%x08 / %x08\nResult: %x08\n", a.lword, b.lword, c.lword);
    return float32_compare(expected, c);
}

Float32 float32_divide(Float32 x, Float32 y ){
    //Performs the x/y float division
    Float32 dividend, divisor, cocient;
    uint8_t tmp_exp;
    uint8_t  i;

    // Make Implicit 1, Explicit!
    dividend.lword = getMantissa(x);
    divisor.lword  = getMantissa(y);
    dividend.byte[2] |= 0x80;
    divisor.byte[2]  |= 0x80;
    cocient.lword  = 0;

    //Calculate Exponent
    tmp_exp = (uint16_t)126 + getExponent(x) - getExponent(y);

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
            cocient.lword  = float32_shiftL8(cocient).lword +(dividend.lword/divisor.lword);
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
    setExponent( &cocient, tmp_exp );
    setSign( &cocient, getSign(x) ^ getSign(y) );
    return cocient;
}


uint8_t float32_multiply_check(uint32_t x, uint32_t y, uint32_t z){
    Float32 a, b, exp, res;

    a.lword = x;
    b.lword = y;
    exp.lword = z;

    res = float32_multiply(a, b);

    debug("Oper A - 0x%08X \t %e\n", a.lword, a.fword);
    debug("Oper B - 0x%08X \t %e\n", b.lword, b.fword);
    debug("Result - 0x%08X \t %e\n", res.lword, res.fword);

    return float32_compare(exp, res);
}

int usage(){
    debug("Invalid Usage!! flotante.exe oper num1 num2 result\n");
    exit(EXIT_FAILURE);
}

#ifdef STANDALONE
int main(int argc, char *argv[]) {
    uint8_t retcode, opt;
    Float32 OperA, OperB, ExpResult;

    // Parse Cmdline Arguments
    if(argc != 5)
		usage();

    opt = argv[1][0];
    if (opt != 'm' && opt != 'd' && opt != 'a' && opt != 's'){
        debug("Unrecognized Argument %s -", argv[1]);
        usage(argv[0]);
    }

    OperA.lword = strtol(argv[2], NULL, 0);
    OperB.lword = strtol(argv[3], NULL, 0);
    ExpResult.lword = strtol(argv[4], NULL, 0);

    switch(opt){
        case 'a':
            retcode = float32_addition_check(OperA.lword, OperB.lword, ExpResult.lword);
            break;

        case 's':
            retcode = float32_substraction_check(OperA.lword, OperB.lword, ExpResult.lword);
            break;

        case 'm':
            retcode = float32_multiply_check(OperA.lword, OperB.lword, ExpResult.lword);
            break;

        case 'd':
            retcode = float32_divide_check(OperA.lword, OperB.lword, ExpResult.lword);
            break;

        default:
            retcode = 2;
            break;
    }

	return retcode;
}
#endif
