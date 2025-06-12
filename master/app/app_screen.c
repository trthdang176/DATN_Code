#include "app_screen.h"

#include "app_main.h"
#include "../Core/Inc/main.h"

typedef struct {
    OS_task task;

    OS_TimeEvt te_rtc;
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

    OS_TimeEvt_init(&pAO->te_rtc,UPDATE_RTC,&pAO->task);
}

static void screen_init(app_screen * const pOS_task, OS_event_t const * const pEvent) {
    OS_TimeEvt_Set(&pOS_task->te_rtc,100,1000);
    UNUSED(pEvent);
} 

static void screen_dispatch(app_screen * const pOS_task, OS_event_t const * const pEvent) {
    switch (pEvent->sig) {
        case DEIVCE_TEST_FINISH : {
            printf("Device test success!!\n");
            uint8_t *device_finish = (uint8_t *)(*(uint32_t *)get_data_dynamic_event(pEvent));
            show_main_page(&_Screen,DWINPAGE_MAIN_FINISH,*device_finish);
            free(device_finish);
        } break;
        case UPDATE_RTC : {
            // printf("Update RTC\n");
            uint8_t data_time[7];
            DS3231_Read_time(&ds3231, data_time);
            char string[20];
            sprintf(string, "%02d/%02d/20%02d", data_time[4], data_time[5], data_time[6] );
            DWIN_SetText(&_Screen,VP_Time_Day,string,strlen(string));
            sprintf(string, "%02d:%02d:%02d", data_time[2], data_time[1], data_time[0] );
            DWIN_SetText(&_Screen,VP_Time_Hour,string,strlen(string));
        } break;
        // case DEVICE_ERROR_TX_CAN : {
        //     warning_page(&_Screen,)
        // }
        default: break;
    }
}
