#include "rtc_os.h"

void OSInit(void){

    init_tasks();
    init_alarms();

    TIM_TIE_C0I = TRUE;
    TIM_TC0 = TIM_TCNT + TICK_TIME;

    PC_STACK_SIZE = 0;
}


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
    ACTIVE_TASK_ID=TASK_LIMIT;
}


void init_alarms(void){
    uint8_t index;

    for(index=0;index < TASK_LIMIT; index++){
        Alarm_list[index].delay = 0;
        Alarm_list[index].period = 0;
        Alarm_list[index].task_id = 255;
    }

    ALARM_COUNTER = 0;
}

void init_mailboxes(void){
    uint8_t index;
    for(index=0;index < TASK_LIMIT; index++){
        Mailbox_list[index].status = MB_EMPTY;
        Mailbox_list[index].sender = 0;
        Mailbox_list[index].receiver = 0;
        Mailbox_list[index].data = 0;
    }
    MAILBOX_COUNTER = 0;
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


void add_alarm(_fptr ptask, uint8_t delay, uint8_t period){
    uint8_t index;

    if( ALARM_COUNTER >= TASK_LIMIT )
        return;

    for(index = 0; index < TASK_COUNTER; index++){
        if(Task_list[index].pc_start == ptask){
            Alarm_list[ALARM_COUNTER].delay = delay;
            Alarm_list[ALARM_COUNTER].period = period;
            Alarm_list[ALARM_COUNTER].task_id = index;
            ALARM_COUNTER++;
            break;
        }
    }
}

uint8_t add_mailbox(_fptr sender, _fptr receiver){
    uint8_t index, sender_id, receiver_id;

    // Prevent array out of bound error
    if(MAILBOX_COUNTER >= TASK_LIMIT)
        return MB_NO_MAILBOX_LEFT;

    sender_id = get_task_id(sender);
    receiver_id = get_task_id(receiver);

    // Validate the sender and receiver task ids
    if(sender_id == TASK_NOT_FOUND || receiver_id == TASK_NOT_FOUND)
        return ERROR;

    // Assigns the sender and receiver to the given mailbox
    Mailbox_list[MAILBOX_COUNTER].sender_id = sender_id;
    Mailbox_list[MAILBOX_COUNTER].receiver_id = receiver_id;

    // Return the index of the mailbox
    return MAILBOX_COUNTER;
}

uint8_t is_mailbox_empty(uint8_t mailbox_id){
    uint8_t retcode;

    // Check we received an existing/valid mailbox
    if(mailbox_id >= MAILBOX_COUNTER)
        return MB_INVALID_MAILBOX;

    // Return True if empty, false otherwise
    retcode = Mailbox_list[mailbox_id].status == MB_EMPTY ? TRUE : FALSE;
    return retcode;
}

uint8_t is_mailbox_full(uint8_t mailbox_id){
    uint8_t retcode;

    // Check we received an existing/valid mailbox
    if(mailbox_id >= MAILBOX_COUNTER)
        return MB_INVALID_MAILBOX;

    // Return True if empty, false otherwise
    retcode = Mailbox_list[mailbox_id].status == MB_FULL ? TRUE : FALSE;
    return retcode;
}

uint8_t write_mailbox(uint8_t mailbox_id, uint8_t *data_ptr){
    uint8_t caller_task_id;
    uint8_t receiver_task_id;

    // Save current active task so we can use that later
    // ie. when scheduler call us back
    DisableInterrupts;
    caller_task_id = ACTIVE_TASK_ID;

    // Check we received an existing/valid mailbox
    if(mailbox_id >= MAILBOX_COUNTER){
        EnableInterrupts;
        return MB_INVALID_MAILBOX;
    }

    // Check current task has writte permissions
    if(Mailbox_list[mailbox_id].sender_id != caller_task_id){
        EnableInterrupts;
        return MB_INVALID_PERMS;
    }

    // Go to scheduler if we need to wait for data
    while(Mailbox_list[mailbox_id].status == MB_FULL){
        task_scheduler_mailbox();
    }

    // Write data to the mailbox
    Mailbox_list[mailbox_id].data = *data_ptr;
    Mailbox_list[mailbox_id].status = MB_FULL;

    // Obtain the recipient task id
    // If task is WAITING for reading data, set to READY
    receiver_task_id = Mailbox_list[mailbox_id].receiver_id;
    if(Task_list[receiver_task_id].status == TASK_WAIT)
        Task_list[receiver_task_id].status = TASK_READY;

    EnableInterrupts;
    return SUCCESS;
}


uint8_t read_mailbox(uint8_t mailbox_id, uint8_t *data_ptr){
    uint8_t caller_task_id;
    uint8_t sender_task_id;

    // Save current active task so we can use that later
    // ie. when scheduler call us back
    DisableInterrupts;
    caller_task_id = ACTIVE_TASK_ID;

    // Check we received an existing/valid mailbox
    if(mailbox_id >= MAILBOX_COUNTER){
        EnableInterrupts;
        return MB_INVALID_MAILBOX;
    }

    // Check current task has writte permissions
    if(Mailbox_list[mailbox_id].receiver_id != caller_task_id){
        EnableInterrupts;
        return MB_INVALID_PERMS;
    }

    // Make sure the mailbox has new data
    while(Mailbox_list[mailbox_id].status == MB_EMPTY){
        task_scheduler_mailbox();
    }

    // Obtain data from mailbox
    *data_ptr = &Mailbox_list[mailbox_id].data;
    Mailbox_list[mailbox_id].status = MB_EMPTY;

    // Obtain sender task id,
    // If task is WAITING for writing data, set to READY
    sender_task_id = Mailbox_list[mailbox_id].sender_id;
    if(Task_list[sender_task_id].status == TASK_WAIT)
        Task_list[sender_task_id].status = TASK_READY;

    EnableInterrupts;
    return SUCCESS;
}


uint8_t get_task_id(_fptr ptask){
    uint8_t index;

    // Looks for the given task in the task list, if found returns its index
    // Otherwise returns 255
    for(index=0;index < TASK_COUNTER; index++){
        if(Task_list[index].pc_start == ptask)
            return index;
    }

    return TASK_NOT_FOUND;
}

//////////////////////////////////////////////////////////
//                    ACTIVATE TASKS                    //
//////////////////////////////////////////////////////////

void activate_task(_fptr ptask){
    uint8_t index;
    uint16_t **start_sp_value;

    DisableInterrupts; 

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
    Task_list[ACTIVE_TASK_ID].sp_continue = (uint16_t)start_sp_value;

    // Sede el control de la tarea activa
    Task_list[ACTIVE_TASK_ID].status = TASK_READY;

    for(index = 0; index < TASK_COUNTER; index++){

        // Buscamos la tarea en la lista de tareas... U don't say!
        if(Task_list[index].pc_start  == ptask){ 
            Task_list[index].status = TASK_READY;
            break;
        }
    }
    
    EnableInterrupts;
    ACTIVE_TASK_ID = TASK_LIMIT;  
    task_scheduler();
}

void task_scheduler_mailbox(void){
    uint16_t **return_sp_value;
    DisableInterrupts;


    //Obtenemos la direccion del stack donde se encuentra
    //guardado el PC de retorno, sin contar la pagina
    _asm{
      TSX                     ; Guarda el SP en X
      LEAX 7,X                ; Compensa el espacio de las variables
                              ; en el stack y apunta a la direccion
                              ; del MS byte del PC. (N bytes + 1)
      STX return_sp_value      ; Guardamos el SP en la variable
    }

    // Hackeamos el valor de PC de retorno para que apunte al
    // inicio de la llamada de mailbox (read or write)
    Task_list[ACTIVE_TASK_ID].pc_continue = *return_sp_value;


    // Guarda el valor del SP antes de haber entrado a activate_task()
    _asm{
      LDX  return_sp_value     ; Guarda en X el valor del SP inicial
      LEAX 2,X                 ; Compensa el decremento en el SP
                               ; de la instrucción CALL
      STX  return_sp_value     ; Guarda el valor en el apuntador
    }
    Task_list[ACTIVE_TASK_ID].sp_continue = return_sp_value;

    // Sede el control de la tarea activa
    Task_list[ACTIVE_TASK_ID].status = TASK_WAIT;
    task_scheduler();
}

//////////////////////////////////////////////////////////
//                    TASK SCHEDULER                    //
//////////////////////////////////////////////////////////

void task_scheduler(void){ 
    uint8_t index;
    uint8_t task_HP_index;  
    uint8_t task_HP_priority;

    while (1){
        DisableInterrupts;
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
        if(task_HP_index >= TASK_COUNTER){ 
            ACTIVE_TASK_ID=TASK_LIMIT;
            EnableInterrupts;
            return;
        }

        // Pos corrale mijo que va tarde  
        run_task(task_HP_index); 
    }
}

  
//////////////////////////////////////////////////////////
//                    RUN TASK                          //
//////////////////////////////////////////////////////////  
                  
void run_task(uint8_t task_id){
    Task_list[task_id].status = TASK_RUNNING;
    ACTIVE_TASK_ID = task_id;

    if(Task_list[task_id].pc_continue){
        
        RegisterHolder = (uint16_t *)Task_list[task_id].sp_continue;
        
        // Despues de esto perderemos nuestras variables locales
        _asm{
            LDS RegisterHolder     ; Regresa el SP al lugar en que se
                                   ; econtraba dentro del task
        }
        
        // Carga el PC en el registro que utiliza jumper para saltar
        JMP_REG = Task_list[ACTIVE_TASK_ID].pc_continue;
        Task_list[ACTIVE_TASK_ID].pc_continue = 0;
        jumper();
    } 
    else{
    
        // Manda a llamar el inicio de la funcion
        _asm{
            TSX                 ; Obtener el SP en X
            LEAX -3,X           ; Ajusta el valor al SP inicial de la funcion
            STX RegisterHolder  ; Transfierelo a la variable temporal
        }
        
        // Guarda el SP inicial de la funcion a llamar
        Task_list[task_id].sp_start = (uint16_t)RegisterHolder;
        
        EnableInterrupts;
        (*Task_list[task_id].pc_start)();
        DisableInterrupts;
    }

    DisableInterrupts;
    ACTIVE_TASK_ID = TASK_LIMIT;
}


//////////////////////////////////////////////////////////
//                    DESTROY TASKS                     //
//////////////////////////////////////////////////////////

void chain_task(_fptr ptask){
    uint8_t index;

    DisableInterrupts;

    // Terminate Task
    Task_list[ACTIVE_TASK_ID].status = TASK_IDLE;
    Task_list[ACTIVE_TASK_ID].pc_continue = 0;
    Task_list[ACTIVE_TASK_ID].sp_continue = 0;

    RegisterHolder = (uint16_t *)Task_list[ACTIVE_TASK_ID].sp_start; 
    ACTIVE_TASK_ID=TASK_LIMIT;

    //Active The task to chain
    for(index = 0; index < TASK_COUNTER; index++){

        // Buscamos la tarea en la lista de tareas... U don't say!
        if(Task_list[index].pc_start  == ptask){ 
            Task_list[index].status = TASK_READY;
            break;
        }
    } 

    _asm{
        LDS RegisterHolder          ; Mueve el SP al inicio de la funcion
        RTC                         ; Vamos al siguente valor del stack
    }  
}

void terminate_task(void){
    DisableInterrupts;
    Task_list[ACTIVE_TASK_ID].status = TASK_IDLE;
    Task_list[ACTIVE_TASK_ID].pc_continue = 0;
    Task_list[ACTIVE_TASK_ID].sp_continue = 0;
    
    RegisterHolder = (uint16_t *)Task_list[ACTIVE_TASK_ID].sp_start; 
    ACTIVE_TASK_ID=TASK_LIMIT;

    _asm{
        LDS RegisterHolder          ; Mueve el SP al inicio de la funcion
        RTC                         ; Vamos al siguente valor del stack
    }
}

//////////////////////////////////////////////////////////
//                    UTILS                             //
//////////////////////////////////////////////////////////

void jumper(void){
    _asm{
        TSX
        LEAX 1,X
        STX RegisterHolder
    }

    *RegisterHolder = (uint16_t)JMP_REG;
    EnableInterrupts;
}


//////////////////////////////////////////////////////////
//                    ISR FUNCTIONS                     //
//////////////////////////////////////////////////////////

void activate_task_isr(_fptr ptask){
    uint8_t index;
    DisableInterrupts;

    if(ACTIVE_TASK_ID < TASK_LIMIT){
    
        if( Task_list[ACTIVE_TASK_ID].pc_continue == 0){
            //Obtenemos la direccion del stack donde se encuentra
            //guardado el PC de retorno, sin contar la pagina
            _asm{
                TSX                     ; Guarda el SP en X
                LEAX 17,X               ; Compensa el espacio de las variables
                                        ; en el stack y apunta a la direccion 
                                        ; del MS byte del PC. (N bytes + 1)
                STX RegisterHolder      ; Guardamos el SP en la variable
                STX sp_value            ; Guardamos el SP en la variable

            }    

            //Obtenemos la direccion del PC y apuntamos a ella
            Task_list[ACTIVE_TASK_ID].pc_continue = *sp_value;
            *RegisterHolder = (uint16_t)((uint32_t)task_scheduler >> 8) ;


            // Guarda el valor del SP antes de haber entrado a activate_task()
            _asm{
                LDX RegisterHolder      ; Guarda en X el valor del SP inicial
                LEAX 2,X              ; Compensa el decremento en el SP 
                                        ; de la instrucción CALL
                STX sp_value            ; Guarda el valor en el apuntador
            }                   

            // Guarda el valor del SP en la estructura
            Task_list[ACTIVE_TASK_ID].sp_continue = (uint16_t)sp_value;

            // Sede el control de la tarea activa
            Task_list[ACTIVE_TASK_ID].status = TASK_READY;
        }
    }

    for(index = 0; index < TASK_COUNTER; index++){

        // Buscamos la tarea en la lista de tareas... U don't say!
        if(Task_list[index].pc_start  == ptask){
            Task_list[index].status = TASK_READY;
            break;
        }
    }
}


/********* ISR SCHEDULER  **********/

void task_scheduler_isr(void){ 
    uint8_t index;
    uint8_t task_HP_index;  
    uint8_t task_HP_priority;

    while(1){

        DisableInterrupts;
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
        if(task_HP_index >= TASK_COUNTER){

            ACTIVE_TASK_ID=TASK_LIMIT;
            _asm LEAS 3, SP;
            JMP_REG = (uint16_t *)PC_STACK[PC_STACK_SIZE-1];
            PC_STACK_SIZE -= 1;
            jumper();

        }

        run_task(task_HP_index);
    }
}
