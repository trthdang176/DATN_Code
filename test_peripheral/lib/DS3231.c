#include "DS3231.h"

static uint8_t decToBcd(int val);
static int bcdToDec(uint8_t val);

DS3231_Status DS3231_Init(ADS1115_t *pADS1115, I2C_HandleTypeDef *i2c, uint16_t dev_address) {
    pADS1115.i2c_port = i2c;

    pADS1115.dev_address = dev_address << 1;
    if (HAL_I2C_IsDeviceReady(pADS1115.i2c_port,pADS1115.dev_address,1,100) == HAL_OK) {
        return DS3231_Init_OK;
    }
    return DS3231_Fail;
}

DS3231_Status DS3231_Read_time(ADS1115_t *pADS1115, uint8_t *data_read) {
    uint8_t get_time[7];
    HAL_I2C_Mem_Read(pADS1115->i2c_port, pADS1115->dev_address, 0x00, I2C_MEMADD_SIZE_8BIT, data_read, 7, 1000);
	
    data_read[0] = bcdToDec(get_time[0]);
	data_read[1] = bcdToDec(get_time[1]);
	data_read[2] = bcdToDec(get_time[2]);
	data_read[3] = bcdToDec(get_time[3]);
	data_read[4] = bcdToDec(get_time[4]);
	data_read[5] = bcdToDec(get_time[5]);
	data_read[6] = bcdToDec(get_time[6]);
}

// Convert normal decimal numbers to binary coded decimal
static uint8_t decToBcd(int val)
{
  return (uint8_t)( (val/10*16) + (val%10) );
}
// Convert binary coded decimal to normal decimal numbers
static int bcdToDec(uint8_t val)
{
  return (int)( (val/16*10) + (val%16) );
}

