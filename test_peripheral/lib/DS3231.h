#ifndef __DS3231_H__
#define __DS3231_H__

#include "stdint.h"
#include "../Core/Inc/main.h"

#define ADDRESS_DA32321 0x68

typedef enum {
    DS3231_Init_OK   = 0,
    DS3231_Fail         ,
    DS3231_Write_OK     ,
    DS3231_Read_OK      
} DS3231_Status;

typedef enum {
    I2C_HandleTypeDef *i2c_port;

    uint8_t dev_address; /* address 8 bit */
} DS3231_t;


DS3231_Status DS3231_Init(ADS1115_t *pADS1115, I2C_HandleTypeDef *i2c, uint16_t dev_address);
DS3231_Status DS3231_Read_time(ADS1115_t *pADS1115, uint8_t *data_read);


#endif /* __DS3231_H__ */