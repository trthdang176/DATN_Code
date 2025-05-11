#ifndef __SCREEN_H__
#define __SCREEN_H__

#include <stdlib.h>
#include <stdio.h>

#include "../lib/DWIN_HMI.h"
#include "../lib/Common.h"
#include "../Core/Inc/main.h"

#define MAX_VALUE_OF_SIG 20             // Check again !!!
#define MAX_DATA_KEYBOARD_INPUT 50 



#define DWINPAGE_MAIN                   100
#define DWINPAGE_MAIN_FINISH            101
#define DWINPAGE_MAIN_DETAIL            102
#define DWINPAGE_MAIN_GRAPH             111
#define DWINPAGE_SETTING                104
#define DWINPAGE_SETTING_WIFI           105
#define DWINPAGE_SETTING_TIME           106
#define DWINPAGE_KEYBOARD_TIME          107   

#define VP_Navigation_button        0x5000
#define VP_ON_OFF_button            0x5100
#define VP_Program_Run              0x5200
#define VP_Navigation_keyboard      0x5300
#define VP_Keyboard                 0xFF00

#define VP_Hour                     0x9000
#define VP_Minute                   0x9010
#define VP_Day                      0x9020
#define VP_Month                    0x9030
#define VP_Year                     0x9040

#define VP_ShowString_Keyboard      0xF000
#define VP_ShowUnit_Keyboard        0xF100
#define VP_ShowWarning_Keyboard     0xF200

#define VP_ON_OFF_Icon_1            0x4000

enum {
    SIG_NAVIGATION = 0,
    SIG_ON_OFF        ,
    SIG_KEYBOARD      ,
    SIG_NAVIGATION_KEYBOARD,
    MAX_SIG_VP          // MAX NUMBER OF SIGNALS FROM VP ADDRESS       
};

enum {
    NAVIGATION_SETTING_PAGE         = 0,
    NAVIGATION_RETURN                  ,
    NAVIGATION_HOME                    ,
    NAVIGATION_SETTING_TIME            ,
    NAVIGATION_SETTING_WIFI            ,
    NAVIGATION_SETTING_PROGRAME        ,   
    NAVIGATION_NEXT_MAINPAGE           ,
    NAVIGATION_PREV_MAINPAGE           ,
    MAX_VALUE_SIG_NAVIGATE             
};

enum {
    ON_OFF_DEV_1                    = 0,
    ON_OFF_DEV_2                       ,
    ON_OFF_DEV_3                       ,
    MAX_VALUE_SIG_ON_OFF      
};

enum {
    VALUE_KEBOARD                  = 0,
    MAX_VALUE_KEYBOARD
};

enum {
    VALUE_NAVIGATION_KEYBOARD      = 0,
    MAX_VALUE_NAVIGATION_KEYBOARD
};

enum {
    CONDITION_HOUR            = 0,
    CONDITION_MINUTE             ,
    CONDITION_DAY                ,
    CONDITION_MONTH              ,
    CONDITION_YEAR               ,

    MAX_CONDITION 
};

enum {
    ICON_OFF                        = 0,
    ICON_ON                   
};

enum {
    ICON_INPUT            = 0,
    ICON_OUTPUT              ,
    ICON_GND                 ,
    ICON_VCC                 ,
    ICON_LOGIC_HIGH          ,
    ICON_LOGIC_LOW
};


enum {
    DEVICE_1   = 0,
    DEVICE_2      ,
    DEVICE_3      ,
    MAX_DEVICE 
};

enum {
    PROGRAM_TEST1  = 0,
    PROGRAM_TEST2     ,
    PROGRAM_TEST3     ,
    PROGRAM_TEST4     ,
    MAX_PROGRAM_TEST
};

enum {
    INDEX_NAME_IC    = 0,
    INDEX_DESCRIPTION   ,
    INDEX_NUM_PIN       ,
    INDEX_NUM_CASE
};

typedef enum {
    Status_ERROR         = 0,
    Status_SUCCESS          
} Return_Status;

typedef struct  {
    uint8_t event; // create by VP 
    uint16_t data; // code HMI in VP address
    uint8_t ASCII_Code;
} screen_event_t;

typedef struct Keyboard {
    uint16_t VP_Text; // Vp text show the data input 
    uint8_t  String[MAX_DATA_KEYBOARD_INPUT]; // the string input 
    uint16_t Index_String;
    bool     Caplock;  // Caplock Status
} Keyboard_t;

typedef struct {

} Screen_info_t;

typedef struct {
    float max_value;
    float min_value;
} Screen_condition_t;

typedef struct {
    char *Name_Program;   // Name program test
    char *Name_IC;        // Name IC test
    char *Description_IC; // Descript ion IC test
    uint8_t num_IC;       // The number per testing program
    uint8_t num_case;     // The number test case
    uint8_t num_pin;      // The number pin IC

    uint8_t **data_test;  // Array 2d data testing

} Program_Test_t;

typedef struct {
    uint8_t curr_case;
    uint8_t curr_PageMain;        // The current main page
    uint8_t selected_program_num;
    bool    state;
    
    char **config_pin;          // Array control icon pin 
    char **data_result;         // Array control icon result
} Device_t;

typedef struct  {
    Dwin_t Screen_DWIN;  // inherit from dwin object

    Keyboard_t Screen_keyboard;
    Screen_condition_t *Screen_condition; // The array store condition data input keyboard   
    Device_t IC_Testerx[MAX_DEVICE];
    Program_Test_t Program_Testx[MAX_PROGRAM_TEST];
    void *p_handler_table;
    uint8_t pre_page;      // previous page 
    uint8_t curr_device;   // Device is displaying 
    bool Status_Dev_1,Status_Dev_2,Status_Dev_3;
} Screen_t;

typedef struct  {
    uint16_t VP_address;
    uint8_t index;
} VP_item;

#define SIZE_LOOKUP_VP_SIG (sizeof(lookup_VP_SIG)/sizeof(VP_item))

typedef void (*handler_func)(Screen_t *const screen_obj, screen_event_t *const screen_event);

/* Function Prototype */

uint8_t Index_VP(uint16_t vp, VP_item* VP_array, uint8_t Size_array);
void Screen_begin(UART_HandleTypeDef * UART_Screen);
void Screen_RX_data(uint8_t *RX_Buffer);
void Screen_RX_Callback(uint16_t Vpaddress, uint16_t lastByte, uint8_t ASCII_Code);

/* Callback VP ADDRESS FUNCTION */
void Navigation_setting_page(Screen_t *const screen_obj, screen_event_t *const screen_event);
void Navigation_home_page(Screen_t *const screen_obj, screen_event_t *const screen_event);

#endif /* __SCREEN_H__ */   
