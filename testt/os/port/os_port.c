/**
 * @file os_port.c
 * @author DANGTT
 * @brief RTOS OS port to ARM Cortex M4 ( STM32F4 )
 * @version 0.1
 * @date 2025-05-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "os_port.h"
 #include "../os.h"
//#include "os/os.h"

/* ---------------------------- DEFINE FOR PORT ARM CORTEX ---------------------------- */
#define NVIC_PEND    ((uint32_t volatile *)0xE000E200U) /* Interrupt Set-pending Registers */
#define NVIC_EN      ((uint32_t volatile *)0xE000E100U) /* Interrupt Set-enable Registers */
#define NVIC_IP      ((uint32_t volatile *)0xE000E400U) /* Interrupt Priority Registers */
#define SCB_SYSPRI   ((uint32_t volatile *)0xE000ED14U)
#define SCB_AIRCR   *((uint32_t volatile *)0xE000ED0CU)
#define FPU_FPCCR   *((uint32_t volatile *)0xE000EF34U)

/* ==================================================================================== */

/* ------------------------------------ OS FUNCTION ----------------------------------- */

/* the bit shift to set the value priority */
static uint32_t nvic_prio_shift;

/**
 * @brief Initialization the os kernel , get the priority bit mask of NVIC
 * 
 */
void OS_init(void) {
    /* determine number of NVIC priority bits by writing 0xFF to the
    * NIVIC IP register for PendSV and then reading back the result,
    * which has only the implemented bits set.
    */
    uint32_t tmp = SCB_SYSPRI[3];       /* Plus 3 register -> 0xE000ED20 :  System Handler Priority Register 3 */
    SCB_SYSPRI[3] |= (0xFFU << 16U);    /* write 0xFF to PendSV prio */
    uint32_t prio = ((SCB_SYSPRI[3] >> 16U) & 0xFFU); /* read back to get the number bit is implemented */
    SCB_SYSPRI[3] = tmp; /* restore the original PendSV prio */

    /* for loop to find how many bit shift to bit implement 
    * example the result is : 0b11110000 mean that need shift 4 bit to set priority will loop until find the value 1 
    */
   for (tmp = 0; tmp < 8; tmp++) {
        if (prio & (1u << tmp) != 0) { /* loop until detect the 1 value */
            break;
        }
   }

   nvic_prio_shift = tmp; /* the tmp now have the value number of bit shift */
}

/**
 * @brief Set task priority 
 * 
 * @param pOS_task Task pointer 
 * @param prio task priority 
 */
void OS_setTask_priority(OS_task * const pOS_task, OS_taskPrio prio) {
    /* Check parameter */
    if (pOS_task->nvic_irq != 0 /* have the number irq */
        && (prio <= (0xFFU >> nvic_prio_shift))) /* the priority number is smaller than max value base on number bit can implement */ 
    {
        /* convert the SST direct priority (1,2,..) to NVIC priority... */
        uint32_t nvic_prio = ((0xFFU >> nvic_prio_shift) + 1U - prio) << nvic_prio_shift;

        OS_PORT_CRIT_ENTRY();
        /* set the task priority */
        uint32_t tmp = NVIC_IP[pOS_task->nvic_irq >> 2U]; /* get the value register correpond with index of irq number */
        tmp &= ~(0xFFU << ((pOS_task->nvic_irq & 3U) << 3U)); /* Clear the value priority of IRQ number */
        tmp |= (nvic_prio << ((pOS_task->nvic_irq & 3U) << 3U)); /* Set the value priority */
        NVIC_IP[pOS_task->nvic_irq >> 2U] = tmp; /* Assign value to register */

        /* enable the IRQ associated with the Task */
        NVIC_EN[pOS_task->nvic_irq >> 5U] = (1U << (pOS_task->nvic_irq & 0x1FU));
        OS_PORT_CRIT_EXIT();

        /* store the address of NVIC_PEND address and the IRQ bit */
        pOS_task->nvic_pend = &NVIC_PEND[pOS_task->nvic_irq >> 5U];
        pOS_task->nvic_irq  = (1U << (pOS_task->nvic_irq & 0x1FU));
    }
}

/**
 * @brief The task performs an action when it receives an event from the queue. Call in interrupt handler to get event and dispatch
 * 
 * @param pOS_task Task pointer
 */
void OS_task_activate(OS_task * const pOS_task) {
    /* Check event in queue */
    if (pOS_task->nUsed == 0) { /* don't have event in queue */
        return;
    }
    OS_event_t const *e_receive = pOS_task->qBuf[pOS_task->tail];
    if (pOS_task->tail == 0) {
        pOS_task->tail = pOS_task->end; /* wrap around */
    } else {
        --pOS_task->tail;
    }
    OS_PORT_CRIT_ENTRY();
    if (--pOS_task->nUsed > 0) { /* check have any event in queue */
        *(pOS_task->nvic_pend) = pOS_task->nvic_irq;
    }
    OS_PORT_CRIT_EXIT();

    /* Dispatch the received event to this task */
    (*pOS_task->dispatch)(pOS_task,e_receive);
}


/**
 * @brief Set the number IRQ for task
 * 
 * @param pOS_task Task pointer
 * @param num_irq IRQ number 
 */
void OS_task_setIRQ(OS_task * const pOS_task, uint8_t num_irq) {
    pOS_task->nvic_irq = num_irq;
}

/* ==================================================================================== */



 
