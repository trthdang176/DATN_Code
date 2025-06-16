#include "app_can_bus.h"

/* app header */
#include "app_main.h"
#include "../Core/Inc/main.h"

#define MAX_MANAGER_TX_BUF_SIZE 5

typedef struct {
    OS_task task;

    /* Test timer */
    OS_TimeEvt te_timeout;

    managerTX_CAN_t **managerTX_buf; /* buff store data check */
    uint8_t head_index;
    uint8_t tail_index;
    uint8_t nUsed;
    uint8_t end;
    bool b_counting ; /* run or not run */
} app_can;

void can_bus_ctor(app_can *pAO);
static void can_bus_init(app_can * const pOS_task, OS_event_t const * const pEvent);
static void can_bus_dispatch(app_can * const pOS_task, OS_event_t const * const pEvent);

static app_can can_bus_instance;
OS_task * const AO_task_can_bus = &can_bus_instance.task; 
static managerTX_CAN_t *buff_store[MAX_MANAGER_TX_BUF_SIZE]; /* buffer store the data write */


void can_bus_task_init(CAN_HandleTypeDef *pCan) {
    printf("init app can bus\n");
    can_bus_instance.b_counting = false;
    can_bus_instance.head_index = 0;
    can_bus_instance.tail_index = 0;
    can_bus_instance.nUsed = 0;
    can_bus_instance.end = MAX_MANAGER_TX_BUF_SIZE - 1;
    can_bus_instance.managerTX_buf = buff_store;

    can_bus_ctor(&can_bus_instance);

    UNUSED(pCan);
}

void can_bus_ctor(app_can *pAO) {
    OS_task_init(AO_task_can_bus,(OS_handler)&can_bus_init,(OS_handler)&can_bus_dispatch);

    OS_TimeEvt_init(&can_bus_instance.te_timeout,TX_TIMEOUT,&can_bus_instance.task);
}

static void can_bus_init(app_can * const pOS_task, OS_event_t const * const pEvent) {
    OS_TimeEvt_Stop(&pOS_task->te_timeout);

    UNUSED(pEvent);
}

static void can_bus_dispatch(app_can * const pOS_task, OS_event_t const * const pEvent) {
    switch (pEvent->sig) {
        case SEND_DATA_CAN_BUS : {
            printf("Send data to device test");
            
        } break;
        case RECEIVE_DATA_CAN_BUS : {

        } break;
        case START_TIMEOUT_TX : {
            printf("start time out\n");
            managerTX_CAN_t *check_TX_CAN = (managerTX_CAN_t *)(*(uint32_t *)get_data_dynamic_event(pEvent));
            pOS_task->managerTX_buf[pOS_task->head_index] = check_TX_CAN;
            if (pOS_task->head_index == 0) {
                pOS_task->head_index = pOS_task->end;
            } else {
                --pOS_task->head_index;
            }
            ++pOS_task->nUsed;
            if (pOS_task->b_counting == false) { /* in free, count to check timeout */
                pOS_task->b_counting = true;
                OS_TimeEvt_Set(&pOS_task->te_timeout,500,0); /* run only 1 time */                
            }
        } break;
        case TX_TIMEOUT : {
            managerTX_CAN_t *check_TX_CAN = pOS_task->managerTX_buf[pOS_task->tail_index];
            if (check_TX_CAN->TX_Done) { // transmit success 
                printf("transmit CAN id %x success\n",check_TX_CAN->ID);
                if (pOS_task->managerTX_buf[pOS_task->tail_index] != NULL) {
                    free(pOS_task->managerTX_buf[pOS_task->tail_index]);
                    pOS_task->managerTX_buf[pOS_task->tail_index] = NULL;
                }
            } else { // transmit error 
                /* go to warning screen */
                printf("transmit CAN id %x failed\n",check_TX_CAN->ID);
                warning_page(&_Screen,check_TX_CAN->ID);
                OS_task_post_event(AO_BlinkyTest,BEGIN_BUZZER,0,0);
            }

            if (pOS_task->tail_index == 0) {
                    pOS_task->tail_index = pOS_task->end;
                } else {
                    --pOS_task->tail_index;
                } 
            --pOS_task->nUsed;
            pOS_task->b_counting = false;
            /* check buff */
            if (pOS_task->nUsed > 0 ) {
                pOS_task->b_counting = true;
                OS_TimeEvt_Set(&pOS_task->te_timeout,500,0); /* run only 1 time */    
            }
        } break;
        case TX_SUCCESS : {
            // if (pOS_task->b_counting = true) {
                printf("tx receive slave\n");
                pOS_task->managerTX_buf[pOS_task->tail_index]->TX_Done = true;
            // }
        } break; 
        case TX_FAILED : {

        } break;
        default : break;
    }
}


