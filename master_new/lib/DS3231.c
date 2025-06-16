#include "DS3231.h"

static uint8_t decToBcd(int val);
static int bcdToDec(uint8_t val);

DS3231_Status DS3231_Init(DS3231_t *pDS3231, I2C_HandleTypeDef *i2c, uint16_t dev_address) {
    pDS3231->i2c_port = i2c;

    pDS3231->dev_address = dev_address << 1;
    if (HAL_I2C_IsDeviceReady(pDS3231->i2c_port,pDS3231->dev_address,1,100) == HAL_OK) {
    	printf("Init DS3231 Success\n");
      return DS3231_Init_OK;
    }
    return DS3231_Fail;
}

DS3231_Status DS3231_Read_time(DS3231_t *pDS3231, uint8_t *data_read) {
    uint8_t get_time[7];
    if (HAL_I2C_Mem_Read(pDS3231->i2c_port, pDS3231->dev_address, 0x00, I2C_MEMADD_SIZE_8BIT, get_time, 7, 1000) != HAL_OK) {
      return DS3231_Fail;
    }
	
  data_read[0] = bcdToDec(get_time[0]);
	data_read[1] = bcdToDec(get_time[1]);
	data_read[2] = bcdToDec(get_time[2]);
	data_read[3] = bcdToDec(get_time[3]);
	data_read[4] = bcdToDec(get_time[4]);
	data_read[5] = bcdToDec(get_time[5]);
	data_read[6] = bcdToDec(get_time[6]);
  return DS3231_Write_OK;
}

DS3231_Status DS3231_Write_time(DS3231_t *pDS3231,uint8_t sec, uint8_t min, uint8_t hour, uint8_t day, uint8_t month, uint8_t year) {
	uint8_t set_time[7];
	set_time[0] = decToBcd(sec);
	set_time[1] = decToBcd(min);
	set_time[2] = decToBcd(hour);
	set_time[3] = decToBcd(7);
	set_time[4] = decToBcd(day);
	set_time[5] = decToBcd(month);
	set_time[6] = decToBcd(year);

	if (HAL_I2C_Mem_Write(pDS3231->i2c_port, pDS3231->dev_address, 0x00, I2C_MEMADD_SIZE_8BIT, set_time, 7, 1000) != HAL_OK) {
    return DS3231_Fail;
  }
  return DS3231_Read_OK;
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

