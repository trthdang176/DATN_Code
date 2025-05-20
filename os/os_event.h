#ifndef __OS_EVENT_H__
#define __OS_EVENT_H__

#include "os.h"

void dynamic_event_pool_init(void);
void free_dynamic_event(OS_event_t *e);
OS_event_t *get_dynamic_event(void);
void set_sig_dynamic_event(OS_event_t *e, uint8_t sig);
void set_data_dynamic_event(OS_event_t *e, uint8_t *data, uint32_t len);
uint8_t *get_data_dynamic_event(OS_event_t *e);
uint32_t get_len_dynamic_event(OS_event_t *e);

#endif /* __OS_EVENT_H__ */
