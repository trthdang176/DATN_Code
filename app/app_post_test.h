#ifndef __APP_POST_TEST_H__
#define __APP_POST_TEST_H__

#include <stdint.h>
#include "../os/os.h"

typedef struct {
    uint32_t data_post;
    // uint8_t *string_post;
} data_send_t;

extern OS_task * const AO_taskPost;

void Post_task_init(void);

#endif /* __APP_POST_TEST_H__ */
