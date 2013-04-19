#include "tasks.h"
#include "rtc_os.h"


void TaskA(void){
  chain_task(TaskB); 
}

void TaskB(void){
  _asm{
    NOP
  }
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
