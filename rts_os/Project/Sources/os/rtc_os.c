#include "rtc_os.h"

static uint8_t TASK_COUNTER;
static Task Task_list[TASK_LIMIT];

void init_tasks(){
  uint8_t index;
  for(index = 0; index < TASK_LIMIT; index++){
      // Initialize the task
      Task_list[index].autostart = FALSE;
      Task_list[index].priority  = 0;
      Task_list[index].pc_start  = 0;
      Task_list[index].pc_actual = 0;
      Task_list[index].sp_start  = 0;
      Task_list[index].sp_actual = 0;
      Task_list[index].status = TASK_IDLE; 
  }
  TASK_COUNTER=0;
}

void add_task(_fptr funct, Bool autostart, uint8_t priority){
  // Prevent adding more tasks than we can allocate
  if(TASK_COUNTER >= TASK_LIMIT)
    //Error too many tasks
    return;
  
  // Initialize the task
  Task_list[TASK_COUNTER].autostart = autostart;
  Task_list[TASK_COUNTER].priority  = priority;
  Task_list[TASK_COUNTER].pc_start  = funct;
   
  // Autostart tasks must be set to READY
  if(autostart)
    Task_list[TASK_COUNTER].status = TASK_READY;
 
  TASK_COUNTER++;
}

void activate_task(_fptr ptask){
  uint8_t index;
 /* // Prevent array out of bounds 
  if(task_id >= TASK_LIMIT)
    return;
  
  // Task index is valid, but no task is present
  if(Task_list[task_id].pc_start == 0)
    return; //*/
  
  for(index = 0; index < TASK_COUNTER; index++){
      // Initialize the task
      
      if( Task_list[index].status == TASK_RUNNING)
          Task_list[index].status = TASK_READY; 
      
      
      if(Task_list[index].pc_start  == ptask)
         Task_list[index].status = TASK_READY; 
  }
  //Task_list[task_id] = TASK_READY; 
}

void task_scheduler(void){ 
      uint8_t index;
      uint8_t task_HP_index=0;
      //Task Task_list_temp [TASK_COUNTER];
      for(index = 0; index < TASK_COUNTER; index++){
           
            if( Task_list[index].status == TASK_READY && (Task_list[index].priority > Task_list[task_HP_index].priority))     
                task_HP_index=index; 
      }
      Task_list[task_HP_index].status = TASK_RUNNING;  
}