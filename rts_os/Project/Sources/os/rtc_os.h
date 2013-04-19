#ifndef __RTS_OS__H
#define __RTS_OS__H 1

#include <hidef.h>      /* common defines and macros */

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
  uint8_t   period;
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

/* GLOBALS */
extern uint8_t ACTIVE_TASK_ID;
extern volatile uint8_t ISR_FLAG;
extern uint16_t *RegisterHolder;
extern uint16_t **sp_value;

void add_task(_fptr funct, uint8_t args);
void add_alarm(_fptr funct, uint8_t delay, uint8_t period);
void task_scheduler(void);
void init_tasks(void);
void init_alarms(void);
void activate_task(_fptr ptask);
void activate_task_isr(_fptr ptask);
void terminate_task(void);
void run_task(uint8_t task_id);

#endif