#include "app_post_test.h"
#include "app_main.h"
#include "../Core/Inc/main.h"

/* the app task */
typedef struct {
    OS_task task;

    OS_TimeEvt te;
} app_post;

void PostApp_ctor(app_post *pAO);
void PostApp_init(app_post * const pOS_task, OS_event_t const * const pEvent);
void PostApp_dispatch(app_post * const pOS_task, OS_event_t const * const pEvent);

static app_post Post_instance;
OS_task * const AO_taskPost = &Post_instance.task;

data_send_t data_send;

void Post_task_init(void) {
    PostApp_ctor(&Post_instance);

    strcpy(data_send.string_post,(char *)"Hello task test: ");
    data_send.string_len = strlen(data_send.string_post);
    data_send.data_post = 0;
}

void PostApp_ctor(app_post *pAO) {
    /* init task */
    OS_task_init(AO_taskPost,(OS_handler)&PostApp_init,(OS_handler)&PostApp_dispatch);

    /* init time */
    // OS_TimeEvt_init(&Post_instance.te,POST_DATA_TIMEOUT,&Post_instance.task);
}

void PostApp_init(app_post * const pOS_task, OS_event_t const * const pEvent) {
    // OS_TimeEvt_Set(&pOS_task->te,20,500);
    // OS_TimeEvt_Stop(&Post_instance.te);
}

void PostApp_dispatch(app_post * const pOS_task, OS_event_t const * const pEvent) {
    switch (pEvent->sig) {
        case POST_DATA_TIMEOUT : {
//            printf("Post data to test task!\n");
            // data_send_t *pdata_send = malloc(sizeof(data_send_t*));
            ++data_send.data_post;
            // memcpy(pdata_send->string_post,data_send.string_post,data_send.string_len);
            // pdata_send->string_len = data_send.string_len;
            // pdata_send->data_post = data_send.data_post;
            // if (AO_BlinkyTest->dispatch != NULL) {
            //     OS_task_post_event(AO_BlinkyTest,PRINT_POST_DATA,(uint8_t *)&data_send,sizeof(data_send_t));
            // }
        }
        default: break;
    }
}

