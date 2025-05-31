#ifndef __APP_CAN_BUS_H__
#define __APP_CAN_BUS_H__

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "../os/os.h"

extern OS_task * const AO_task_can_bus;

void app_can_bus_init(CAN_HandleTypeDef *pCan);

#endif /* __APP_CAN_BUS_H__ */