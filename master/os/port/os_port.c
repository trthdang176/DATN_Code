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
uint32_t nvic_prio_shift;

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

/* ==================================================================================== */



 
