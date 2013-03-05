#include <stdio.h>
#include <stdlib.h>
#include "qpoint.h"


Qpoint qpoint_shiftL8(Qpoint x){
    x.byte.b1 = x.byte.b0;
    x.byte.b0 = 0;
    return x;
}

Qpoint qpoint_shiftR8(Qpoint x){
    x.byte.b0 = x.byte.b1;
    x.byte.b1 = 0;
    return x;
}

/*********   Addition/Substraction Algorithm    ************/

Qpoint qpoint_addition_substraction(Qpoint x, Qpoint y, uint8_t operation){
    Qpoint tmp_x, tmp_y, tmp_r ,result;

    // for the uint8_t operation selector 0 means adding, 1 means substracting
    setSign( &y , (operation>0) ^ getSign(y) );

    tmp_x.lword = getMantissa(x);
    tmp_y.lword = getMantissa(y);

    tmp_x.byte.b2 |= 0x80;
    tmp_y.byte.b2 |= 0x80;

    tmp_x.lword = qpoint_shiftL8(tmp_x).lword >> 1;
  	tmp_y.lword = qpoint_shiftL8(tmp_y).lword >> 1;

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

Qpoint qpoint_multiply(Qpoint a, Qpoint b){
    uint8_t tmp;
    Qpoint Am, Bm, Cr;

    Cr.lword = 0;

    // Test for Zero
    if(qpoint_isZero(a) || qpoint_isZero(b)){
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

    Cr = qpoint_shiftR8(Cr);

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

Qpoint qpoint_divide(Qpoint x, Qpoint y ){
	//Performs the x/y float division
    Qpoint dividend, divisor, cocient;
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
            dividend = qpoint_shiftL8(dividend);
            //Weak point here maybe
            cocient.lword  = qpoint_shiftL8(cocient).lword |(dividend.lword/divisor.lword);
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
