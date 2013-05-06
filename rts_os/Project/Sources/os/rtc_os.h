#ifndef __RTS_OS__H
#define __RTS_OS__H 1

/* common defines and macros */
#include <hidef.h>
#include <MC9S12XEP100.h>


#define TASK_LIMIT  (10)
#define TICK_TIME   (2000)
#define AUTOSTART   (0x80)
#define TASK_NOT_FOUND (0xFE)
#define ERROR (0xFF)

typedef unsigned char uint8_t;
typedef unsigned int  uint16_t;
typedef unsigned long uint32_t;
typedef void (*_fptr)(void);

typedef struct{
    Bool      autostart;
    uint8_t   priority;
    uint8_t   status;
    _fptr     pc_start;
    uint16_t  *pc_continue;
    uint16_t  sp_start;
    uint16_t  sp_continue;
}Task;


typedef struct{
  uint8_t   delay;
  uint8_t   period;
  uint8_t   task_id;
}Alarm;

typedef struct{
    uint8_t status;
    uint8_t sender_id;
    uint8_t receiver_id;
    uint8_t data;
}Mailbox;

/* TASK STATES */
enum{
  TASK_IDLE,
  TASK_READY,
  TASK_RUNNING,
  TASK_COMPLETE,
  TASK_WAIT
};

enum{
    SUCCESS,
    FAILURE
};

enum{
    MB_EMPTY=128,
    MB_FULL,
    MB_READ,
    MB_WRITE,
    MB_NO_MAILBOX_LEFT,
    MB_INVALID_MAILBOX,
    MB_INVALID_PERMS,
    MB_INVALID_ACTION,
    MB_UNABLE_TO_READ,
    MB_UNABLE_TO_WRITE
};

/* GLOBALS */
extern uint8_t  ACTIVE_TASK_ID;
extern uint16_t *RegisterHolder;
extern uint16_t *JMP_REG;
extern uint16_t **sp_value;
extern uint16_t PC_STACK [1];

extern volatile uint8_t PC_STACK_SIZE;
extern volatile uint8_t  ALARM_COUNTER;
extern volatile uint8_t  TASK_COUNTER;
extern volatile uint8_t  MAILBOX_COUNTER;
extern volatile Bool     TASK_ACTIVATED;
extern volatile Task Task_list[TASK_LIMIT];
extern volatile Alarm Alarm_list[TASK_LIMIT];
extern volatile Mailbox Mailbox_list[TASK_LIMIT];


/* PROTOTYPES */
void OSInit(void);
void jumper(void);
void terminate_task(void);
void add_task(_fptr funct, uint8_t args);
void add_alarm(_fptr funct, uint8_t delay, uint8_t period);
void task_scheduler(void);
void task_scheduler_isr(void);
uint8_t add_mailbox(_fptr sender, _fptr receiver);
uint8_t get_task_id(_fptr ptask);

void init_tasks(void);
void init_alarms(void);
void init_mailboxes(void);

void activate_task(_fptr ptask);
void activate_task_isr(_fptr ptask);
void run_task(uint8_t task_id);

uint8_t is_mailbox_full(uint8_t mailbox_id);
uint8_t is_mailbox_empty(uint8_t mailbox_id);
uint8_t write_mailbox(uint8_t mailbox_id, uint8_t *data);
uint8_t read_mailbox(uint8_t mailbox_id, uint8_t *data);

#endif
