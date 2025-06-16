#ifndef __SCREEN_H__
#define __SCREEN_H__

#include <stdlib.h>
#include <stdio.h>

#include "../Core/Inc/main.h"
#include "../lib/DWIN_HMI.h"

#include "app_main.h"

#define MAX_VALUE_OF_SIG 20             // Check again !!!
#define MAX_DATA_KEYBOARD_INPUT 50

#define MAX_PROGRAM_NAME_SIZE   20
#define MAX_IC_NAME_SIZE        20     
#define MAX_IC_NUM_SIZE         2   

#define MAX_NAME_WIFI_SIZE      30
#define MAX_PASSWORD_WIFI_SIZE  30

#define MACHINE_PASSWORD             "1111"    /* Machine Password */
#define INCORRECT_PASSWORD_TEXT      "Incorrect password"

#define DWINPAGE_MAIN                   101
#define DWINPAGE_MAIN_FINISH            100
#define DWINPAGE_MAIN_DETAIL            102
#define DWINPAGE_MAIN_PULSE             103
#define DWINPAGE_MAIN_DETAIL_14PIN      104 
#define DWINPAGE_SETTING                116
#define DWINPAGE_SETTING_PROGRAM        107   
#define DWINPAGE_MODIFY_PROGRAM         108
#define DWINPAGE_NUM_KEYBOARD           109
#define DWINPAGE_PASSWORD               110
#define DWINPAGE_INFORMATION            111
#define DWINPAGE_LOGGING                112
#define DWINPAGE_SETTING_WIFI           117
#define DWINPAGE_SETTING_TIME           118
#define DWINPAGE_WARNING                119
#define DWINPAGE_FULL_KEYBOARD          120

#define VP_Pulse_graph                  0x1000
#define VP_vertical_line                0x2000 // 0x3C00

#define VP_Navigation_button            0x5000
#define VP_Selection_button             0x5100
#define VP_Program_Run                  0x5200
#define VP_Num_Keyboard                 0x5300
#define VP_Full_Keyboard                0x5380
#define VP_Save_button                  0x5400
#define VP_Enter_button                 0x5500
#define VP_Keyboard                     0xFF00

#define SP_Current_Case                 0x8000 //f800

#define VP_Hour                         0x9000
#define VP_Minute                       0x9010
#define VP_Day                          0x9020
#define VP_Month                        0x9030
#define VP_Year                         0x9040
#define VP_Name_IC                      0x9100
#define VP_Description_IC               0x9150
#define VP_Num_IC_Test                  0x9200
#define VP_Program_Name_1               0x9228
#define VP_Program_Name_2               0x9250
#define VP_Program_Name_3               0x9278
#define VP_Program_Name_4               0x92A0
#define VP_Name_Tester                  0x9350
#define VP_Show_CurrentCase             0x9380
#define VP_Name_Pin                     0x9400 // -> 94F0
#define VP_Modify_Program_Name          0x9500
#define VP_Modify_IC_Name               0x9520
#define VP_Modify_IC_Num                0x9540
#define VP_Password                     0x9560
#define VP_Warning_Password             0x9580 // 20 
#define VP_Name_Wifi                    0x95A0 // 30
#define VP_Password_Wifi                0x95D0 // 30
#define VP_Text_Short_Circuit           0x9600
#define VP_Text_Test_Function           0x9700 // 50
#define VP_Text_Pin_Pulse               0x9750 //- > 0x9800
#define VP_Time_Hour                    0x9800 
#define VP_Time_Day                     0x9820 // 40
#define VP_Data_Log                     0x9900 // -> 0x9B50
#define VP_INFO                         0x9C00
#define VP_Warning                      0x9F00
#define VP_Warning_setting_time         0xA000 //-> 50
#define VP_Warning_modify_program       0xA050 // -> 100

#define VP_ShowString_Keyboard          0xF000
#define VP_ShowUnit_Keyboard            0xF050
#define VP_ShowType_Keyboard            0xF080
#define VP_ShowWarning_Keyboard         0xF100
    
#define VP_ICON_ON_OFF                  0x4000
#define VP_ICON_DIRECTION_PIN           0x4050 // -> 0x405F 
#define VP_ICON_SELECT_PROGRAM          0x4100 
#define VP_ICON_RESULT                  0x4150
#define VP_ICON_CAPLOCK                 0x4200
#define VP_ICON_WIFI                    0x4250
#define VP_ICON_CLOCK                   0x4300


enum {
    SIG_NAVIGATION           = 0,
    SIG_SELECTION               ,
    SIG_KEYBOARD                ,
    SIG_NUM_KEYBOARD            ,
    SIG_FULL_KEYBOARD           ,
    SIG_SAVE                    ,
    SIG_ENTER                   ,
    MAX_SIG_VP          // MAX NUMBER OF SIGNALS FROM VP ADDRESS       
};

/* ================================ ACTION VALUE ================================ */

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
    NAVIGATION_DETAIL_MAINPAGE         ,
    NAVIGATION_GRAPH_MAINPAGE          ,
    NAVIGATION_CHANGE_CASE_PULSE       ,
    NAVIGATION_FINISH_REVIEW           ,
    MAX_VALUE_SIG_NAVIGATE             
};

/* ACTION SELECTION BUTTON */
enum {
    ON_OFF_DEV                  = 0,
    SELECT_PROGRAM                 ,
    SElECT_MODIFY_PROGRAM          ,
    MAX_VALUE_SIG_SELECTION    
};

/* ACTION KEYBOARD */
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
    ENTER_PASSWORD     = 0,
    ENTER_NUM_KEYBOARD    ,
    ENTER_WIFI            ,
    ENTER_TIME            ,
    MAX_VALUE_SIG_ENTER
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
    ICON_LOGIC_NONE          ,
    ICON_DOWN_CLOCK       = 0,
    ICON_UP_CLOCK            ,
    ICON_CLOCK_NONE                 
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

enum {
    WIFI_ON     = 0,
    WIFI_OFF       
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
    char String[MAX_DATA_KEYBOARD_INPUT]; // the string input 
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
    char *Name_Program_temp;
    char *Name_IC;        // Name IC test
    char *Name_IC_temp;
    char *num_IC;         // The number IC per testing program
    char *num_IC_temp;
    char *Description_IC; // Descript ion IC test
    uint8_t num_case;     // The number test case
    uint8_t num_pin;      // The number pin IC

    char *data_pin;   // Array description of pin IC test
    uint16_t data_pin_len;
    char *data_test;  // Array data testing
    uint16_t data_test_len;
} Program_Test_t;

typedef struct {
    uint8_t curr_case;
    uint8_t curr_PageMain;          // The current main page
    uint8_t curr_num_ic;
    uint8_t selected_Program_Index; // index selected program to show data in main page
    bool    state;                  /* Status of device */
    
    char NameIC_Tester[15];
    char *config_pin;                   // Array control icon pin 
    char *data_result;                 // Array control icon result
    char *icon_result;
    char *data_short_circuit;          // array data result test short circuit
    char isShort;
    char *data_result_case;
    char *data_clock;                   // data pulse 
    char result_text[250];
    uint8_t array[MAX_PROGRAM_TEST];    // Array control icon selected program
} Device_t;

typedef struct {
    char Day[3];
    char Month[3];
    char Year[3];
    char Hour[3];
    char Minute[3];
} Time_t;

typedef struct {
    char Name_Wifi[MAX_NAME_WIFI_SIZE];
    char Password_Wifi[MAX_PASSWORD_WIFI_SIZE];
} Wifi_t;

typedef struct  {
    Dwin_t Screen_DWIN;  // inherit from dwin object

    Keyboard_t Screen_keyboard;
    Screen_condition_t *Screen_condition; // The array store condition data input keyboard   
    Device_t IC_Testerx[MAX_DEVICE];
    Program_Test_t Program_Testx[MAX_PROGRAM_TEST];
    Wifi_t Wifi_setting;
    Time_t Time_setting;
    void *p_handler_table;
    uint8_t pre_page;      // previous page 
    uint8_t curr_device;   // Device is displaying 
    uint8_t modify_program_index; // index of program is modified
    uint8_t page_setting;  // Selected setting page 
    bool Ishome;
} Screen_t;

typedef struct  {
    uint16_t VP_address;
    uint8_t index;
} VP_item;

// The string corresponds to the VP requirement
typedef struct {
    char String_Name[15];
    char String_Unit[15];
} VP_String; 

typedef void (*handler_func)(Screen_t *const screen_obj, screen_event_t *const screen_event);

extern Screen_t _Screen;

/* Function Prototype */

uint8_t Index_VP(uint16_t vp, VP_item* VP_array, uint8_t Size_array);
void Screen_begin(UART_HandleTypeDef * UART_Screen);
void Screen_RX_data(uint8_t *RX_Buffer);
void Screen_RX_Callback(uint16_t Vpaddress, uint8_t lowByte, uint8_t highByte);

/* Callback VP ADDRESS FUNCTION */
void Navigation_setting_page(Screen_t *const screen_obj, screen_event_t *const screen_event);
void Navigation_home_page(Screen_t *const screen_obj, screen_event_t *const screen_event);

void get_data_testing_finish(Screen_t *const screen_obj,uint8_t device, uint8_t *result_array);
void show_main_page(Screen_t *const screen_obj, uint8_t PageMain, uint8_t curr_program);

void warning_page(Screen_t *const screen_obj,uint32_t id_slave);

void updata_data_program_from_app(Screen_t *const screen_obj,char *data,uint16_t len);
#endif /* __SCREEN_H__ */   
