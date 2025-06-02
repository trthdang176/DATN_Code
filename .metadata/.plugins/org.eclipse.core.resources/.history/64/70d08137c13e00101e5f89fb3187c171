#include "app_can_bus.h"

/* app header */
#include "app_main.h"
#include "../Core/Inc/main.h"

typedef struct {
    OS_task task;

    /* Test timeer */
    OS_TimeEvt te;
} app_can;

void can_bus_ctor(app_can *pAO);
static void can_bus_init(app_can * const pOS_task, OS_event_t const * const pEvent);
static void can_bus_dispatch(app_can * const pOS_task, OS_event_t const * const pEvent);

static app_can can_bus_instance;
OS_task * const AO_task_can_bus = &can_bus_instance.task; 

void app_can_bus_init(CAN_HandleTypeDef *pCan) {
    can_bus_ctor(&can_bus_instance);

    UNUSED(pCan);
}

void can_bus_ctor(app_can *pAO) {
    OS_task_init(AO_task_can_bus,(OS_handler)&can_bus_init,(OS_handler)&can_bus_dispatch);

    OS_TimeEvt_init(&can_bus_instance.te,RECEIVE_DATA_CAN_BUS,&can_bus_instance.task);
}

static void can_bus_init(app_can * const pOS_task, OS_event_t const * const pEvent) {
    OS_TimeEvt_Stop(pOS_task->te);

    UNUSED(pEvent);
}

static void can_bus_dispatch(app_can * const pOS_task, OS_event_t const * const pEvent) {
    switch (pEvent->sig) {
        case SEND_DATA_CAN_BUS : {
            printf("Send data to device test");
            
        } break;
        case RECEIVE_DATA_CAN_BUS : {

        } break;
        default : break;
    }
}


