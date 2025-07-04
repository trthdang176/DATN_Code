#include "app_screen.h"

#include "app_main.h"
#include "../Core/Inc/main.h"

typedef struct {
    OS_task task;

} app_screen;

void screen_ctor(app_screen *pAO);
static void screen_init(app_screen * const pOS_task, OS_event_t const * const pEvent);
static void screen_dispatch(app_screen * const pOS_task, OS_event_t const * const pEvent);

static app_screen screen_instance;
OS_task * const AO_task_screen = &screen_instance.task;

void screen_task_init(void) {
    screen_ctor(&screen_instance);
}

void screen_ctor(app_screen *pAO) {
    OS_task_init(AO_task_screen,(OS_handler)screen_init,(OS_handler)screen_dispatch);
}

static void screen_init(app_screen * const pOS_task, OS_event_t const * const pEvent) {
    UNUSED(pOS_task);
    UNUSED(pEvent);
} 

static void screen_dispatch(app_screen * const pOS_task, OS_event_t const * const pEvent) {
    switch (pEvent->sig) {
        case DEIVCE_TEST_FINISH : {
            printf("Device test success!!\n");
            
        } break;

        default: break;
    }
}
