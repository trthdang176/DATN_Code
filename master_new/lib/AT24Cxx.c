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

AT24Cxx_Status AT24Cxx_write_buffer_bloking(AT24Cxx_t *pDev, uint16_t mem_addr, uint8_t* data_buf, uint16_t buf_length) {
    uint8_t page_remaining = AT24C256_PG_SIZE - (mem_addr % AT24C256_PG_SIZE); /* num bytes written in current page from address */

    if (buf_length < page_remaining) page_remaining = buf_length;
    uint8_t page_write = (buf_length - page_remaining) / AT24C256_PG_SIZE;   /* num pages can write full */
    uint8_t byte_remaining = (buf_length - page_remaining) % AT24C256_PG_SIZE; /* num bytes remain after write all full page */

    /* write in current page */
    if (page_remaining > 0x00 && (mem_addr + page_remaining) < AT24C256_MAX_ADDR) {
        while (HAL_I2C_Mem_Write(pDev->i2c_port,pDev->dev_address,mem_addr,I2C_MEMADD_SIZE_16BIT,data_buf,page_remaining,AT24C256_I2C_TIMOUT) != HAL_OK);
        if (HAL_I2C_IsDeviceReady(pDev->i2c_port, pDev->dev_address, 100,5) == HAL_OK) {
            // printf("transmit i2c success\n");
        }
    }

    /* write full page */
    for (uint8_t cur_page = 0; cur_page < page_write; cur_page ++) {
        if ((mem_addr + page_remaining+ (cur_page * AT24C256_PG_SIZE)) > 0x00 && (mem_addr + page_remaining + (cur_page * AT24C256_PG_SIZE)) < AT24C256_MAX_ADDR) {
            while (HAL_I2C_Mem_Write(pDev->i2c_port,pDev->dev_address,(uint16_t)(mem_addr + page_remaining + (cur_page * AT24C256_PG_SIZE)),
                    I2C_MEMADD_SIZE_16BIT,
                    data_buf + page_remaining + (cur_page * AT24C256_PG_SIZE),
                    AT24C256_PG_SIZE,
                    AT24C256_I2C_TIMOUT) != HAL_OK);
            if (HAL_I2C_IsDeviceReady(pDev->i2c_port, pDev->dev_address, 100,5) == HAL_OK) {
                // printf("transmit i2c success\n");
            }
        }
    }

    /* write bytes after write full page still remain */
    if (byte_remaining != 0) {
        if ((mem_addr + page_remaining + (page_write * AT24C256_PG_SIZE)) > 0x00 && (mem_addr + page_remaining+ (page_write * AT24C256_PG_SIZE)) < AT24C256_MAX_ADDR) {
            while (HAL_I2C_Mem_Write(pDev->i2c_port,pDev->dev_address,
                    (uint16_t)(mem_addr + page_remaining + (page_write * AT24C256_PG_SIZE)),
                    I2C_MEMADD_SIZE_16BIT,
                    data_buf + page_remaining + (page_write * AT24C256_PG_SIZE),
                    byte_remaining,
                    AT24C256_I2C_TIMOUT) == HAL_OK) {
                    } 
            if (HAL_I2C_IsDeviceReady(pDev->i2c_port, pDev->dev_address, 100,5) == HAL_OK) {
                // printf("transmit i2c success\n");
            }
        }
    }
    HAL_Delay(10);
}

AT24Cxx_Status AT24Cxx_read_buffer(AT24Cxx_t *pDev, uint16_t mem_addr, uint8_t *data_ret, uint16_t buf_length) {
    if (mem_addr < AT24C256_MAX_ADDR) {
        while (HAL_I2C_Mem_Read(pDev->i2c_port,pDev->dev_address,mem_addr,I2C_MEMADD_SIZE_16BIT,data_ret,buf_length,1000) != HAL_OK);
        return AT24Cxx_OK;
    } else {
        return AT24Cxx_Error_read;
    }
}

