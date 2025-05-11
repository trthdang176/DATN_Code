#ifndef __SCREEN_H__
#define __SCREEN_H__

#include <stdlib.h>
#include <stdio.h>

#include "../lib/DWIN_HMI.h"
#include "../lib/Common.h"
#include "../Core/Inc/main.h"

#define MAX_VALUE_OF_SIG 20             // Check again !!!
#define MAX_DATA_KEYBOARD_INPUT 50 



#define DWINPAGE_MAIN                   101
#define DWINPAGE_MAIN_FINISH            100
#define DWINPAGE_MAIN_DETAIL            102
#define DWINPAGE_MAIN_GRAPH             111
#define DWINPAGE_SETTING                116
#define DWINPAGE_SETTING_WIFI           105
#define DWINPAGE_SETTING_PROGRAM        107   
#define DWINPAGE_MODIFY_PROGRAM         108
#define DWINPAGE_NUM_KEYBOARD           109
#define DWINPAGE_SETTING_TIME           118
#define DWINPAGE_FULL_KEYBOARD          120

#define VP_Navigation_button            0x5000
#define VP_Selection_button             0x5100
#define VP_Program_Run                  0x5200
#define VP_Num_Keyboard                 0x5300
#define VP_Full_Keyboard                0x5380
#define VP_Save_button                  0x5400
#define VP_Keyboard                     0xFF00
    
#define VP_Hour                         0x9000
#define VP_Minute                       0x9010
#define VP_Day                          0x9020
#define VP_Month                        0x9030
#define VP_Year                         0x9040
#define VP_Name_IC                      0x9100
#define VP_Description_IC               0x9150
#define VP_Num_IC_Test                  0x9200
#define VP_Program_Name_1               0x9225
#define VP_Program_Name_2               0x9250
#define VP_Program_Name_3               0x9275
#define VP_Program_Name_4               0x9300
#define VP_Name_Tester                  0x9350
#define VP_Show_CurrentCase             0x9380
#define VP_Name_Pin                     0x9400 // -> 94F0
#define VP_Modify_Program_Name          0x9500
#define VP_Modify_IC_Name               0x9520
#define VP_Modify_IC_Num                0x9540
    
#define VP_ShowString_Keyboard          0xF000
#define VP_ShowUnit_Keyboard            0xF050
#define VP_ShowType_Keyboard            0xF080
#define VP_ShowWarning_Keyboard         0xF100
    
#define VP_ON_OFF_Icon_1                0x4000
#define VP_ICON_DIRECTION_PIN           0x4050 // -> 0x405F 
#define VP_ICON_SELECT_PROGRAM          0x4100 
#define VP_ICON_RESULT                  0x4150
#define VP_ICON_CAPLOCK                 0x4200

enum {
    SIG_NAVIGATION           = 0,
    SIG_SELECTION               ,
    SIG_KEYBOARD                ,
    SIG_NUM_KEYBOARD            ,
    SIG_FULL_KEYBOARD           ,
    SIG_SAVE                    ,
    MAX_SIG_VP          // MAX NUMBER OF SIGNALS FROM VP ADDRESS       
};

/* ACTION NAVIGATION */
enum {
    NAVIGATION_SETTING_PAGE         = 0,
    NAVIGATION_RETURN                  ,
    NAVIGATION_HOME                    ,
    NAVIGATION_SETTING_TIME            ,
    NAVIGATION_SETTING_WIFI            ,
    NAVIGATION_SETTING_PROGRAM         ,   
    NAVIGATION_CHANGE_MAINPAGE         ,
    NAVIGATION_CHANGE_CASEDATA         ,
    NAVIGATION_INFORMATION             ,
    NAVIGATION_LOGGING                 ,
    NAVIGATION_MODIFY_PROGRAM          ,
    MAX_VALUE_SIG_NAVIGATE             
};

/* ACTION SELECTION BUTTON */
enum {
    ON_OFF_DEV                  = 0,
    SELECT_PROGRAM                 ,
    SElECT_MODIFY_PROGRAM          ,
    MAX_VALUE_SIG_SELECTION    
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
    VALUE_SAVE_INFORMATION = 0,
    MAX_VALUE_SAVE_INFORMATION
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
    ICON_LOGIC_LOW        = 0,
    ICON_LOGIC_HIGH          ,
    ICON_CLOCK_LOW           ,
    ICON_CLOCK_HIGH          ,
    ICON_LOGIC_NONE 
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

#pragma region ENUM DATA EVENT BUTTON 
enum {
    DATA_PREV_MAINPAGE  = 0, 
    DATA_NEXT_MAINPAGE  
};

enum {
    DATA_PREV_CASE  = 0,
    DATA_NEXT_CASE  
};
#pragma endregion ENUM DATA EVENT BUTTON 

typedef enum {
    Status_ERROR         = 0,
    Status_SUCCESS          
} Return_Status;

typedef struct  {
    uint8_t event;      // create by VP 
    uint8_t action;     // low byte code HMI in VP address
    uint8_t data;       // high byte code HMI in VP address
    uint16_t keyvalue;  // the full key value in VP address
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
    char *num_IC;         // The number IC per testing program
    uint8_t num_case;     // The number test case
    uint8_t num_pin;      // The number pin IC

    char **data_pin;   // Array description of pin IC test
    char **data_test;  // Array 2d data testing
} Program_Test_t;

typedef struct {
    uint8_t curr_case;
    uint8_t curr_PageMain;          // The current main page
    uint8_t selected_Program_Index; // index selected program to show data in main page
    bool    state;
    
    char *NameIC_Tester;
    char *config_pin;                   // Array control icon pin 
    char **data_result;                 // Array control icon result
    uint8_t array[MAX_PROGRAM_TEST];    // Array control icon selected program
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
    uint8_t modify_program_index; // index of program is modified
    bool Status_Dev_1,Status_Dev_2,Status_Dev_3;
} Screen_t;

typedef struct  {
    uint16_t VP_address;
    uint8_t index;
} VP_item;

// The string corresponds to the VP requirement
typedef struct {
    uint16_t VP_address;
    char *String_Name;
    char *String_Unit;
} VP_String; 

typedef void (*handler_func)(Screen_t *const screen_obj, screen_event_t *const screen_event);

/* Function Prototype */

uint8_t Index_VP(uint16_t vp, VP_item* VP_array, uint8_t Size_array);
void Screen_begin(UART_HandleTypeDef * UART_Screen);
void Screen_RX_data(uint8_t *RX_Buffer);
void Screen_RX_Callback(uint16_t Vpaddress, uint8_t lowByte, uint8_t highByte);

/* Callback VP ADDRESS FUNCTION */
void Navigation_setting_page(Screen_t *const screen_obj, screen_event_t *const screen_event);
void Navigation_home_page(Screen_t *const screen_obj, screen_event_t *const screen_event);

#endif /* __SCREEN_H__ */   
