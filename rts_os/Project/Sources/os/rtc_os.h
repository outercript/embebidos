#ifndef __RTS_OS__H
#define __RTS_OS__H 1

#include <hidef.h>      /* common defines and macros */
#include <MC9S12XEP100.h>

typedef unsigned char uint8_t;
typedef unsigned int  uint16_t;
typedef unsigned long  uint32_t;

#define AUTOSTART (0x80)


typedef void (*_fptr)(void);

typedef struct{
  Bool      autostart;
  uint8_t   priority;
  uint8_t   status;
  _fptr     pc_start;
  uint16_t  *pc_continue;
  uint16_t  *sp_start;
  uint16_t  *sp_continue;
}Task;

typedef struct{
  uint8_t   delay;
  uint16_t   period;
  uint8_t   task_id;
}Alarm;


enum{
  TASK_IDLE,
  TASK_READY,
  TASK_RUNNING,
  TASK_COMPLETE,
  TASK_SUSPENDED  
};

#define TASK_LIMIT    (10)
#define TICK_TIME     (2000)

/* GLOBALS */
extern uint8_t ACTIVE_TASK_ID;
extern uint16_t *RegisterHolder;
extern uint16_t *JMP_REG;
extern uint16_t **sp_value;
extern uint16_t *PC_STACK [TASK_LIMIT*2];

extern volatile uint8_t  PC_STACK_SIZE;
extern volatile uint8_t  ALARM_COUNTER;
extern volatile uint8_t  TASK_COUNTER;
extern volatile Bool     TASK_ACTIVATED;
extern volatile Task Task_list[TASK_LIMIT];
extern volatile Alarm Alarm_list[TASK_LIMIT];

//extern volatile Bool INTERRUPT_CASE;

void add_task(_fptr funct, uint8_t args);
void add_alarm(_fptr funct, uint8_t delay, uint16_t period);
void task_scheduler(void);
void task_scheduler_isr(void);
void init_tasks(void);
void init_alarms(void);
void jumper(void);
void jumper_isr(void);
void activate_task(_fptr ptask);
void activate_task_isr(_fptr ptask);
void terminate_task(void);
void run_task(uint8_t task_id);

#endif