#include "app_uart_esp32.h"

/* app header */
#include "app_main.h"
#include "../Core/Inc/main.h"

#include <stdbool.h>


typedef struct {
    OS_task task;

    UART_HandleTypeDef *pUart;
    // uart_esp32_t **uart_esp32_buf;
    // uint8_t head_index;
    // uint8_t tail_index;
    // uint8_t nUsed;
    // uint8_t end;

} app_uart_esp32;

void uart_esp32_ctor(app_uart_esp32 *pAO);
static void uart_esp32_init(app_uart_esp32 * const pOS_task, OS_event_t const * const pEvent);
static void uart_esp32_dispatch(app_uart_esp32 * const pOS_task, OS_event_t const * const pEvent);


static app_uart_esp32 uart_esp32_instance;
OS_task * const AO_task_uart_esp32 = &uart_esp32_instance.task;


void uart_esp32_task_init(UART_HandleTypeDef *pUart) {
    uart_esp32_instance.pUart = pUart;
    
    uart_esp32_ctor(&uart_esp32_instance);

}

void uart_esp32_ctor(app_uart_esp32 *pAO) {
    OS_task_init(AO_task_uart_esp32,(OS_handler)&uart_esp32_init,(OS_handler)&uart_esp32_dispatch);
}

static void uart_esp32_init(app_uart_esp32 * const pOS_task, OS_event_t const * const pEvent) {
    UNUSED(pOS_task);
    UNUSED(pEvent);
}

static void uart_esp32_dispatch(app_uart_esp32 * const pOS_task, OS_event_t const * const pEvent) {
    switch (pEvent->sig) {
        case SEND_DATA_ESP32 : {
            printf("Send data to esp32\n");
            uart_esp32_t *data_send_esp32 = (uart_esp32_t *)(*(uint32_t *)get_data_dynamic_event(pEvent));
            HAL_UART_Transmit(pOS_task->pUart,data_send_esp32->data,data_send_esp32->len,1000);

            if (data_send_esp32->data != NULL) free(data_send_esp32->data);
            free(data_send_esp32);
        } break;
        case RECEIVE_DATA_ESP32 : {
            
        } break;

        default: break;
    }
}


