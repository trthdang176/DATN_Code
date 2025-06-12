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
#include "port/os_port.h"
#include "os_event.h"

/* private function prototypes */
void OS_task_post(OS_task *const pOS_task, OS_event_t const * const pEvent);

static OS_ring_buf_tcb pri_queue[OS_PRI_MAX];

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
        // OS_setTask_priority(pOS_task,task_prio);
        pOS_task->task_piro = task_prio - 1;


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
    OS_ring_buf_tcb *pQueue = &pri_queue[pOS_task->task_piro];

    /* Check over flow of queue */
    if (pri_queue[pOS_task->task_piro].nUsed >= OS_TASK_PER_PRI || pOS_task->nUsed > pOS_task->end ) { /* full queue */
        return;
    }

    /* insert tcb to queue */
    OS_PORT_CRIT_ENTRY();
    pQueue->buf_tcb[pQueue->head_index].pEvent = pEvent;
    pQueue->buf_tcb[pQueue->head_index].pOS_task = pOS_task;

    if (pQueue->head_index == 0) {
        pQueue->head_index = OS_TASK_PER_PRI - 1; /* wrap around to end of the buffer */
    } else {
        --pQueue->head_index;
    }
    ++pQueue->nUsed; /* Increase number event used in queue */

    pOS_task->qBuf[pOS_task->head] = pEvent; /* insert event to queue */
    if (pOS_task->head == 0) { 
        pOS_task->head = pOS_task->end; /* wrap around to end of the buffer */
    } else {
        --pOS_task->head;
    }
    ++pOS_task->nUsed; /* Increase number event used in queue */

    /* Post event -> make the interrupt */
    // OS_PORT_TASK_PEND();
    HAL_NVIC_SetPendingIRQ(pQueue->nvic_irq);
    OS_PORT_CRIT_EXIT();
}

void OS_task_post_event(OS_task *const pOS_task,uint8_t sig, uint8_t *data, uint32_t size) {
    OS_event_t *e = get_dynamic_event();
    set_sig_dynamic_event(e,sig);
    set_data_dynamic_event(e,data,size);
    OS_task_post(pOS_task,e);
}

/* ================================== OS TIMER ================================== */
static OS_TimeEvt *timeEvt_head = (OS_TimeEvt *)0 ; /* NULL for the first head */

void OS_TimeEvt_init(
    OS_TimeEvt *pOS_Evt,
    uint16_t  sig,
    OS_task *task)
{
    pOS_Evt->Time_event.sig = sig;
//    pOS_Evt->Time_event.data = 0;
    pOS_Evt->Time_event.len = 0;
    pOS_Evt->task_post = task;
    pOS_Evt->Time_Ctr = 0;
    pOS_Evt->Time_interval = 0;

    /* add new node to head of liked list */
    pOS_Evt->next = timeEvt_head;
    timeEvt_head = pOS_Evt;
} 

/**
 * @brief Set value for OS timer 
 * 
 * @param pOS_Evt Timer for generating event when TIMEOUT 
 * @param count TIMEOUT value 
 * @param interval interval value for periodic time event
 */
void OS_TimeEvt_Set(OS_TimeEvt *pOS_Evt, uint16_t count, uint16_t interval) {
    OS_PORT_CRIT_ENTRY();
    pOS_Evt->Time_Ctr = count;
    pOS_Evt->Time_interval = interval;
    OS_PORT_CRIT_EXIT();
}

void OS_TimeEvt_Stop(OS_TimeEvt *pOS_Evt) {
    OS_PORT_CRIT_ENTRY();
    pOS_Evt->Time_Ctr = 0;
    pOS_Evt->Time_interval = 0;
    OS_PORT_CRIT_EXIT();
}

/**
 * @brief function in systemtick handler (1ms) to execute task time OS 
 * 
 */
void OS_TimeEvt_tick(void) {
    /* run for all the node time event */
    for (OS_TimeEvt *node = timeEvt_head; node != ((OS_TimeEvt *)0); node = node->next) {
        OS_PORT_CRIT_ENTRY();
        if (node->Time_Ctr == 0) { /* down counter = 0  -> stop node */
            OS_PORT_CRIT_EXIT();
        } else if (node->Time_Ctr == 1) { /* TIMEOUT */
            node->Time_Ctr = node->Time_interval;
            OS_PORT_CRIT_EXIT();
            /* Post event */
            // OS_task_post(node->task_post,&node->Time_event);
            OS_task_post_event(node->task_post,node->Time_event.sig,(uint8_t *)0,0);
        } else { /* count down value */
            --node->Time_Ctr;
            OS_PORT_CRIT_EXIT();
        }
    }
}




/**
 * @brief The task performs an action when it receives an event from the queue. Call in interrupt handler to get event and dispatch
 * 
 * @param pOS_task Task pointer
 */
void OS_task_activate(uint8_t prio_task) {
    OS_ring_buf_tcb *pQueue = &pri_queue[prio_task - 1];
    
    if (pQueue->nUsed == 0) { /* don't have tcb */
        return;
    }
    
    uint8_t index = pQueue->tail_index;
    if (pQueue->tail_index == 0) {
        pQueue->tail_index = OS_TASK_PER_PRI - 1; /* wrap around */
    } else {
        --pQueue->tail_index;
    }

    --pQueue->nUsed;

    OS_task *pOS_task = pQueue->buf_tcb[index].pOS_task;

    /* Check event in queue */
    if (pOS_task->nUsed == 0) { /* don't have event in queue */
        return;
    }

    OS_event_t const *e_receive = pQueue->buf_tcb[index].pEvent;
    if (pOS_task->tail == 0) {
        pOS_task->tail = pOS_task->end; /* wrap around */
    } else {
        --pOS_task->tail;
    }
    OS_PORT_CRIT_ENTRY();
    // --pOS_task->nUsed;
    if (--pOS_task->nUsed > 0) { /* check have any event in queue */
        HAL_NVIC_SetPendingIRQ(pQueue->nvic_irq);
    }
    OS_PORT_CRIT_EXIT();

    /* Dispatch the received event to this task */
    (*pOS_task->dispatch)(pOS_task,e_receive);

    free_dynamic_event(e_receive);
}

/**
 * @brief Set task priority 
 * 
 * @param pOS_task Task pointer 
 * @param prio task priority 
 */
void OS_priority_IRQ(uint8_t IRQ, OS_taskPrio prio) {
    HAL_NVIC_SetPriority(IRQ,(15U - prio),0);
    HAL_NVIC_EnableIRQ(IRQ);
}


/**
 * @brief Set the number IRQ for specific priority
 * 
 * @param prio priority begin from 1 
 * @param num_irq IRQ number 
 */
void OS_prio_setIRQ(uint8_t prio, uint8_t num_irq) {
    if (prio < OS_PRI_MAX) {
        pri_queue[prio - 1].nvic_irq = num_irq;
    }
}

void OS_Test_setIRQ(OS_task * const pAO, uint8_t num_irq) {
    pAO->nvic_irq = num_irq;
}
