#include "rtc_os.h"

static uint8_t TASK_COUNTER;
static Task Task_list[TASK_LIMIT];

void init_tasks(){
  uint8_t index;
  for(index = 0; index < TASK_LIMIT; index++){
      // Initialize the task
      Task_list[TASK_COUNTER].autostart = FALSE;
      Task_list[TASK_COUNTER].priority  = 0;
      Task_list[TASK_COUNTER].pc_start  = 0;
      Task_list[TASK_COUNTER].pc_actual = 0;
      Task_list[TASK_COUNTER].sp_start  = 0;
      Task_list[TASK_COUNTER].sp_actual = 0;
      Task_list[TASK_COUNTER].status = TASK_IDLE; 
  }
}

void add_task(_fptr funct, Bool autostart, uint8_t priority){
  // Prevent adding more tasks than we can allocate
  if(TASK_COUNTER >= TASK_LIMIT)
    return;
  
  // Initialize the task
  Task_list[TASK_COUNTER].autostart = autostart;
  Task_list[TASK_COUNTER].priority  = priority;
  Task_list[TASK_COUNTER].pc_start  = funct;
  Task_list[TASK_COUNTER].pc_actual = 0;
  Task_list[TASK_COUNTER].sp_start  = 0;
  Task_list[TASK_COUNTER].sp_actual = 0;
   
  // Autostart tasks must be set to READY
  if(autostart)
    Task_list[TASK_COUNTER].status = TASK_READY;
  
  else
    Task_list[TASK_COUNTER].status = TASK_IDLE;
 
  TASK_COUNTER++;
}

void activate_task(uint8_t task_id){
  // Prevent array out of bounds 
  if(task_id >= TASK_LIMIT)
    return;
  
  // Task index is valid, but no task is present
  if(Task_list[task_id].pc_start == 0)
    return;
  
  
  Task_list[task_id] = TASK_READY;
  
}