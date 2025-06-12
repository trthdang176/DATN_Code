#ifndef __APP_CAN_BUS_H__
#define __APP_CAN_BUS_H__

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "../os/os.h"
#include "Screen.h"

typedef struct {
    uint32_t ID;
    bool TX_Done;
} managerTX_CAN_t;

extern OS_task * const AO_task_can_bus;

void can_bus_task_init(CAN_HandleTypeDef *pCan);

#endif /* __APP_CAN_BUS_H__ */