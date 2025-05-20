#include "test_os.h"
#include "app_main.h"
#include "../Core/Inc/main.h"

/* the app task */
typedef struct {
    OS_task task;  /* inherit os_task */
    
    /* External data app */
    OS_TimeEvt te1; /* time event */   
    OS_TimeEvt te2; /* time event */   
} Test_Blinky_OS; 

void BlinkyTest_ctor(Test_Blinky_OS * const pAO);
void BlinkyTest_init(Test_Blinky_OS * const pOS_task, OS_event_t const * const pEvent);
void BlinkyTest_Dispatch(Test_Blinky_OS *const pOS_task, OS_event_t const * const pEvent);

static Test_Blinky_OS Blinky_instance; /* the app instance */
OS_task * const AO_BlinkyTest = &Blinky_instance.task; /* AO task pointer */

/* call in main file to initialization Active object of app using */
void BlinkyTest_app_init(void) {
    BlinkyTest_ctor(&Blinky_instance);
}


void BlinkyTest_ctor(Test_Blinky_OS * const pAO) {
    /* Init task */
    OS_task_init(&pAO->task,(OS_handler *)&BlinkyTest_init,(OS_handler *)&BlinkyTest_Dispatch);

    /* init time os if using */
    OS_TimeEvt_init(&pAO->te1,TIMEOUT1_SIG,&pAO->task);
    OS_TimeEvt_init(&pAO->te2,TIMEOUT2_SIG,&pAO->task);
}

/* The init function of app task (optional) */
void BlinkyTest_init(Test_Blinky_OS * const pOS_task, OS_event_t const * const pEvent) {
    
    /* init time for generate event */
    OS_TimeEvt_Set(&pOS_task->te1,
    1U,
    OS_EVT_DOWNCAST(TestOS_Evt,pEvent)->ticks1);

    OS_TimeEvt_Set(&pOS_task->te2,
    1U + (OS_EVT_DOWNCAST(TestOS_Evt,pEvent)->ticks2 / 4),
    OS_EVT_DOWNCAST(TestOS_Evt,pEvent)->ticks2);

    /* if don't using this function */
    /*
    * UNSED(pOS_task);
    * UNSED(pEvent);
    */
}

void BlinkyTest_Dispatch(Test_Blinky_OS *const pOS_task, OS_event_t const * const pEvent) {
    switch (pEvent->sig) {
        case TIMEOUT1_SIG : {
            /* Turn on led */
            HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_RESET);
            printf("Turn on led\n");
        } break;
        case TIMEOUT2_SIG : {
            /* Turn off led */
        	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_SET);
            printf("Turn off led\n");
        } break;
        case PRINT_POST_DATA : {
            printf("Receive data post\n");
            data_send_t *data_receive = (data_send_t *)(get_data_dynamic_event(pEvent));
             printf("Data receive: %ld\n",data_receive->data_post);
        }
        default: break;
    }
}
