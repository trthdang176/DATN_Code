#ifndef __TEST_OS_H__
#define __TEST_OS_H__

#include <stdint.h>
#include "app_post_test.h"

/* the app's event */
typedef struct {
    OS_event_t appEvt; /* Inherit OS event */
    
    /* the app's event value, element of event */
    uint16_t ticks1;   /* number of clock ticks between */
    uint16_t ticks2;   /* number of clock ticks between */
} TestOS_Evt;

void BlinkyTest_app_init(void);

extern OS_task * const AO_BlinkyTest; /* App task pointer */

#endif /* __TEST_OS_H__ */
