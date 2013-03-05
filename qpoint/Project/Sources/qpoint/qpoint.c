#include <stdio.h>
#include <stdlib.h>
#include "float32.h"


/*----------------------------------------------------
    WA for shitty freescale religious code thingys
-----------------------------------------------------*/

/* Getters */

    uint8_t getExponent(Float32 a){
        a.byte.b0 = (a.byte.b3<<1)|((a.byte.b2&0x80)>0);
        return a.byte.b0;
    }
    
    uint32_t getMantissa(Float32 a){
        a.byte.b3 = 0;
        a.byte.b2 = a.byte.b2&0x7F;
        return a.lword;
    }

/* Setters */

    void setSign(Float32 *a, uint8_t value){
        (*a).byte.b3 = value > 0 ? (*a).byte.b3|0x80 : (*a).byte.b3&0x7F ;
    }
    
    void setExponent(Float32 *a, uint8_t value){
        
        (*a).byte.b3 = ((*a).byte.b3&0x80)|(value>>1);
        (*a).byte.b2 = ((*a).byte.b2&0x7F)|(value<<7);
        
    }

    void setMantissa(Float32 *a, uint32_t value){
        Float32 tmp;
        tmp.lword = value;
        (*a).byte.b2  = (((*a).byte.b2)&0x80)|(tmp.byte.b2&0x7F);
        (*a).dbyte.b0 = tmp.dbyte.b0;
    }

/*----------------------------------------------------
    End of Freescale shitty religious code thingys WA
-----------------------------------------------------*/

Float32 float32_shiftL8(Float32 x){
    x.byte.b3 = x.byte.b2;
    x.byte.b2 = x.byte.b1;
    x.byte.b1 = x.byte.b0;
    x.byte.b0 = 0;
    return x;
}

Float32 float32_shiftR8(Float32 x){
    x.byte.b0 = x.byte.b1;
    x.byte.b1 = x.byte.b2;
    x.byte.b2 = x.byte.b3;
    x.byte.b3 = 0;
    return x;
}


uint8_t float32_isZero(Float32 p){
    if(getExponent(p) == 0 && getMantissa(p) == 0)
       return TRUE;
    else
        return FALSE;
}

/*********   Addition/Substraction Algorithm    ************/

Float32 float32_addition_substraction(Float32 x, Float32 y, uint8_t operation){
    Float32 tmp_x, tmp_y, tmp_r ,result;
    
    // for the uint8_t operation selector 0 means adding, 1 means substracting
    setSign( &y , (operation>0) ^ getSign(y) );

    tmp_x.lword = getMantissa(x);
    tmp_y.lword = getMantissa(y);

    tmp_x.byte.b2 |= 0x80;
    tmp_y.byte.b2 |= 0x80;
    
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
    while((tmp_r.byte.b2&0x80) == 0 || tmp_r.byte.b3 > 0){
        if(tmp_r.byte.b3 > 0){
            setExponent( &result , getExponent(result)+1 );
            tmp_r.lword >>= 1;
        }
        else{
            setExponent( &result , getExponent(result)-1 );
            tmp_r.lword <<= 1;
        }
    }

    setMantissa( &result , getMantissa(tmp_r));
    return result;
}


/*********   Multiplication Algorithm    ************/

Float32 float32_multiply(Float32 a, Float32 b){
    uint8_t tmp;
    Float32 Am, Bm, Cr;

    Cr.lword = 0;
    
    // Test for Zero
    if(float32_isZero(a) || float32_isZero(b)){
        setSign( &Cr, getSign(a) ^ getSign(b) );
        return Cr;
    }

    Am.lword = getMantissa(a);
    Bm.lword = getMantissa(b);

    // Insert Implicit Ones
    Am.byte.b2 |= 0x80;
    Bm.byte.b2 |= 0x80;
                   
    // Calculate Mantissa
    Cr.lword = (uint32_t)Am.dbyte.b0 * Bm.dbyte.b0;
    Cr.dbyte.b0 = Cr.dbyte.b1;
    Cr.dbyte.b1 = 0;

    Cr.lword += (uint32_t)Am.dbyte.b0 * Bm.byte.b2;
    Cr.lword += (uint32_t)Bm.dbyte.b0 * Am.byte.b2;

    Cr.dbyte.b1 += Am.byte.b2 * Bm.byte.b2;

    // Save the Less Significant Byte,
    // will need it when mantissa is adjusted
    tmp = Cr.byte.b0;

    Cr = float32_shiftR8(Cr);

    // Normalize (if needed)
    if(Cr.byte.b2 & 0x80){
        setExponent( &Cr, 1 );
    }
    else{
        Cr.lword <<= 1;
        Cr.byte.b0 = (tmp & 0x80) ? Cr.byte.b0 | 0x01 : Cr.byte.b0 & 0xFE;
        setExponent( &Cr, 0 );
    }

    // Calculate Exponent and Sign
    setExponent( &Cr, (uint16_t)getExponent(Cr) + getExponent(a) + getExponent(b) - 127 );
    setSign( &Cr, getSign(a) ^ getSign(b) );

    return Cr;
}


/*********   Division Algorithm    ************/

Float32 float32_divide(Float32 x, Float32 y ){
	//Performs the x/y float division
    Float32 dividend, divisor, cocient;
    uint8_t tmp_exp;
    uint8_t  i;

    // Make Implicit 1, Explicit!
    dividend.lword = getMantissa(x); 
    divisor.lword  = getMantissa(y); 
    dividend.byte.b2 |= 0x80;
    divisor.byte.b2  |= 0x80;
    cocient.lword  = 0;

    //Calculate Exponent
    tmp_exp = (uint8_t)((uint16_t)(126 + getExponent(x) - getExponent(y)));

    //Perform Division
    if(dividend.lword == divisor.lword){
        ++tmp_exp;
        cocient.byte.b2 = 0x80; 
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
    while((cocient.byte.b2&0x80) == 0 || cocient.byte.b3 > 0){
        if(cocient.byte.b3 > 0){
            ++tmp_exp;
            cocient.lword >>= 1;
        }
        else{
            --tmp_exp;
            cocient.lword <<= 1;
        }
    }

    //Place It All Together
    setExponent(&cocient, tmp_exp);
    setSign(&cocient, getSign(x) ^ getSign(y) );
    return cocient;
}