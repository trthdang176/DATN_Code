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
#ifndef __OS_PORT_H__
#define __OS_PORT_H__

#include <stdint.h>

extern uint32_t nvic_prio_shift;

/* OS PORT critical section */
#define OS_PORT_CRIT_ENTRY() __asm volatile ("cpsid i") /* disable interrupt */
#define OS_PORT_CRIT_EXIT()  __asm volatile ("cpsie i") /* enable interrupt */
// #define OS_PORT_TASK_PEND()  *(pOS_task->nvic_pend) = pOS_task->nvic_irq; /* set pending interrupt */
#define OS_PORT_TASK_PEND()  *(pQueue->nvic_pend) = pQueue->nvic_irq; /* set pending interrupt */



#endif /* __OS_PORT_H__ */
