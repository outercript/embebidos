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

Qpoint qpoint_multiply(Qpoint a, uint8_t Qa, Qpoint b, uint8_t Qb, uint8_t Qr){
    uint32_t    tmp;
    uint8_t     Qtmp, is_negative;
    tmp =   0;
    Qtmp =  0;
    is_negative = 0;

    // Test for Zero
    if(Qa == 0 || Qb == 0)
        return 0;

    // Check Sign and complement operands as needed
    is_negative = (a.dbyte & 0xF0) == (b.dbyte & 0xF0) ? 0 : 1;
    if(a.dbyte & 0xF0)
        a.dbyte = complement(a.dbyte)
    if(b.dbyte & 0xF0)
        b.dbyte = complement(b.dbyte)

    // Get the resultant Q and the multiplication result
    Qtmp = Qa + Qb
    tmp = (uint32_t)a.dbyte * b.dbyte;


    // Normalize the result


    // Calculate Exponent and Sign
    if(is_negative)
        tmp = complement(tmp)

    return tmp;
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
