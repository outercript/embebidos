#include <MC9S12XEP100.h>
#include "rtc_os.h"


extern volatile unsigned char PWM0;
extern volatile unsigned char PWM1;
extern volatile unsigned char PWM2;
extern volatile unsigned char PWM3;
extern volatile unsigned char PWM4;
extern volatile unsigned char PWM5;

void PWM_0(void);
void PWM_1(void);
void PWM_2(void);
void PWM_3(void);
void PWM_4(void);
void PWM_5(void);