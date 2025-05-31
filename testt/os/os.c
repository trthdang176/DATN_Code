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

#include "os.h"

void OS_task_run(void) {
    OS_start();         /* start of multipletasking in port-specific */
    OS_onStart();       /* application config & start interrupt */

    for (;;) {
        /* idle loop of the OS kernel */

    }
}

/* Initialize object by assigning init and dispatch handler function */
void OS_task_init(
    OS_task * const pOS_task,
    OS_handler init,
    OS_handler dispatch) 
{
    pOS_task->init = init;
    pOS_task->dispatch = dispatch;
}

/**
 * @brief Create the OS task
 *  
 * @param pOS_task Task( a.k.a active object ) to start
 * @param task_prio priority of task
 * @param pBuf the event queue of task
 * @param eqLen event queue length
 * @param init_e initialization event 
 */
void OS_task_create(
    OS_task * const pOS_task,
    OS_taskPrio task_prio,
    OS_event_t const **pBuf ,
    OS_EQCt eqLen,
    OS_event_t const * const init_e)
{
    /* check parameter */
    if ( task_prio > 0  /* the priority must be greater than 0 */
        && pBuf != (OS_event_t const **)0 /* The queue exists */
        && eqLen > 0) { /* the length greater than 0 */
        pOS_task->qBuf = pBuf;
        pOS_task->end = eqLen - 1;
        pOS_task->head = 0;
        pOS_task->tail = 0;
        pOS_task->nUsed = 0;

        /* Set task priority */
        OS_setTask_priority(pOS_task,task_prio);

        /* Initialize task with initialization event */
        (*pOS_task->init)(pOS_task,init_e);
    } 
}

/**
 * @brief Post the event to event queue of task 
 * 
 * @param pOS_task 
 * @param pEvent 
 */
void OS_task_post(OS_task *const pOS_task, OS_event_t const * const pEvent) {
    /* Check over flow of queue */
    if (pOS_task->nUsed <= pOS_task->end) {
        OS_PORT_CRIT_ENTRY();
        pOS_task->qBuf[pOS_task->head] = pEvent; /* insert event to queue */
        if (pOS_task->head = 0) { 
            pOS_task->head = pOS_task->end; /* wrap around to end of the buffer */
        } else {
            --pOS_task->head;
        }
        ++pOS_task->nUsed; /* Increase number event used in queue */

        /* Post event -> make the interrupt */
        OS_PORT_TASK_PEND();
        OS_PORT_CRIT_EXIT();
    }
}

