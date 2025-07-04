#include "os_event.h"
#include "port/os_port.h"

#include "os.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#define OS_EVT_NULL         ((OS_event_t*)0)
#define OS_EVT_POOL_SIZE    (OS_TASK_PER_PRI)

/* dynamic event pool */
static OS_event_t dynamic_event_pool[OS_EVT_POOL_SIZE];
static uint8_t dynamic_event_pool_free[OS_EVT_POOL_SIZE]; /* Store index in free */
static uint8_t dynamic_event_free_stack; /* point to the next index freen in pool */


void dynamic_event_pool_init(void) {
    OS_PORT_CRIT_ENTRY();

    dynamic_event_free_stack = 0;
    for (uint8_t i = 0; i < OS_EVT_POOL_SIZE; i++) {
        dynamic_event_pool_free[i] = i; /* Store the index free */
    }
    dynamic_event_free_stack = OS_EVT_POOL_SIZE;
    
    OS_PORT_CRIT_EXIT();
}


void free_dynamic_event(OS_event_t *e) {
    OS_PORT_CRIT_ENTRY();

    uint8_t idx = e - dynamic_event_pool; /* Get index of event */
//    free(e->data); /* free allocated memory */
    if (idx > 0 && idx < OS_EVT_POOL_SIZE) {
        dynamic_event_pool_free[dynamic_event_free_stack++] = idx; 
    }

    OS_PORT_CRIT_EXIT();
}

OS_event_t *get_dynamic_event(void) {
    OS_PORT_CRIT_ENTRY();
    
    if (dynamic_event_free_stack == 0) return NULL; /* full */
    uint8_t idx = dynamic_event_pool_free[--dynamic_event_free_stack]; /* Get index in free */
    
    OS_PORT_CRIT_EXIT();
    return &dynamic_event_pool[idx];
}

void set_data_dynamic_event(OS_event_t *e, uint8_t *data, uint32_t len) {
    if (len != 0) {
        e->len = len;
        memcpy(e->data,data,len);
    } else {
        e->len = len;
        memset(e->data, 0, sizeof(e->data));
    }
}

void set_sig_dynamic_event(OS_event_t *e, uint8_t sig) {
    e->sig = sig;
}

uint8_t *get_data_dynamic_event(OS_event_t * const e) {
    return (e->data);
}

uint32_t get_len_dynamic_event(OS_event_t *e) {
    return e->len;
}
