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

    MAX_SIG
};

void BSP_init(void);

OS_event_t const *TestOS_Work(void);

#endif /* __APP_MAIN_H__ */