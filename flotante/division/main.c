#include <stdio.h>
#include <stdlib.h>
#include "main.h"

int main(int argc, char *argv[])
{
    Float32 x,y;

    x.fword = 80 ;
    y.fword = 10;

    printf("Result of: %f / %f\n", x.fword, y.fword);
    float32_print(float32_divide(x,y));

    // Let Me See WTF happend
    system("PAUSE");
    return 0;
}


/********   Print The Float32 Structure   *********/

void float32_print(Float32 p){
    printf("\n[ Value    ]: %f\n", p.fword);
    printf("[ Hex Val  ]: %08x\n", p.lword);
    printf("[ Sign     ]: %u\n", (int)p.My.Sign);
    printf("[ Exponent ]: %u\n", (int)p.My.Exponent);
    printf("[ Mantissa ]: %06x\n\n", (int)p.My.Mantissa);
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
        printf("[ SPECIAL ] Same Mantissa: %06x\n", divisor.lword);
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
        	printf("[ ADJUST ] Cocient: Shift Right\n");
            ++tmp_exp;
            cocient.lword >>= 1;
        }
        else{
        	printf("[ ADJUST ] Cocient: Shift left\n");
            --tmp_exp;
            cocient.lword <<= 1;
        }
    }

    //Place It All Together
    cocient.My.Exponent = tmp_exp;
    cocient.My.Sign     = x.My.Sign ^ y.My.Sign;
    return cocient;
}
