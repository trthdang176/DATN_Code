/*
 * Control_IC.h
 *
 *  Created on: Mar 27, 2025
 *      Author: dangtt
 */

#ifndef INC_CONTROL_IC_H_
#define INC_CONTROL_IC_H_

#include "main.h"
#include <string.h>
#include <stdbool.h>
#include "../../lib/ADS1115.h"

#define DSA1  GPIO_PIN_15
#define DSB1  GPIO_PIN_14
#define CP1   GPIO_PIN_13
#define MR1   GPIO_PIN_12

#define DSA2  GPIO_PIN_11
#define DSB2  GPIO_PIN_10
#define CP2   GPIO_PIN_9
#define MR2   GPIO_PIN_8

#define DSA3  GPIO_PIN_13
#define DSB3  GPIO_PIN_12
#define CP3   GPIO_PIN_11
#define MR3   GPIO_PIN_10

// postion bit LE in shift ic -> the bit is reverse 
#define LE1_Bit 3     
#define LE2_Bit 7
#define LE3_Bit 5 
#define LE4_Bit 1
#define LE5_Bit 5
#define LE6_Bit 7
#define LE7_Bit 3

#define ENABLE_OE_1 0x00 // 0b 0000 0000 OE = 0 : enable IC, LE = 0 : lock ic 
#define ENABLE_OE_2 0x00

#define PIN_IC_TEST_1                 1 
#define PIN_IC_TEST_2                 2 
#define PIN_IC_TEST_3                 3 
#define PIN_IC_TEST_4                 4 
#define PIN_IC_TEST_5                 5 
#define PIN_IC_TEST_6                 6 
#define PIN_IC_TEST_7                 7 
#define PIN_IC_TEST_8                 8 
#define PIN_IC_TEST_9                 9 
#define PIN_IC_TEST_10                10 
#define PIN_IC_TEST_11                11 
#define PIN_IC_TEST_12                12 
#define PIN_IC_TEST_13                13 
#define PIN_IC_TEST_14                14 
#define PIN_IC_TEST_15                15 
#define PIN_IC_TEST_16                16

enum {
    Shift_IC1 = 0,
    Shift_IC2    ,
    Shift_IC3    ,
    Shift_IC_MAX
};

enum {
    Latch_IC1  = 0,
    Latch_IC2     ,
    Latch_IC3     ,
    Latch_IC4     ,
    Latch_IC5     ,
    Latch_IC6     ,
    Latch_IC7     ,  
    Latch_IC_MAX
};

enum {
    PULL_UP = 0,
    PULL_DOWN 
};


typedef struct {
    GPIO_TypeDef *Port_x;
    uint16_t PIN_x;
} Pin_IC_Test_Mapping;

typedef enum {
    TEST_SHORT_CIRCUIT  = 0,
    TEST_FUNCTION          ,
    FINISH_TEST
} Test_State_t;

typedef struct {
    bool isShort;

    Test_State_t cur_state;
    uint8_t cur_case;

    uint8_t *data_control_testing;
    uint8_t data_control_first_state[300];
    uint8_t data_control_second_state[300];
    float *result_short_circuit;
    uint8_t *result_test_function;

    uint8_t result_case[20];
    uint8_t result_short_pin[20];

    uint8_t data_test[500];
    uint16_t data_test_len;

    uint8_t num_case;
    uint8_t num_pin;
} Control_IC_Test_t;

void Control_IC_begin(void);
void Control_Vcc_pin(uint8_t data);
void Latch_IC_begin(void);

void shift_out(uint8_t num, uint8_t *data);

void WritePin_ICTest(uint8_t *dataPin);
void ReadPin_IC_test(uint8_t *dataPin,uint8_t num_pin);

void WritePin_Ron(uint8_t *dataPin);
void WritePin_CurrentLeakage(uint8_t *dataPin);

void Read_ADC_IC_test(ADS1115_t *pADS1115, uint8_t pin,float *data_buf);

void convert_data_test(uint8_t num_pin, char *data_test, uint8_t *data_control);
uint8_t convert_data_compare(char c_input);

bool has_clock_transition(uint8_t num_pin, char *data_test);
void convert_data_test_first_state(uint8_t num_pin, char *data_test, uint8_t *data_control);
void convert_data_test_second_state(uint8_t num_pin, char *data_test, uint8_t *data_control);

void TurnOn_short_circuit(void);
void TurnOff_short_circuit(void);

void reset_TXS(void);

#endif /* INC_CONTROL_IC_H_ */
