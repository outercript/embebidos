#include "tasks.h"


void PWM_0(void){
  PORTA_PA0 = (PWM0 <= 0);
  PWM0 = PWM0 >= 3 ? 0 : PWM0 + 1;
  terminate_task(); 
}

void PWM_1(void){
  PORTA_PA1 = (PWM1 <= 1);
  PWM1 = PWM1 >= 6 ? 0 : PWM1 + 1;
  terminate_task();
}

void PWM_2(void){
  PORTA_PA2 = (PWM2 <= 16);
  PWM2 = PWM2 >= 24 ? 0 : PWM2 + 1;
  terminate_task();
}

void PWM_3(void){
  PORTA_PA3 = (PWM3 <= 0);
  PWM3 = PWM3 >= 4 ? 0 : PWM3 + 1;
  terminate_task();
}

void PWM_4(void){
  PORTA_PA4 = (PWM4 <= 8);
  PWM4 = PWM4 >= 9 ? 0 : PWM4 + 1;
  terminate_task();
}

void PWM_5(void){
  PORTA_PA5 = (PWM5 <= 1);
  PWM5 = PWM5 >= 4 ? 0 : PWM5 + 1;
  terminate_task();
}