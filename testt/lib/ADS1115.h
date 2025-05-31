#ifndef __ADS_1115_H__
#define __ADS_1115_H__

#include "stdint.h"
#include <string.h>

#include "../Core/Inc/main.h"

#define ADS1115_READ_ADC_ADDRESS   0x48

#define DEFAULT_VALUE_CONFIG_REG   0x8583
#define DEFAULT_VALUE_CHANNEL      CHANNEL_AIN0_AIN1
#define DEFAULT_VALUE_PGA          PGA_2_048
#define DEFAULT_VALUE_MODE         MODE_SINGLE_SHOT
#define DEFAULT_VALUE_DATARATE     DRATE_128
#define DEFAULT_VALUE_COMP_MODE    COMP_HYSTERESIS
#define DEFAULT_VALUE_COMP_POL     POLARITY_ACTIVE_LOW
#define DEFAULT_VALUE_COMP_LAT     LATCHING_NONE
#define DEFAULT_VALUE_COMP_QUE     QUEUE_DISABLE

typedef enum{
	CHANNEL_AIN0_AIN1 = 0,
	CHANNEL_AIN0_AIN3,
	CHANNEL_AIN1_AIN3,
	CHANNEL_AIN2_AIN3,
	CHANNEL_AIN0_GND,
	CHANNEL_AIN1_GND,
	CHANNEL_AIN2_GND,
	CHANNEL_AIN3_GND
}MultiplexerConfig_t;

typedef enum{
	PGA_6_144 = 0,
	PGA_4_096,
	PGA_2_048,
	PGA_1_024,
	PGA_0_512,
	PGA_0_256
}PGA_Config_t;

typedef enum{
	MODE_CONTINOUS = 0,
	MODE_SINGLE_SHOT
}OperatingMode_t;

typedef enum{
	DRATE_8 = 0,
	DRATE_16,
	DRATE_32,
	DRATE_64,
	DRATE_128,
	DRATE_250,
	DRATE_475,
	DRATE_860
}DataRate_t;

typedef enum{
	COMP_HYSTERESIS = 0,
	COMP_WINDOW
}CompareMode_t;

typedef enum{
	POLARITY_ACTIVE_LOW = 0,
	POLARITY_ACTIVE_HEIGH
}ComparePolarity_t;

typedef enum{
	LATCHING_NONE = 0,
	LATCHING_COMPARATOR
}LatchingMode_t;

typedef enum{
	QUEUE_ONE,
	QUEUE_TWO,
	QUEUE_FOUR,
	QUEUE_DISABLE
}QueueComparator_t;

typedef enum {
    ADS1115_Init_OK      = 0,
    ADS1115_Init_Fail       ,
    ADS1115_Read_Data_Fail  
} ADS115_Status; 

typedef struct {
    MultiplexerConfig_t channel;
    DataRate_t          DataRate;
    PGA_Config_t        PGA;
    OperatingMode_t     mode;
    CompareMode_t		compareMode;
	ComparePolarity_t	polarityMode;
	LatchingMode_t		latchingMode;
	QueueComparator_t	queueComparator;
} config_reg_t;

typedef struct {
    I2C_HandleTypeDef* i2c_port;

    uint8_t dev_address; /* address 8 bit */
    float voltageConv;
    config_reg_t *config;
} ADS1115_t;

ADS115_Status ADS1115_Init(ADS1115_t *pADS1115, I2C_HandleTypeDef *i2c, uint16_t dev_address);
float ADS1115_single_getdata(ADS1115_t *pADS1115, MultiplexerConfig_t channel);

#endif /* __ADS_1115_H__ */

