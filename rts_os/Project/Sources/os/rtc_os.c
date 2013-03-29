#include "rtc_os.h"

static uint8_t  TASK_COUNTER;
static Task Task_list[TASK_LIMIT];

void init_tasks(void){
  uint8_t index;
  for(index = 0; index < TASK_LIMIT; index++){
      // Initialize the task
      Task_list[index].autostart = FALSE;
      Task_list[index].priority  = 0;
      Task_list[index].pc_start  = 0;
      Task_list[index].sp_start  = 0;
      Task_list[index].pc_continue = 0;
      Task_list[index].sp_continue = 0;
      Task_list[index].status = TASK_IDLE; 
  }
  TASK_COUNTER=0;
}

void add_task(_fptr funct, uint8_t args){
  
  // Prevent adding more tasks than we can allocate
  if(TASK_COUNTER >= TASK_LIMIT)
    //Error too many tasks
    return;
  
  // Initialize the task
  Task_list[TASK_COUNTER].autostart = args&0x80;
  Task_list[TASK_COUNTER].priority  = args&0x7F;
  Task_list[TASK_COUNTER].pc_start  = funct;
   
  // Autostart tasks must be set to READY
  if(args&0x80)
    Task_list[TASK_COUNTER].status = TASK_READY;
 
  TASK_COUNTER++;
}

void activate_task(_fptr ptask){
  uint8_t index;
  uint16_t **start_sp_value;
  
  //Obtenemos la direccion del stack donde se encuentra
  //guardado el PC de retorno, sin contar la pagina
  _asm{
    TSX                     ; Guarda el SP en X
    LEAX 9,X                ; Compensa el espacio de las variables
                            ; en el stack y apunta a la direccion 
                            ; del MS byte del PC. (N bytes + 1)
    STX start_sp_value      ; Guardamos el SP en la variable
  }
  
  //Obtenemos la direccion del PC y apuntamos a ella
  Task_list[ACTIVE_TASK_ID].pc_continue = *start_sp_value;
 
  // Guarda el valor del SP antes de haber entrado a activate_task()
  _asm{
    LDX start_sp_value      ; Guarda en X el valor del SP inicial
    LEAX 2,X                ; Compensa el decremento en el SP 
                            ; de la instrucción CALL
    STX  start_sp_value     ; Guarda el valor en el apuntador
  }
  
  // Guarda el valor del SP en la estructura
  Task_list[ACTIVE_TASK_ID].sp_continue = start_sp_value;
  
  // Sede el control de la tarea activa
  Task_list[ACTIVE_TASK_ID].status = TASK_READY;
  
  for(index = 0; index < TASK_COUNTER; index++){
  
      // Buscamos la tarea en la lista de tareas... U don't say!
      if(Task_list[index].pc_start  == ptask){ 
         Task_list[index].status = TASK_READY;
         break;
      }
  } 
  task_scheduler();
}

void task_scheduler(void){ 
  uint8_t index;
  uint8_t task_HP_index;  
  uint8_t task_HP_priority;

  task_HP_index = 255;  // Ponemos un valor invalido
  task_HP_priority = 0; // Ponemos la prioridad mas baja

  // Obtener la tarea con la prioridad  mas alta
  for(index = 0; index < TASK_COUNTER; index++){
        if(Task_list[index].status == TASK_READY && 
          (Task_list[index].priority >= task_HP_priority)){
            task_HP_index = index;
            task_HP_priority = Task_list[index].priority;    
          }
  }

  // Verificar que es un valor valido
  if(task_HP_index > TASK_COUNTER)
    return;

  // Pos corrale mijo que va tarde  
  run_task(task_HP_index);
      
}
                  
void run_task(uint8_t task_id){
      Task_list[task_id].status = TASK_RUNNING;
      ACTIVE_TASK_ID = task_id;
       
      if(Task_list[task_id].pc_continue){
         RegisterHolder = Task_list[task_id].sp_start;
         // Despues de esto perderemos nuestras variables locales
         _asm{
            LDS RegisterHolder                 ; Regresa el SP al lugar en que se
                                               ; econtraba dentro del task
         }
         // Solo usar globales
         RegisterHolder = Task_list[ACTIVE_TASK_ID].pc_continue;
         _asm{
            LDX RegisterHolder    ; Carga en X el PC del task donde debe regresar
            JMP 0,X               ; Salta al PC del TASK
         }
      } 
      else{
          // Manda a llamar el inicio de la funcion
          _asm{
            TSX                 ; Obtener el SP en X
            LEAX -3,X           ; Ajusta el valor al SP inicial de la funcion
            STX RegisterHolder  ; Transfierelo a la variable temporal
          }
          // Guarda el SP inicial de la funcion a llamar
          Task_list[task_id].sp_start = RegisterHolder;
          (*Task_list[task_id].pc_start)();
      }
      Task_list[task_id].sp_start = 0;
      task_scheduler();
}

void terminate_task(void){
  Task_list[ACTIVE_TASK_ID].status = TASK_IDLE;
  Task_list[ACTIVE_TASK_ID].pc_continue = 0;
  Task_list[ACTIVE_TASK_ID].sp_continue = 0;
  RegisterHolder = Task_list[ACTIVE_TASK_ID].sp_start; 
  _asm{
    LDS RegisterHolder          ; Mueve el SP al inicio de la funcion
    RTC                         ; Vamos al siguente valor del stack
  }
}