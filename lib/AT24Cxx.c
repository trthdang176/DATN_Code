#include "AT24Cxx.h"
#include "../Core/Inc/main.h"

#include <stdio.h>
#include <stdint.h>

/* Private function prototype */

AT24Cxx_Status AT24Cxx_Init(AT24Cxx_t *pDev, uint8_t dev_address, I2C_HandleTypeDef *dev_i2c) {
    pDev->i2c_port = dev_i2c;
    pDev->dev_address = dev_address << 1;
    /* Test communication */
    //uint8_t test_buf[1] = {0};
    if (HAL_I2C_IsDeviceReady(pDev->i2c_port, pDev->dev_address, 1, 100) == HAL_OK) {
        return AT24Cxx_Init_OK;
    }
    return AT24Cxx_Init_Fail;
    HAL_Delay(1);
    
}

//AT24Cxx_Status AT24Cxx_write_buffer(AT24Cxx_t *pDev, uint16_t start_addr, uint8_t* data_buf, uint16_t buf_length) {
//    uint8_t byte_in_fisrtPage = AT24C256_PG_SIZE - start_addr % AT24C256_PG_SIZE;   /* Number of bytes writable to first page from address */
//    uint16_t page_write = (buf_length - byte_in_fisrtPage) / AT24C256_PG_SIZE;      /* Number of pages remaining after writing to the first page */
//    uint8_t byte_redundant = (buf_length - byte_in_fisrtPage) % AT24C256_PG_SIZE;   /* Number of redundant bytes after writing to all pages */
//}

AT24Cxx_Status AT24Cxx_write_page(AT24Cxx_t *pDev, uint16_t mem_addr, uint8_t* data_buf) {
    /* Check the vaid memory address */
    if (mem_addr + AT24C256_PG_SIZE < AT24C256_MAX_ADDR) { 
        HAL_I2C_Mem_Write_IT(pDev->i2c_port,pDev->dev_address,mem_addr,I2C_MEMADD_SIZE_16BIT,data_buf,AT24C256_PG_SIZE);
        return AT24Cxx_OK;
    } else {
        return AT24Cxx_Error_write;
    }
}

AT24Cxx_Status AT24Cxx_write_buffer(AT24Cxx_t *pDev, uint16_t mem_addr, uint8_t* data_buf, uint16_t buf_length) {
    /* Check valid size buffer and address */
    if (mem_addr+buf_length < AT24C256_MAX_ADDR && buf_length < AT24C256_PG_SIZE) {
        HAL_I2C_Mem_Write_IT(pDev->i2c_port,pDev->dev_address,mem_addr,I2C_MEMADD_SIZE_16BIT,data_buf,buf_length);
        return AT24Cxx_OK;
    } else {
        return AT24Cxx_Error_write;
    }
}

AT24Cxx_Status AT24Cxx_read_buffer(AT24Cxx_t *pDev, uint16_t mem_addr, uint8_t *data_ret, uint16_t buf_length) {
    if (mem_addr < AT24C256_MAX_ADDR) {
        HAL_I2C_Mem_Read(pDev->i2c_port,pDev->dev_address,mem_addr,I2C_MEMADD_SIZE_16BIT,data_ret,buf_length,10);
        return AT24Cxx_OK;
    } else {
        return AT24Cxx_Error_read;
    }
}

