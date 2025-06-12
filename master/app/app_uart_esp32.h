#ifndef __APP_UART_ESP32_H__
#define __APP_UART_ESP32_H__

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "../os/os.h"

typedef struct {
    char *data; /* data */
    uint32_t len;
} uart_esp32_t;

extern OS_task * const AO_task_uart_esp32;

void uart_esp32_task_init(UART_HandleTypeDef *pUart);



#endif /* __APP_UART_ESP32_H__ */
