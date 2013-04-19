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
  activate_task_isr(TaskC);
}

interrupt VectorNumber_Vtimovf void TimerOverflow_ISR(void){
    uint8_t index;
    
    //Clear Timer Interrupt Flag
    TIM_TFLG1 |= TIM_TFLG1_C0F_MASK;
    
    // Interrupt code goes here
    
    for(index=0; index < ALARM_COUNTER; index++){
       
       //Check the task to be run
       if(Alarm_list[index].delay > 0)
          Alarm_list[index].delay--;
       
       else
          continue;
       
       // Activate Task
       if(Alarm_list[index].delay == 0){
       
         //Is one shot alarm? 
         if(Alarm_list[index].period) {
             Alarm_list[index].delay = Alarm_list[index].period;
         }
         Task_list[Alarm_list[index].task_id].status = TASK_READY;
       }
    }
    _asm{
      TSX                     ; Guarda el SP en X
      LEAX 9,X                ; Compensa el espacio de las variables
                              ; en el stack y apunta a la direccion 
                              ; del MS byte del PC. (N bytes + 1)
      STX RegisterHolder      ; Guardamos el SP en la variable
    }  
    //Obtenemos la direccion del PC y apuntamos a ella
    *RegisterHolder = (uint16_t)((uint32_t)task_scheduler >> 8) ;
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

void TimerInit(void){

    // Enable Timer
    TIM_TSCR1 = 0x90; // TSCR1 - Enable normal timer
 
    // Enable Timer Overflow Interrupt 
    TIM_TSCR2 |= TIM_TSCR2_TOI_MASK; 
}

void main(void) {

    PeriphInit();
    TimerInit();
    OSInit();
    
    
    add_task(TaskA, 1 | AUTOSTART);
    add_task(TaskB, 2);
    add_task(TaskC, 3);
    add_alarm(TaskB, 1,2);
    for(;;) {
    
      task_scheduler();

    } /* loop forever */
  
} /* please make sure that you never leave main */