#ifndef __APP_EEPROM_H__
#define __APP_EEPROM_H__

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "../os/os.h"
#include "../lib/AT24Cxx.h"
#include "Screen.h"

#define START_MEM_ADDR_PROGRAM_TEST     0x10
#define TOTAL_ONE_PROGRAM_TEST_LEN      (MAX_PROGRAM_NAME_SIZE + MAX_IC_NAME_SIZE + MAX_IC_NUM)
#define TOTAL_ALL_PROGRAM_TEST_LEN      (TOTAL_ONE_PROGRAM_TEST_LEN * MAX_PROGRAM_TEST)
#define START_ADDR_PROGRAM_TEST_X(i)    (START_MEM_ADDR_PROGRAM_TEST + (TOTAL_ONE_PROGRAM_TEST_LEN * i) + 3)


typedef struct {
    uint16_t mem_addr;      /* starting memory address */
    uint8_t *data;          /* pointer to buffer data */
    uint16_t data_len;      /* total bytes data */
} data_eeprom_t;

/* parameter using for write eeprom */
typedef struct {
    // uint8_t byte_first_page; /* number of bytes to write in the first page from mem_addr */
    // uint8_t page_write; /* number of full pages to write */
    // uint8_t byte_remain; /* number of bytes remaining after writing full pages */

    uint16_t mem_addr; /* begin address write */
    uint8_t *data;     /* data write */
    uint16_t data_len; /* len of data write */
    uint16_t data_written; /* bytes already written */
    uint16_t data_lastlen; /* length of data just written */

} write_param_t;


extern OS_task * const AO_task_eeprom;

void eeprom_task_init(I2C_HandleTypeDef* i2c_port, uint8_t dev_address);


#endif /* __APP_EEPROM_H__ */
