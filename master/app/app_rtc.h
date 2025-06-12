#ifndef __APP_RTC_H__
#define __APP_RTC_H__

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "../os/os.h"
#include "../lib/DS3231.h"


extern DS3231_t ds3231;
void ds3231_task_init(I2C_HandleTypeDef* i2c_port, uint8_t dev_address);

#endif /* __APP_RTC_H__ */