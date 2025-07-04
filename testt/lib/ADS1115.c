#include "ADS1115.h"

void ADS1115_makeConfig_frame(uint8_t *buffer,config_reg_t *reg_config);
void compute_VoltageConv(ADS1115_t *pADS1115);
float getValue(ADS1115_t *pADS1115);
void ADS1115_setThresholds(ADS1115_t *pADS1115, int16_t lowValue, int16_t highValue);

ADS115_Status ADS1115_Init(ADS1115_t *pADS1115, I2C_HandleTypeDef *i2c, uint16_t dev_address) {
    pADS1115->i2c_port = i2c;
//    memcpy(pADS1115->i2c_port,i2c,sizeof(I2C_HandleTypeDef));
    pADS1115->dev_address = dev_address << 1;

    compute_VoltageConv(pADS1115);
    /* Test communication */
    if (HAL_I2C_IsDeviceReady(pADS1115->i2c_port, pADS1115->dev_address, 1, 100) == HAL_OK) {
        return ADS1115_Init_OK;
    }
    return ADS1115_Init_Fail;
}

float ADS1115_single_getdata(ADS1115_t *pADS1115, MultiplexerConfig_t channel) {
    uint8_t  config_val[2];
    uint16_t config_check;
    /* update channel */
    uint8_t bytes[3] = {0};
    pADS1115->config->channel = channel;

    ADS1115_makeConfig_frame(bytes,pADS1115->config);
    /* Set single conversion */
    bytes[1] |= (1u << 7);

    HAL_I2C_Master_Transmit(pADS1115->i2c_port,pADS1115->dev_address,bytes,3,100);
    HAL_Delay(8);

    // ADS1115_setThresholds(pADS1115,0x0000,0x8000);
    // HAL_Delay(8);

    // if (HAL_I2C_Mem_Read( pADS1115->i2c_port, pADS1115->dev_address, 0x01, I2C_MEMADD_SIZE_8BIT, config_val, 2, 100 ) != HAL_OK ) {
    // 	return -1;
    // }
    /* wait conversion complete */
    do {
        HAL_I2C_Mem_Read( pADS1115->i2c_port, pADS1115->dev_address, 0x01, I2C_MEMADD_SIZE_8BIT, config_val, 2, 100 );
        config_check = ((config_val[0] << 8) | config_val[1]);
    } while ((config_check & 0x8000) == 0);
    
    return getValue(pADS1115);
}

float getValue(ADS1115_t *pADS1115) {
    uint8_t bytes[2] = {0};
    int16_t value_ret;
	bytes[0] = 0x00; /* Conversion register contains the result */

    // HAL_I2C_Master_Transmit(pADS1115->i2c_port,pADS1115->dev_address,bytes,1,100);

    // if (HAL_I2C_Master_Receive(pADS1115->i2c_port,pADS1115->dev_address,bytes,2,100) != HAL_OK) {
    //     return ADS1115_Read_Data_Fail;
    // }
    HAL_I2C_Mem_Read(pADS1115->i2c_port, pADS1115->dev_address, 0x00, I2C_MEMADD_SIZE_8BIT, bytes, 2, 100);

    value_ret = ((bytes[0] << 8) | bytes[1]);
    /* Convert to Voltage */
    return value_ret * pADS1115->voltageConv;
}

void ADS1115_updateConfig(ADS1115_t *pADS1115) {
    uint8_t byte_write[3] = {0}; /* config register, MSB of config register, LSB of config register */
    ADS1115_makeConfig_frame(byte_write,pADS1115->config);

    HAL_I2C_Master_Transmit(pADS1115->i2c_port,pADS1115->dev_address,byte_write,3,100);
}

void ADS1115_makeConfig_frame(uint8_t *buffer,config_reg_t *reg_config) {
    buffer[0] = 0x01; /* points to Config register */
    buffer[1] = (reg_config->channel << 4) | (reg_config->PGA << 1) | (reg_config->mode << 0);
    buffer[2] = (reg_config->DataRate << 5) | (reg_config->compareMode << 4) | (reg_config->polarityMode << 3) | (reg_config->latchingMode << 2) | (reg_config->queueComparator << 0);
}

void compute_VoltageConv(ADS1115_t *pADS1115) {
    switch (pADS1115->config->PGA) {
        case PGA_6_144 : {
            pADS1115->voltageConv = (6.144f)/(32768.0f);
        } break;
        case PGA_4_096 : {
            pADS1115->voltageConv = (4.096f)/(32768.0f);
        } break;
        case PGA_2_048 : {
            pADS1115->voltageConv = (2.048f)/(32768.0f);
        } break;
        case PGA_1_024 : {
            pADS1115->voltageConv = (1.024f)/(32768.0f);
        } break;
        case PGA_0_512 : {
            pADS1115->voltageConv = (0.512f)/(32768.0f);
        } break;
        case PGA_0_256 : {
            pADS1115->voltageConv = (0.256f)/(32768.0f);
        } break; 
        
        default : break;
    }
}

void ADS1115_setThresholds(ADS1115_t *pADS1115, int16_t lowValue, int16_t highValue){
	uint8_t ADSWrite[3] = { 0 };

	//hi threshold reg
	ADSWrite[0] = 0x03;
	ADSWrite[1] = (uint8_t)((highValue & 0xFF00) >> 8);
	ADSWrite[2] = (uint8_t)(highValue & 0x00FF);
	HAL_I2C_Master_Transmit(pADS1115->i2c_port,pADS1115->dev_address,ADSWrite,3,100);
    HAL_Delay(10);
	//lo threshold reg
	ADSWrite[0] = 0x02;
	ADSWrite[1] = (uint8_t)((lowValue & 0xFF00) >> 8);
	ADSWrite[2] = (uint8_t)(lowValue & 0x00FF);
	HAL_I2C_Master_Transmit(pADS1115->i2c_port,pADS1115->dev_address,ADSWrite,3,100);
}
