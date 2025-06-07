#ifndef __AT24C_XX_H__
#define __AT24C_XX_H__

#include "stdint.h"
#include "../Drivers/STM32F1xx_HAL_Driver/Inc/stm32f1xx_hal.h"

#define AT24C256_I2C_TIMOUT		2000

#define AT24C256_MAX_ADDR		0x7FFF /* 32,768 Byte */
#define AT24C256_PG_SIZE		64     /* Byte per page */
#define AT24C256_PAGE_NUM       512    /* Number of pages */

/* Check the size from address is within 1 page */
#define AT24C256_CHECK_IN_PAGE(addr,size) (((addr % AT24C256_PG_SIZE) + size) <= AT24C256_PG_SIZE)
#define AT24C256_ADDR_PAGE(x) (AT24C256_PG_SIZE*x)

typedef enum {
    AT24Cxx_Init_OK    = 0,
    AT24Cxx_Init_Fail     ,
    AT24Cxx_OK            ,
    AT24Cxx_Error_write   ,
    AT24Cxx_Error_read
} AT24Cxx_Status;

typedef struct {
    I2C_HandleTypeDef* i2c_port; /* I2C Port */
    uint8_t dev_address; /* Device's address */

} AT24Cxx_t;

AT24Cxx_Status AT24Cxx_Init(AT24Cxx_t *pDev, uint8_t dev_address, I2C_HandleTypeDef *dev_i2c);
// AT24Cxx_Status AT24Cxx_write_buffer(AT24Cxx_t *pDev, uint16_t start_addr, uint8_t* data_buf, uint16_t buf_length);
AT24Cxx_Status AT24Cxx_write_page(AT24Cxx_t *pDev, uint16_t mem_addr, uint8_t* data_buf);
AT24Cxx_Status AT24Cxx_write_buffer(AT24Cxx_t *pDev, uint16_t mem_addr, uint8_t* data_buf, uint16_t buf_length);
AT24Cxx_Status AT24Cxx_write_buffer_bloking(AT24Cxx_t *pDev, uint16_t mem_addr, uint8_t* data_buf, uint16_t buf_length);
AT24Cxx_Status AT24Cxx_read_buffer(AT24Cxx_t *pDev, uint16_t mem_addr, uint8_t *data_ret, uint16_t buf_length);

#endif /* __AT24C_XX_H__ */
