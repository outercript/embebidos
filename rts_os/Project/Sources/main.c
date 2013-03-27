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
    add_task(TaskA, AUTOSTART, 0);
    add_task(TaskB,!AUTOSTART, 1);  
}


void main(void) {
	
    OSInit();    

    //EnableInterrupts;
    
    for(;;) {
    
		   
    } /* loop forever */
  
} /* please make sure that you never leave main */