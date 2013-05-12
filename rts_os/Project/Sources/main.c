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
    activate_task_isr(PWM_4);
}

interrupt VectorNumber_Vtimch0 void Timer_alarm(void){
    uint8_t index;

    //Clear Timer Interrupt Flag
    TIM_TFLG1 |= TIM_TFLG1_C0F_MASK;
    TIM_TC0 = TIM_TCNT + TICK_TIME;


    //Code to activate Alarm Tasks
    TASK_ACTIVATED = FALSE;
    for(index=0; index < ALARM_COUNTER; index++){

        if(Alarm_list[index].delay > 0)
            Alarm_list[index].delay--;

        //Check the task to be run
        if(Alarm_list[index].delay == 0){

            //Is one shot alarm?
            if(Alarm_list[index].period)
                Alarm_list[index].delay = Alarm_list[index].period;


            Task_list[Alarm_list[index].task_id].status = TASK_READY;
            TASK_ACTIVATED = TRUE;
        }

    }//end for


    _asm{
        TSX                 ; Guarda el SP en X
        LEAX 9,X            ; Compensa el espacio de las variables
                            ; en el stack y apunta a la direccion
                            ; del MS byte del PC. (N bytes + 1)
        STX RegisterHolder  ; Guardamos el SP en la variable
        STX sp_value        ; Guardamos el SP en la variable
    }

    //Obtenemos la direccion del PC y apuntamos a ella
    if(ACTIVE_TASK_ID < TASK_LIMIT && TASK_ACTIVATED)
        Task_list[ACTIVE_TASK_ID].pc_continue = *sp_value;


    if(TASK_ACTIVATED){

        if(PC_STACK_SIZE == 0){
            PC_STACK[0] = (uint16_t)*sp_value;
            PC_STACK_SIZE += 1;
            *RegisterHolder = (uint16_t)((uint32_t)task_scheduler_isr >> 8);
        }

        else if(ACTIVE_TASK_ID < TASK_LIMIT)
            *RegisterHolder = (uint16_t)((uint32_t)task_scheduler_isr >> 8);

        else
            *RegisterHolder = (uint16_t)*sp_value;

    }


    // Guarda el valor del SP antes de haber entrado a activate_task()
    _asm{
        LDX RegisterHolder      ; Guarda en X el valor del SP inicial
        LEAX 2,X                ; Compensa el decremento en el SP
                                ; de la instrucción CALL
        STX sp_value            ; Guarda el valor en el apuntador
    }

    if(ACTIVE_TASK_ID < TASK_LIMIT && TASK_ACTIVATED){

        // Guarda el valor del SP en la estructura
        Task_list[ACTIVE_TASK_ID].sp_continue = (uint16_t)sp_value;

        // Sede el control de la tarea activa
        Task_list[ACTIVE_TASK_ID].status = TASK_READY;
        ACTIVE_TASK_ID = TASK_LIMIT;
    }
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


    TIM_TSCR2_PR0 = 0;
    TIM_TSCR2_PR1 = 0;
    TIM_TSCR2_PR2 = 0;
    TIM_PACTL  = 0x00; // Setup Timer Preset

    // Enable Output compare Port 0
    TIM_TIOS_IOS0 = TRUE;
}


void main(void) {

    PeriphInit();
    TimerInit();
    OSInit();
    
    add_task(PWM_0, 3);
    add_task(PWM_1, 2);
    add_task(PWM_2, 1);
    add_task(PWM_3, 4);
    add_task(PWM_4, 2);
    add_task(PWM_5, 1);
 
    add_alarm(PWM_0, 0, 5); // 1 prendido, 3 apagados 50Hz  %25
    add_alarm(PWM_1, 1, 1); // 1 prendido, 3 apagados 250Hz %25
    add_alarm(PWM_2, 3, 1); // 17 prendidos, apagado 40Hz %70
    add_alarm(PWM_3, 2, 5); // 1 prendido, 4 apagados 40Hz %20
    add_alarm(PWM_4, 4, 1); // 9 prendidos, 1 apagado 100Hz %90
    //add_alarm(PWM_5, 5, 5); // 2 prendidos, 3 apagado 50Hz %40

    task_scheduler();
    for(;;) {
      DisableInterrupts;
      _asm NOP; //Because I can!
      EnableInterrupts;

    } /* loop forever */

} /* please make sure that you never leave main */