/////////////////////////////////////////////////////////////////////////////////////////
//
// Spanish Text To Syllables Converter MC9S12XEP100
//
// --------------------------------------------------------------------------------------
// Author : Gibran Gonzalez
// Last Modified : Oct 20, 2012
// 
/////////////////////////////////////////////////////////////////////////////////////////

#include <MC9S12XEP100.h>     /* derivative-specific definitions */
#include <stdio.h>
#include "syllabus.h"
#include "sci.h"


Bool isVowel(unsigned char x){
	//Char to lower case
	if( x<='Z')
		x += 32;
	return ( x == 'a' || x == 'e' || x == 'i' || x == 'o' || x == 'u' );
}

Bool isLetter(unsigned char L){
	// True if char is in range of the ascii letters
	return ('A' <= L && L <= 'Z') || ('a' <= L && L <= 'z');
}

Bool is_CCV_next(unsigned char word[30], unsigned char size, unsigned char index){
	volatile unsigned char i;
	if ((size - index) > 2){
		if (isVowel(word[index+2])){
			for (i=0; i < 17; i++){
				if(word[index] == uns[(i*2)] && word[index+1] == uns[(i*2 + 1)])
					return TRUE;
			}
		}
	}
	return FALSE;
}

struct syllable case_1(unsigned char word[30], unsigned char size, unsigned char *index, unsigned char L1){
	//index points to the position after L1
	volatile struct syllable syl;
	syl.ch[0]=L1;
	syl.size =1;

	//case: V | #
	if ( *(index) == size )	return syl;

	//case: V | %
	if ( !isLetter(word[*(index)]) ) 	return syl;

	/****** cases for V | VC | VV syllables ******/
	
	//case: VX | #
	if ( (*(index)+1) == size){
		syl.ch[1] = word[*index];
		*index += 1;
		syl.size  = 2;
		return syl;
	}
	
	//error VX | %
	if ( !isLetter(word[*(index)+1]) ){
		//Skip with index pointing to weird char
		syl.ch[1] = word[*index];
		*index += 1;
		syl.size  = 2;
		return syl;
	}

	//case: V | CV
	if ( (!isVowel(word[*(index)]) && isVowel(word[*(index)+1])) || is_CCV_next(word, size, *(index)) )
		return syl;
/*	
	//case: V | CCV
	if ( is_CCV_next(word, size, *(index)) ) return syl;
*/
	//case: VX | CV
	if ( (!isVowel(word[*(index)+1]) && isVowel(word[*(index)+2])) || is_CCV_next(word, size, *(index)+1) ){
		syl.ch[1] = word[*index];
		*index += 1;
		syl.size  = 2;
		return syl;
	}
/*
	//case: VX | CCV
	if ( is_CCV_next(word, size, *(index)+1) ){
		syl.ch[1] = word[*index];
		syl.size  = 2;
		return syl;
	}
*/
	//case: VVC | #
	if ( *(index)+2 == size ){
		syl.ch[1] = word[*index];
		*index += 1;
		syl.ch[2] = word[*index];
		*index += 1;
		syl.size  = 3;
		return syl;
	}
	
	//Default
	syl.ch[1] = word[*index];
	*index += 1;
	syl.ch[2] = word[*index];
	*index += 1;
	syl.size  = 3;
	return syl;	
}

struct syllable case_2(unsigned char word[30], unsigned char size, unsigned char *index, unsigned char L1, unsigned char L2){
	//index points to the position after L2
	volatile struct syllable syl;
	syl.ch[0]=L1;
	syl.ch[1]=L2;
	syl.size =2;

	//case: CV | #
	if ( *(index) == size ) return syl;

	//case: CV | %
	if ( !isLetter(word[*(index)]) ) return syl;

	//case: CVX | #
	if ( *(index)+1 == size ){	  
		syl.ch[2] = word[*index++];
		*index += 1;
		syl.size  = 3;
		return syl;
	}

	//case: CVX | %
	if ( !isLetter(word[*(index)+1]) ){
		syl.ch[2] = word[*index];
		*index += 1;
		syl.size  = 3;
		return syl;
	}

	//case: CV | CV
	if ( (!isVowel(word[*(index)]) && isVowel(word[*(index)+1])) || is_CCV_next(word, size, *(index)) ) return syl;
/*
	//case: CV | CCV
	if ( is_CCV_next(word, size, *(index)) ) return syl;
*/
	//case: CVXX | #
	if ( *(index)+2 == size ){
		syl.ch[2] = word[*index];
		*index += 1;
		syl.ch[3] = word[*index];
		*index += 1;
		syl.size  = 4;
		return syl;
	}

	//case: CVXX | %
	if ( !isLetter(word[*(index)+2]) ){
		syl.ch[2] = word[*index];
		*index += 1;
		syl.ch[3] = word[*index];
		*index += 1;
		syl.size  = 4;
		return syl;
	}

	//case: CVX | CV | CCV
	if ( (!isVowel(word[*(index)+1]) && isVowel(word[*(index)+2])) || is_CCV_next(word, size, *(index)+1) ){
		syl.ch[2] = word[*index];
		*index += 1;
		syl.size  = 3;
		return syl;
	}
/*
	//case: CVX | CCV
	if ( is_CCV_next(word, size, *(index)+1) ){
		syl.ch[2] = word[*index];
		syl.size  = 3;
		return syl;
	}
*/
	//index CVXX|XX exists ?
	if ( *(index)+3 < size ){
		//case: CVXX | CV | CCV
		if ( (!isVowel(word[*(index)+2]) && isVowel(word[*(index)+3])) || is_CCV_next(word, size, *(index)+2) ){
			syl.ch[2] = word[*index];
			*index += 1;
			syl.ch[3] = word[*index];
			*index += 1;
			syl.size  = 4;
			return syl;
		}
	}

	//case: CVVVC | #
	if ( *(index)+3 == size ){
		syl.ch[2] = word[*index];
		*index += 1;
		syl.ch[3] = word[*index];
		*index += 1;
		syl.ch[4] = word[*index];
		*index += 1;
		syl.size  = 5;
		return syl;
	}
	if ( isVowel(word[*(index)]) && isVowel(word[*(index)+1]) && !isVowel(word[*(index)+2]) ){
		syl.ch[2] = word[*index];
		*index += 1;
		syl.ch[3] = word[*index];
		*index += 1;
		syl.ch[4] = word[*index];
		*index += 1;
		syl.size  = 5;
		return syl;
	}
	
	//default
	return syl;
}

struct syllable case_3(unsigned char word[30], unsigned char size, unsigned char *index, unsigned char L1, unsigned char L2){
	//index points to the position after L2
	volatile struct syllable syl;
	syl.ch[0]=L1;
	syl.ch[1]=L2;
	syl.size=2;

	//case: CC | #
	if ( *(index) == size ) return syl;
	
	//case: CC | %
	if ( !isLetter(word[*(index)]) ) return syl;

	//valid letters for this case CCV | XXX
	if ( isVowel(word[*(index)]) ){
		//move pointer after CCV and update syl since all returns will include CCV
		syl.ch[2] = word[*index];
		*index += 1;
		syl.size  = 3;
		
		//case: CCV | #
		if ( *(index) == size ) return syl;
		
		//case: CCV | %
		if ( !isLetter(word[*(index)]) ) return syl;
		
		//cases: CCVX | #
		if ( *(index)+1 == size ){
			syl.ch[3] = word[*index];
			*index += 1;
			syl.size = 4;
			return syl;
		}
		
		//cases: CCVX | %
		if ( !isLetter(word[*(index)+1]) ){
			syl.ch[3] = word[*index];
			*index += 1;
			syl.size  = 4;
			return syl;
		}
		
		/* Now we verified that positions CCV | XX exists */
		//case: CCV | CV
		if ( (!isVowel(word[*(index)]) && isVowel(word[*(index)+1])) || is_CCV_next(word, size, *(index)) )
			return syl;
/*
		//case: CCV | CCV
		if is_CCV_next(word, size, index):
			return index, syl
*/
		//case: CCVXX | #
		if ( *(index)+2 == size ){
			syl.ch[3] = word[*index];
			*index += 1;
			syl.ch[4] = word[*index];
			*index += 1;
			syl.size  = 5;
			return syl;
		}
		
		//case: CCVXX | %
		if ( !isLetter(word[*(index)+2]) ){
			syl.ch[3] = word[*index];
			*index += 1;
			syl.ch[4] = word[*index];
			*index += 1;
			syl.size  = 5;
			return syl;
		}
		
		/* Verified that CCVX | XX exists */
		//case: CCVX | CV
		if ( (!isVowel(word[*(index)+1]) && isVowel(word[*(index)+2])) || is_CCV_next(word, size, *(index)+1) ){
			syl.ch[3] = word[*index];
			*index += 1;
			syl.size  = 4;
			return syl;
		}
/*
		//case: CCVX | CCV
		if is_CCV_next(word, size, index+1):
			return index+1, syl+word[index]
*/
		//default case: CCVXC
		if ( !isVowel(word[*(index)+1]) ){
			syl.ch[3] = word[*index];
			*index += 1;
			syl.ch[4] = word[*index];
			*index += 1;
			syl.size  = 5;
			return syl;
		}
		else{
			//syllable is CCV followed by syllable VV or V-V
			return syl;
		}
	}
}

void text2speech(unsigned char word[30]){
	volatile unsigned char index;
	volatile unsigned char size;
	volatile unsigned char L1;
	volatile unsigned char L2;
	volatile struct syllable syl;
	
	index = 0;
	size  = 30;
	
	while ( index < size ){
	
		L1 = word[index];
		
		if ( !isLetter(L1) ){
			index++;
			//Space between words
			continue;
		}
/*
		if ( index != 0 && index != size-1)
			parsed += "-"
*/
		if ( isVowel(L1) ){
			//print "Case 1. V"
			index++;
			syl = case_1(word, size, &(index), L1);
			send_syllable(syl);
			continue;
		}
		
		//Hang in there baby
		if ( size == index+1 ){
			//Play sound L1 which is a C and is the last char
			index++;
			syl.ch[0] = L1;
			syl.size  = 1;
			send_syllable(syl);
			break;
		}

		L2 = word[index+1];

		if (isVowel(L2)){
			//print "Case 2. CV"
			index += 2;
			syl = case_2(word, size, &index, L1, L2);
			send_syllable(syl);
			continue;
		}
		else if ( isLetter(L2) ){
			//print "Case 3. CC"
			index += 2;
			syl = case_3(word, size, &index, L1, L2);
			send_syllable(syl);
			continue;
		}
		else{
			//play sound L1 which is a C alone
			index++;
			syl.ch[0] = L1;
			syl.size  = 1;
			send_syllable(syl);
			continue;
		}
	}
}

void send_syllable(struct syllable syl){
  volatile unsigned char i;
  volatile unsigned char coso[10];
  
  for (i=0; i<7; i++){
      if(i < syl.size) 
          coso[i] = syl.ch[i];
      else 
          coso[i] = ' ';
  }
  coso[7] = '\r';
  coso[8] = '\n';
  coso[9] = '\0';
  SCIOpenCommunication(SCI_0);
  SendString(SCI_0, coso);
 
}