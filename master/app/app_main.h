/**
 * @file app_main.h
 * @author DANGTT
 * @brief the header file for OS tasker 
 * @version 0.1
 * @date 2025-05-16
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef __APP_MAIN_H__ 
#define __APP_MAIN_H__

#include "test_os.h"
#include "app_eeprom.h"
#include "app_uart_esp32.h"
#include "app_can_bus.h"
#include "app_screen.h"

#include "../os/os.h"
#include "../os/os_event.h"

#define TEST_OS_TICK_PER_SEC 1000u

#define ARRAY_ELEMENT(array) (sizeof(array)/sizeof(array[0]))

/* Event signal */
enum TestOS_sig {
    TIMEOUT1_SIG,
    TIMEOUT2_SIG,
    POST_DATA_TIMEOUT,
    PRINT_POST_DATA,
    READ_ALL_PROGRAM_TEST,
    READ_ONE_PROGRAM_TEST,
    WRITE_PROGRAM_TEST,
    TEST_SEND_DATA_ESP32,

    MAX_SIG
};

/* eeprom signal */
enum {
    WAIT_WRITE_TIMEOUT = 0,
    CHECK_WRITE_BUF       ,
    WRITE_EEPROM_IN_BUF   ,
    FINISH_WRITE          ,
    READ_EEPROM           ,
    WRITE_EEPROM          ,

    MAX_SIG_EEPROM
};

/* Uart esp32 signal */ 
enum {
    SEND_DATA_ESP32   = 0,
    RECEIVE_DATA_ESP32   ,

    MAX_SIG_UART_ESP32
};

/* Can bus signal */
enum {
    SEND_DATA_CAN_BUS  = 0,
    RECEIVE_DATA_CAN_BUS  ,

    MAX_SIG_CAN_BUS
};

/* screen signal */
enum {
    DEIVCE_TEST_FINISH  = 0,

    MAX_SIG_SCREEN
};


void BSP_init(void);

OS_event_t const *TestOS_Work(void);

#endif /* __APP_MAIN_H__ */