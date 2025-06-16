#ifndef __APP_SD_H__
#define __APP_SD_H__

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "../os/os.h"


#define MAX_STRING_LOGGING 150

typedef struct {
    uint32_t timestamp;
    char *String_logging;
} DataLogging_t;

typedef struct {
    uint32_t file_position;
}IndexEntry ;

extern OS_task * const AO_task_sd;

void sd_task_init(void);

#endif /* __APP_SD_H__ */

