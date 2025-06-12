#include "app_rtc.h"

#include "app_main.h"
#include "../Core/Inc/main.h"

DS3231_t ds3231;

void ds3231_task_init(I2C_HandleTypeDef* i2c_port, uint8_t dev_address) {
    DS3231_Init(&ds3231,i2c_port,dev_address);
}