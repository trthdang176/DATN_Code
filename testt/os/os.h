/**
 * @file os.c
 * @author DANGTT
 * @brief RTOS OS base on Super-Simple Tasker (SST/C)
 * @version 0.1
 * @date 2025-05-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef __OS_H__
#define __OS_H__

#include <stdint.h>
#include "port/os_port.h"

/* ---------------------------------- OS EVENT ---------------------------------- */
typedef struct {
    uint16_t sig; /* signal of event */

    /* Payload */
} OS_event_t;

/* ---------------------------------- OS TASK ---------------------------------- */

typedef struct OS_task_t OS_task;
/* OS task priority */
typedef uint8_t OS_taskPrio;

/* event-queue counter */
typedef uint8_t OS_EQCt;

/* Generic Handler */
typedef void (*OS_handler)(OS_task * const pOS_task, OS_event_t const * const pEvent) ;

struct OS_task_t {
    OS_handler init;
    OS_handler dispatch;

    OS_event_t const **qBuf; /* ring buffer of the event queue */
    OS_EQCt end;             /* Last index into the ring buffer */
    OS_EQCt head;            /* index for inserting event */
    OS_EQCt tail;            /* index for removing event */
    OS_EQCt nUsed;           /* used entries currently in the queue */

    /* task attributes for ARM Cortex-M */
    uint32_t volatile *nvic_pend;
    uint32_t nvic_irq;
};



/* ---------------------------------- OS FUNCTION PROTOTYPE ---------------------------------- */
void OS_task_run(void);
void OS_task_init(OS_task * const pOS_task, OS_handler init, OS_handler dispatch);
void OS_task_create(OS_task * const pOS_task, OS_taskPrio task_prio, OS_event_t const **pBuf, OS_EQCt eqLen, OS_event_t const * const init_e);
void OS_task_post(OS_task *const pOS_task, OS_event_t const * const pEvent);

/* Task operations for ARM Cortex-M */
void OS_setTask_priority(OS_task * const pOS_task, OS_taskPrio prio);
void OS_task_activate(OS_task * const pOS_task);
void OS_task_setIRQ(OS_task * const pOS_task, uint8_t num_irq);

#endif /* __OS_H__ */
