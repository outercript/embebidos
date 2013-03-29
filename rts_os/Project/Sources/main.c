#include <hidef.h>      /* common defines and macros */
#include <stdio.h>
#include <MC9S12XEP100.h>     /* derivative-specific definitions */

#include <string.h>
#include "sci.h"
#include "tasks.h"
#include "rtc_os.h"
#pragma LINK_INFO DERIVATIVE "MC9S12XEP100"


void OSInit(void){
    init_tasks();
    add_task(TaskA, 3 | AUTOSTART);
    add_task(TaskB, 1);
    add_task(TaskC, 5);
    add_task(TaskD, 4);
    add_task(TaskE, 1);
    add_task(TaskF, 8);  
}


void main(void) {
	
    OSInit();    

    //EnableInterrupts;
    
    for(;;) {
    
      task_scheduler();

    } /* loop forever */
  
} /* please make sure that you never leave main */