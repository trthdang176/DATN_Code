#ifndef __APP_SCREEN_H__
#define __APP_SCREEN_H__

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "../os/os.h"
#include "Screen.h"

typedef struct {
    uint8_t device;
    
} Screen_finish_data_t;

extern OS_task * const AO_task_screen;
void screen_task_init(void);


#endif /* __APP_SCREEN_H__ */