#include "tasks.h"
#include "rtc_os.h"


void TaskA(void){
  activate_task(TaskF);
  terminate_task(); 
}

void TaskB(void){
  activate_task(TaskC);
  terminate_task();
}

void TaskC(void){
  activate_task(TaskD);
  terminate_task();
}

void TaskD(void){
  activate_task(TaskE);
  terminate_task();
}

void TaskE(void){
  activate_task(TaskA);
  terminate_task();
}

void TaskF(void){
  activate_task(TaskB);
  terminate_task();
}