#include <hidef.h>      /* common defines and macros */
#include <stdio.h>
#include <MC9S12XEP100.h>     /* derivative-specific definitions */

#include <string.h>
#include "sci.h"
#include "tasks.h"
#include "rtc_os.h"

#pragma LINK_INFO DERIVATIVE "MC9S12XEP100"


#pragma CODE_SEG __NEAR_SEG NON_BANKED
interrupt VectorNumber_Vportp void PortP_ISR(void){
  PIFP_PIFP0 = 1; 
  /*
  _asm{
    TSX 
    LEAX 8,X
    STX RegisterHolder
  }
  *RegisterHolder = (uint16_t)((uint32_t)task_scheduler >> 8) ; 
  //*/
  activate_task_isr(TaskE);
}

#pragma CODE_SEG DEFAULT

void PeriphInit(void){
    PTP   = 0x00;       // Output 0
    DDRP  = 0x00;       // Configure Port P as inputs
    PIEP_PIEP0 = TRUE;  // Enable Interrupt Port_P0
    PIFP_PIFP0 = TRUE;
    
    PORTA = 0x08;       // Clear Port A
    DDRA  = 0xFF;       // Conf Port A as Output
}

void main(void) {

    PeriphInit();
    OSInit();
    
    add_task(TaskA, 3 | AUTOSTART);
    add_task(TaskB, 1);
    add_task(TaskC, 5);
    add_task(TaskD, 4);
    add_task(TaskE, 1);
    add_task(TaskF, 8);
    
    for(;;) {
    
      task_scheduler();

    } /* loop forever */
  
} /* please make sure that you never leave main */