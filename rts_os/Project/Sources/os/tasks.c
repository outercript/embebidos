#include "tasks.h"
#include "rtc_os.h"
//#define PORTA_PA0                   _PORTAB.Overlap_STR.PORTASTR.Bits.PA0
//#include <MC9S12XEP100.h> 


void TaskA(void){
  chain_task(TaskB); 
}

void TaskB(void){
  if(PRE > 0){
    PRE = 0; 
  } else
    PRE = 1;
  terminate_task();
}

void TaskC(void){
  _asm{
    NOP
  }
  if(PRE > 0){
   PRE = 0; 
  } else
   PRE = 1;
  terminate_task();
}

void TaskD(void){
  _asm{
    NOP
  }
  terminate_task();
}


