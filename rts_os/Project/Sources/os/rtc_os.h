#ifndef __RTS_OS__H
#define __RTS_OS__H 1

#include <hidef.h>      /* common defines and macros */

typedef unsigned char uint8_t;
typedef unsigned int  uint16_t;

#define AUTOSTART TRUE


typedef void (*_fptr)(void);

typedef struct{
  Bool      autostart;
  uint8_t   priority;
  uint8_t   status;
  _fptr  pc_start;
  _fptr  pc_actual;
  _fptr  sp_start;
  _fptr  sp_actual;
}Task;


enum{
  TASK_IDLE,
  TASK_READY,
  TASK_RUNNING,
  TASK_COMPLETE,
  TASK_SUSPENDED  
};

#define TASK_LIMIT    (10)

void add_task(_fptr funct, Bool autostart, uint8_t priority);
void task_scheduler(void);
void activate_task(_fptr ptask);

#endif