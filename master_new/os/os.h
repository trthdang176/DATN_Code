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
//#include "../Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal.h"
#include "../Drivers/STM32F3xx_HAL_Driver/Inc/stm32f3xx_hal.h"

/* ---------------------------------- OS MACRO ---------------------------------- */
/* Macro for downcasting OS Event to Event of substruct app */
#define OS_EVT_DOWNCAST(Evt_sub, e) ((Evt_sub const *)e)
#define OS_PRI_MAX				(3)  /* Max priority */
#define OS_TASK_PER_PRI         (10) /* number of tasks in per priority */


/* ---------------------------------- OS EVENT ---------------------------------- */
typedef struct {
    uint16_t sig; /* signal of event */

    /* Payload */
    uint8_t data[64];
    uint32_t len;
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

    uint8_t task_piro;       /* priority of task */
    /* task attributes for ARM Cortex-M */
    uint32_t volatile *nvic_pend;
    uint32_t nvic_irq;
};

/* Task control block */
typedef struct {
    OS_task * pOS_task;
    OS_event_t *pEvent;
} OS_tcb;

typedef struct {
    uint8_t head_index;
    uint8_t tail_index;
    uint8_t end;
    uint8_t nUsed; 

    OS_tcb buf_tcb[OS_TASK_PER_PRI]; /* ring buffer of the Task control block queue */

    /* task attributes for ARM Cortex-M */
    uint32_t nvic_irq;
} OS_ring_buf_tcb;

/* ---------------------------------- OS TIME ---------------------------------- */
typedef struct OS_TimeEvt_t OS_TimeEvt;
struct OS_TimeEvt_t {
    OS_event_t Time_event;    /* Inherit super struct */

    OS_TimeEvt *next;       /* link to next time event */
    OS_task *task_post;      /* The task post time event */
    uint16_t Time_Ctr;      /* down-counter value */
    uint16_t Time_interval; /* interval for periodic time event */
};

/* ---------------------------------- OS FUNCTION PROTOTYPE ---------------------------------- */
void OS_task_run(void);
void OS_task_init(OS_task * const pOS_task, OS_handler init, OS_handler dispatch);
void OS_task_create(OS_task * const pOS_task, OS_taskPrio task_prio, OS_event_t const **pBuf, OS_EQCt eqLen, OS_event_t const * const init_e);
void OS_task_post_event(OS_task *const pOS_task,uint8_t sig, uint8_t *data, uint32_t size);

/* Task operations for ARM Cortex-M */
// void OS_setTask_priority(OS_task * const pOS_task, OS_taskPrio prio);
void OS_priority_IRQ(uint8_t IRQ, OS_taskPrio prio);
void OS_Test_setIRQ(OS_task * const pAO, uint8_t num_irq);
// void OS_task_activate(OS_task * const pOS_task);
void OS_task_activate(uint8_t prio_task);
// void OS_task_setIRQ(OS_task * const pOS_task, uint8_t num_irq);
void OS_prio_setIRQ(uint8_t prio, uint8_t num_irq);

void OS_TimeEvt_init(OS_TimeEvt *pOS_Evt, uint16_t  sig, OS_task *task);
void OS_TimeEvt_Set(OS_TimeEvt *pOS_Evt, uint16_t count, uint16_t interval);
void OS_TimeEvt_Stop(OS_TimeEvt *pOS_Evt);
void OS_TimeEvt_tick(void);



#endif /* __OS_H__ */
