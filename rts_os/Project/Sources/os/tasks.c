#include "tasks.h"
#include "rtc_os.h"
//#include <MC9S12XEP100.h> 


void TaskA(void){
  chain_task(TaskB); 
}

void TaskB(void){
  PRE++;
  terminate_task();
}

void TaskC(void){
  _asm{
    NOP
  }
  terminate_task();
}

void TaskD(void){
  _asm{
    NOP
  }
  terminate_task();
}


