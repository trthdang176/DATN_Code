#ifndef __APP_POST_TEST_H__
#define __APP_POST_TEST_H__

#include <stdint.h>
#include <string.h>
#include "../os/os.h"

#define EEPROM_ADDRESS 0x50

//typedef struct {
//    uint32_t data_post;
//    uint32_t string_len;
//    char string_post[64];
//} data_send_t;

//extern OS_task * const AO_taskPost;

void Post_task_init(void);

#endif /* __APP_POST_TEST_H__ */
