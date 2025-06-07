#ifndef __DS3231_H__
#define __DS3231_H__

#include "stdint.h"
#include "../Core/Inc/main.h"
#include "../Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal.h"


#define ADDRESS_DS3231 0x68

typedef enum {
    DS3231_Init_OK   = 0,
    DS3231_Fail         ,
    DS3231_Write_OK     ,
    DS3231_Read_OK      
} DS3231_Status;

typedef struct {
    I2C_HandleTypeDef *i2c_port;

    uint8_t dev_address; /* address 8 bit */
} DS3231_t;


DS3231_Status DS3231_Init(DS3231_t *pDS3231, I2C_HandleTypeDef *i2c, uint16_t dev_address);
DS3231_Status DS3231_Write_time(DS3231_t *pDS3231,uint8_t sec, uint8_t min, uint8_t hour, uint8_t dow, uint8_t dom, uint8_t month, uint8_t year);
DS3231_Status DS3231_Read_time(DS3231_t *pDS3231, uint8_t *data_read);

#endif /* __DS3231_H__ */
