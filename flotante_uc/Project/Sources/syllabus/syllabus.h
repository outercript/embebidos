/////////////////////////////////////////////////////////////////////////////////////////
//
// Spanish Text To Syllables Converter MC9S12XEP100
//
// --------------------------------------------------------------------------------------
// Author : Gibran Gonzalez
// Last Modified : Oct 20, 2012
// 
/////////////////////////////////////////////////////////////////////////////////////////

#include <hidef.h>

//extern unsigned char index;
/*
extern unsigned char [17][3] uns = {"br","bl","cr","cl","dr","fr","fl","gr",
			                              "gl","kr","ll","pr","pl","tr","rr","ch","tl"};
*/

extern struct syllable{
  unsigned char ch[5];
  unsigned char size;
};

static unsigned char uns[35] = {'b','r','b','l','c','r','c','l','d',
                                'r','f','r','f','l','g','r','g','l',
                                'k','r','l','l','p','r','p','l','t',
                                'r','r','r','c','h','t','l'};

Bool isVowel(unsigned char x);
Bool isLetter(unsigned char L);
Bool is_CCV_next(unsigned char word[30], unsigned char size, unsigned char index);
struct syllable case_1(unsigned char word[30], unsigned char size, unsigned char *index, unsigned char L1);
struct syllable case_2(unsigned char word[30], unsigned char size, unsigned char *index, unsigned char L1, unsigned char L2);
struct syllable case_3(unsigned char word[30], unsigned char size, unsigned char *index, unsigned char L1, unsigned char L2);
void text2speech(unsigned char word[30]);
void send_syllable(struct syllable syl);