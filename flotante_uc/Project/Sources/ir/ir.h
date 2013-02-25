/////////////////////////////////////////////////////////////////////////////////////////
//
// IR Communication Interface (SCI) MC9S12XEP100
//
// --------------------------------------------------------------------------------------
// Author : Oscar Suro
// Last Modified : Sept 10, 2012
// 
/////////////////////////////////////////////////////////////////////////////////////////
#include <hidef.h>

#define IR_PORT PORTA_PA3
#define CARRIER_TIME 402
#define CONTROLER_TIME 1280

#define SONY_1LOW   13
#define SONY_1HIGH  29
#define SONY_SLOW   13
#define SONY_SHIGH  58
#define SONY_0LOW   13
#define SONY_0HIGH  15

#define NEC_1LOW   41
#define NEC_1HIGH  12
#define NEC_SLOW   109 //112
#define NEC_SHIGH  219 //225 Originales
#define NEC_0LOW   13
#define NEC_0HIGH  12
#define NEC_RHIGH  217
#define NEC_RLOW   54


extern Bool sendingBit;
extern Bool toggle;
extern Bool ControlStatus;
extern unsigned int ir_pulseCount;
extern unsigned int ir_idleCount;
extern unsigned int ControlCount;


void Setup_IR(void);
void rawSend(unsigned char buff[30]);

void sendSony(unsigned char buff[30]);
void SIRCS_Send_High(void);
void SIRCS_Send_Low(void);
void SIRCS_Send_Start(void);

void sendNEC(unsigned char buff[30]);
void NEC32_Send_High(void);
void NEC32_Send_Low(void);
void NEC32_Send_Start(void);
void NEC32_Send_Repeat(void);

void IR_sendBit(void);
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////