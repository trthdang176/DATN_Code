#include "Screen.h"

#define SIZE_LOOKUP_VP_SIG (sizeof(lookup_VP_SIG)/sizeof(VP_item))
#define SIZE_LOOKUP_VP_STRING (sizeof(lookup_VP_STRING)/sizeof(VP_String))

Screen_t _Screen;  // screen object

/* Private prototype function */
void Screen_init_handler_table(Screen_t *const obj_screen);
void Screen_excute_RX_function(Screen_t *const obj_screen,screen_event_t *const screen_e);

void Screen_Init_Variable(Screen_t *const obj_screen);
void Screen_ShowData_Mainpage(Screen_t *const screen_obj, uint8_t index_program);
void Screen_SetInfo_Pin(Screen_t *const screen_obj, char **array_data);

void Screen_GetIcon_Pin(Screen_t *const screen_obj, char **data_pin);
void Screen_GetIcon_Result(Screen_t *const screen_obj, char **result);

Return_Status Screen_CheckInput_Keyboard(Screen_t *const obj_screen);


/* Array store the VP address signal */
/* Note: The VP address must be increased to find the index */
VP_item lookup_VP_SIG[] = {
    {VP_Navigation_button,SIG_NAVIGATION},   // 5000
    {VP_Selection_button,SIG_SELECTION},     // 5100
    {VP_Num_Keyboard,SIG_NUM_KEYBOARD},      // 5300
    {VP_Full_Keyboard,SIG_FULL_KEYBOARD},    // 5380
    {VP_Save_button,SIG_SAVE},               // 5400
    {VP_Enter_button,SIG_ENTER},             // 5500
    {VP_Keyboard,SIG_KEYBOARD}               // FF00
};

/* The array store the VP address to check data input */
VP_item lookup_VP_check_datakeyboard[] = {
    {VP_Day,CONDITION_DAY},
    {VP_Month,CONDITION_MONTH},
    {VP_Year,CONDITION_YEAR},
    {VP_Minute,CONDITION_MINUTE},
    {VP_Hour,CONDITION_HOUR}
};

VP_String lookup_VP_STRING[] = {
    {VP_Num_IC_Test,"NUMBER OF IC","PSC"}
};

Screen_condition_t condition_array[MAX_CONDITION] = {
    [CONDITION_DAY]      = {.max_value = 31, .min_value = 1},
    [CONDITION_MONTH]    = {.max_value = 12, .min_value = 1},
    [CONDITION_YEAR]     = {.max_value = 99, .min_value = 1},
    [CONDITION_HOUR]     = {.max_value = 23, .min_value = 0},
    [CONDITION_MINUTE]   = {.max_value = 59, .min_value = 0}
};

void Screen_begin(UART_HandleTypeDef * UART_Screen) {
    // Init _DWIN UART
	_Screen.Screen_DWIN.pUart = UART_Screen;

    // add receive function callback
	Screen_init_handler_table(&_Screen);
    DWIN_SetCallback((Dwin_t *)&_Screen,(pListenDWIN)Screen_RX_Callback);

    Screen_Init_Variable(&_Screen);

    // Init screen keyboard     
    _Screen.Screen_keyboard.VP_Text = 0xFFFF;
    _Screen.Screen_keyboard.Index_String = 0;
    _Screen.Screen_keyboard.Caplock = 0;
    
    DWIN_SetPage((Dwin_t *)&_Screen,DWINPAGE_MAIN);  // Go to first page when start

    _Screen.pre_page = 106;

    char* data_pin[] = {
    "VEE", "INH", "Z", "Y", "X", "VSS", "B", "C",
        "A", "VCC", "X0", "X1", "Y0", "Y1", "Z0", "Z1"
    };

    Screen_SetInfo_Pin(&_Screen,data_pin);

    char *data_test[] = {
        "11L1100G0001011V",
        "11L1100G0011101V",
        "11L1100G0101110V",
        "11L1100G0110111V",
        "01L1100G1001111V",
        "11L1000G1011111V",
        "10L1100G1101111V",
        "11L0100G1111111V"
    };

    Screen_GetIcon_Pin(&_Screen,data_test);
    DWIN_SetArray_Icon(&_Screen,VP_ICON_DIRECTION_PIN,_Screen.IC_Testerx[PROGRAM_TEST1].config_pin,16);
    Screen_GetIcon_Result(&_Screen,data_test);
    DWIN_SetArray_Icon(&_Screen,VP_ICON_RESULT,_Screen.IC_Testerx[PROGRAM_TEST1].data_result[0],16);
    // DWIN_SetVariable_Icon(&_Screen,0x4150,1);
    // DWIN_SetVariable_Icon(&_Screen,0x4151,1);
    // DWIN_SetVariable_Icon(&_Screen,0x4152,1);


    // uint8_t data_clock[] = {1,1,0,1,0,1};
    // DWIN_Create_Basic_line((Dwin_t *)&_Screen,0x1800,25,200,data_clock,sizeof(data_clock));
    // HAL_Delay(200);
    // uint8_t data_clock2[] = {1,0,1,0,1,0,1,0,1,0};
    // DWIN_Create_Basic_line((Dwin_t *)&_Screen,0x1800,25,200,data_clock2,sizeof(data_clock2));
    // HAL_Delay(200);
    // uint8_t data_clock3[] = {0,1,0,1,0,1,0,1,0,1,1,1};
    // DWIN_Create_Basic_line((Dwin_t *)&_Screen,0x1800,25,200,data_clock3,sizeof(data_clock3));
    uint8_t data[15] = {1,1,0,0,1,0,0,1,1,0,0,0,1,1,0};
    // DWIN_Create_Basic_line((Dwin_t *)&_Screen,0x1000,14,140,data,15);
    // DWIN_SetWidth_Basic_line((Dwin_t *)&_Screen,0x11D0,2);
    // uint8_t data_2[15] = {1,1,0,0,1,0,0,1,1,0,0,0,1,1,0};
    // DWIN_Create_Basic_line((Dwin_t *)&_Screen,0x1200,14,190,data,15);
    // DWIN_SetWidth_Basic_line((Dwin_t *)&_Screen,0x13D0,2);
    // uint8_t data_2[15] = {1,1,0,0,1,0,0,1,1,0,0,0,1,1,0};
    // DWIN_Create_Basic_line((Dwin_t *)&_Screen,0x1400,14,240,data,15);
    // DWIN_SetWidth_Basic_line((Dwin_t *)&_Screen,0x15D0,2);
    // DWIN_Create_Basic_line((Dwin_t *)&_Screen,0x1600,14,290,data,15);
    // DWIN_SetWidth_Basic_line((Dwin_t *)&_Screen,0x17D0,2);
    // DWIN_Create_Basic_line((Dwin_t *)&_Screen,0x1800,14,340,data,15);
    // DWIN_SetWidth_Basic_line((Dwin_t *)&_Screen,0x19D0,2);
    // DWIN_Create_Basic_line((Dwin_t *)&_Screen,0x1A00,14,390,data,15);
    // DWIN_SetWidth_Basic_line((Dwin_t *)&_Screen,0x1BD0,2);
    // DWIN_Create_Basic_line((Dwin_t *)&_Screen,0x1C00,14,440,data,15);
    // DWIN_SetWidth_Basic_line((Dwin_t *)&_Screen,0x1DD0,2);

    // uint8_t current = 0;

    // for(uint8_t i = 0; i < 8; i++) {
    //     uint16_t Address_show = 0x1000;
    //     uint16_t X_Coordinate = 14;
    //     uint16_t Y_Coordinate = 142;
    //     DWIN_Create_Basic_line((Dwin_t *)&_Screen,Address_show,X_Coordinate,Y_Coordinate,data,15);
    //     Address_show = Address_show + 0x200;
    //     Y_Coordinate = Y_Coordinate + 30;
    //     HAL_Delay(500);
    // }
}

// USED IN RX UART CALLBACK FUNCTION WHEN RECEIVE DATA
void Screen_RX_data(uint8_t *RX_Buffer) {
    DWIN_Listen((Dwin_t *)&_Screen, RX_Buffer);
}

void Screen_RX_Callback(uint16_t Vpaddress, uint8_t lowByte, uint8_t highByte) { 
    screen_event_t screen_event;
    // If the VP is FFFF -> skip
    if (Vpaddress != 0xFFFF) {
        screen_event.event = Index_VP(Vpaddress,lookup_VP_SIG,SIZE_LOOKUP_VP_SIG);
        screen_event.action = lowByte;
        screen_event.data = highByte;
        screen_event.keyvalue = (uint16_t)((highByte<<8) | (lowByte&0xFF));

        // call to respective function
        Screen_excute_RX_function(&_Screen,&screen_event);
    }
}


#pragma region CALLBACK FUNCTION VP ADRRESS 

void Navigation_setting_page(Screen_t *const screen_obj, screen_event_t *const screen_event) {
    DWIN_SetPage((Dwin_t *)screen_obj,DWINPAGE_SETTING);
}

void Navigation_home_page(Screen_t *const screen_obj, screen_event_t *const screen_event) {
    uint8_t current_program = screen_obj->IC_Testerx[screen_obj->curr_device].selected_Program_Index;
    UNUSED(current_program);
    // Show data

    // Switch page
    DWIN_SetPage((Dwin_t *)screen_obj,screen_obj->IC_Testerx[screen_obj->curr_device].curr_PageMain);
}   

void Navigation_setting_program(Screen_t *const screen_obj, screen_event_t *const screen_event) {

    // Switch page
    // DWIN_SetPage((Dwin_t *)screen_obj,DWINPAGE_SETTING_PROGRAM);
    /* Get the setting page */
    screen_obj->page_setting = DWINPAGE_SETTING_PROGRAM;
    // Switch page password 
    DWIN_SetText((Dwin_t *)screen_obj,VP_Password,"",strlen(""));
    DWIN_SetPage((Dwin_t *)screen_obj,DWINPAGE_PASSWORD);
}

void Navigation_setting_time(Screen_t *const screen_obj, screen_event_t *const screen_event) {
    
    // Switch page
    DWIN_SetPage((Dwin_t *)screen_obj,DWINPAGE_SETTING_TIME);
}

void Navigation_setting_wifi(Screen_t *const screen_obj, screen_event_t *const screen_event) {
    /* show data */
    DWIN_SetText((Dwin_t *)screen_obj,VP_Name_Wifi,screen_obj->Wifi_setting.Name_Wifi,strlen(screen_obj->Wifi_setting.Name_Wifi));
    DWIN_SetText((Dwin_t *)screen_obj,VP_Password_Wifi,screen_obj->Wifi_setting.Password_Wifi,strlen(screen_obj->Wifi_setting.Password_Wifi));

    DWIN_SetPage((Dwin_t *)screen_obj,DWINPAGE_SETTING_WIFI);
}

void Navigation_modify_program(Screen_t *const screen_obj, screen_event_t *const screen_event) {
    // Show data in screen
    DWIN_SetText((Dwin_t *)screen_obj,VP_Modify_Program_Name,screen_obj->Program_Testx[screen_obj->modify_program_index].Name_Program,strlen(screen_obj->Program_Testx[screen_obj->modify_program_index].Name_Program));
    DWIN_SetText((Dwin_t *)screen_obj,VP_Modify_IC_Name,screen_obj->Program_Testx[screen_obj->modify_program_index].Name_IC,strlen(screen_obj->Program_Testx[screen_obj->modify_program_index].Name_IC));
    DWIN_SetText((Dwin_t *)screen_obj,VP_Modify_IC_Num,screen_obj->Program_Testx[screen_obj->modify_program_index].num_IC,strlen(screen_obj->Program_Testx[screen_obj->modify_program_index].num_IC));
    // Switch page
    DWIN_SetPage((Dwin_t *)screen_obj,DWINPAGE_MODIFY_PROGRAM);
}

void Navigation_Change_MainPage(Screen_t *const screen_obj, screen_event_t *const screen_event) {
    // Change current device
    if (screen_event->data == DATA_NEXT_MAINPAGE) {
        screen_obj->curr_device++;
        if (screen_obj->curr_device == MAX_DEVICE) {
            screen_obj->curr_device = DEVICE_1;
        }
    } else if (screen_event->data == DATA_PREV_MAINPAGE) {
        screen_obj->curr_device--;
        if (screen_obj->curr_device == 255 ) {
            screen_obj->curr_device = DEVICE_3;
        }
    }
    uint8_t curr_program = screen_obj->IC_Testerx[screen_obj->curr_device].selected_Program_Index;
    /* Show data with correspond device index */
    switch (screen_obj->IC_Testerx[screen_obj->curr_device].curr_PageMain) {
        case DWINPAGE_MAIN : {
            /* Show info */
            DWIN_SetText((Dwin_t *)screen_obj,VP_Name_Tester,screen_obj->IC_Testerx[screen_obj->curr_device].NameIC_Tester,strlen(screen_obj->IC_Testerx[screen_obj->curr_device].NameIC_Tester));
            DWIN_SetVariable_Icon((Dwin_t *)screen_obj,VP_ICON_ON_OFF,screen_obj->IC_Testerx[screen_obj->curr_device].state);
            Screen_ShowData_Mainpage(screen_obj,curr_program);
        } break;
        case DWINPAGE_MAIN_FINISH: {

        } break;
        case DWINPAGE_MAIN_DETAIL: {

        } break;
        case DWINPAGE_MAIN_GRAPH: {

        } break;
    }
}

void Navigation_Change_CaseTest(Screen_t *const screen_obj, screen_event_t *const screen_event) {
    uint8_t num_case = screen_obj->Program_Testx[screen_obj->IC_Testerx[screen_obj->curr_device].selected_Program_Index].num_case;
    uint8_t selectedProgram = screen_obj->IC_Testerx[screen_obj->curr_device].selected_Program_Index;
    uint8_t num_pin = screen_obj->Program_Testx[selectedProgram].num_pin;
    char ShowCase_String[30];
    // Change current case
    if (screen_event->data == DATA_PREV_CASE) {
        if (screen_obj->IC_Testerx[screen_obj->curr_device].curr_case > 0) {
            screen_obj->IC_Testerx[screen_obj->curr_device].curr_case--;
        }
    } else if (screen_event->data == DATA_NEXT_CASE) {
        if (screen_obj->IC_Testerx[screen_obj->curr_device].curr_case < (num_case - 1)) {
            screen_obj->IC_Testerx[screen_obj->curr_device].curr_case++;
        }
    }

    /* Show data */
    snprintf(ShowCase_String,sizeof(ShowCase_String),"%d OF %d",screen_obj->IC_Testerx[screen_obj->curr_device].curr_case + 1,num_case);
    DWIN_SetText((Dwin_t *)screen_obj,VP_Show_CurrentCase,ShowCase_String,strlen(ShowCase_String));
    DWIN_SetArray_Icon((Dwin_t *)screen_obj,VP_ICON_RESULT,_Screen.IC_Testerx[selectedProgram].data_result[screen_obj->IC_Testerx[screen_obj->curr_device].curr_case],num_pin);
}

void Navigation_Next_PrevPage(Screen_t *const screen_obj, screen_event_t *const screen_event) {

}

void ON_OFF_Button(Screen_t *const screen_obj, screen_event_t *const screen_event) {
    // do sth
    screen_obj->IC_Testerx[screen_obj->curr_device].state = !screen_obj->IC_Testerx[screen_obj->curr_device].state;
    // change icon
    // DWIN_SetVariable_Icon((Dwin_t *)screen_obj,VP_ON_OFF_Icon_1,ON_ICON);
    //    HAL_Delay(1000);
    DWIN_SetVariable_Icon((Dwin_t *)screen_obj,VP_ICON_ON_OFF,screen_obj->IC_Testerx[screen_obj->curr_device].state);
    
    OS_task_post_event(AO_task_can_bus,SEND_DATA_CAN_BUS,(uint8_t *)0,0);
}

void select_running_program(Screen_t *const screen_obj, screen_event_t *const screen_event) {
    uint8_t programID = screen_event->data; // Get new index program

    /* Show data correspond with new index program */
    Screen_ShowData_Mainpage(screen_obj,programID);
}

void select_modify_program(Screen_t *const screen_obj, screen_event_t *const screen_event) {
    // Get the index modify program 
    uint8_t index_program = screen_event->data;
    screen_obj->modify_program_index = index_program;
    
    uint8_t size_Name_IC = strlen("IC Name: ") + strlen((char *)screen_obj->Program_Testx[index_program].Name_IC);
    uint8_t size_Num_IC = strlen("Number of ICs: ") + strlen((char *)screen_obj->Program_Testx[index_program].num_IC);

    char *Text_Name_IC = malloc(size_Name_IC + 1); // Null 
    char *Text_Num_IC = malloc(size_Num_IC + 1);

    strcpy(Text_Name_IC,"IC Name: ");
    strcat(Text_Name_IC,(char *)screen_obj->Program_Testx[index_program].Name_IC);
    strcpy(Text_Num_IC,"Number of ICs: ");
    strcat(Text_Num_IC,(char *)screen_obj->Program_Testx[index_program].num_IC);

    // Show data in screen
    DWIN_SetText((Dwin_t *)screen_obj,VP_Modify_IC_Name,Text_Name_IC,size_Name_IC);
    DWIN_SetText((Dwin_t *)screen_obj,VP_Modify_IC_Num,Text_Num_IC,size_Num_IC);
}

void Navigation_num_keyboard(Screen_t *const screen_obj, screen_event_t *const screen_event) {
    //char TypeData_String[20] = {0};
    //char UnitData_String[20] = {0};
    // Switch screen keyboard will excute in screen

    // get the datavalue -> the VP will show string input
    screen_obj->Screen_keyboard.VP_Text = screen_event->keyvalue;

    // Reset the string
    screen_obj->Screen_keyboard.Index_String = 0;
    memset(screen_obj->Screen_keyboard.String,0,sizeof(screen_obj->Screen_keyboard.String));
    /* Show text num keyboard */
    DWIN_SetText((Dwin_t *)screen_obj,VP_ShowString_Keyboard,screen_obj->Screen_keyboard.String,sizeof(screen_obj->Screen_keyboard.String));
    // DWIN_SetText((Dwin_t *)screen_obj,VP_ShowType_Keyboard)

    /* Switch page */
    DWIN_SetPage((Dwin_t *)screen_obj,DWINPAGE_NUM_KEYBOARD);
}

void Navigation_full_keyboard(Screen_t *const screen_obj, screen_event_t *const screen_event) {
    // get the datavalue -> the VP will show string input
    screen_obj->Screen_keyboard.VP_Text = screen_event->keyvalue;

    // Reset the string
    screen_obj->Screen_keyboard.Index_String = 0;
    memset(screen_obj->Screen_keyboard.String,0,sizeof(screen_obj->Screen_keyboard.String));
    /* Show text num keyboard */
    DWIN_SetText((Dwin_t *)screen_obj,VP_ShowString_Keyboard,screen_obj->Screen_keyboard.String,sizeof(screen_obj->Screen_keyboard.String));

    /* Switch page */
    DWIN_SetPage((Dwin_t *)screen_obj,DWINPAGE_FULL_KEYBOARD);
}

void Save_Information(Screen_t *const screen_obj, screen_event_t *const screen_event) {

}

void Enter_password(Screen_t *const screen_obj, screen_event_t *const screen_event) {
    /* Get the password input */
    uint8_t len = screen_obj->Screen_keyboard.Index_String + 1;
    char input_pass[len];
    strcpy(input_pass,screen_obj->Screen_keyboard.String);
    /* Check password */
    if (strcmp(input_pass,MACHINE_PASSWORD) == 0) { /* Correct password */
        /* Goto setting page */
        DWIN_SetPage((Dwin_t *)screen_obj,screen_obj->page_setting);
    } else {
        /* Show text */
        DWIN_SetText((Dwin_t *)screen_obj,VP_Warning_Password,INCORRECT_PASSWORD_TEXT,strlen(INCORRECT_PASSWORD_TEXT));
    }
}

void Enter_num_keyboard(Screen_t *const screen_obj, screen_event_t *const screen_event) {
    uint16_t VP_show = screen_obj->Screen_keyboard.VP_Text;
    
    /* Show text to VP */
    DWIN_SetText((Dwin_t *)screen_obj,VP_show,screen_obj->Screen_keyboard.String,screen_obj->Screen_keyboard.Index_String);

    /* Return page base on VP address */
    switch (VP_show) {
        case VP_Password : {
            DWIN_ClearText((Dwin_t *)screen_obj,VP_Warning_Password); /* Clear warning text if any */
            DWIN_SetPage((Dwin_t *)screen_obj,DWINPAGE_PASSWORD);
        } break;
        case VP_Modify_Program_Name : 
        case VP_Modify_IC_Name      : 
        case VP_Modify_IC_Num       :{
            DWIN_SetPage((Dwin_t *)screen_obj,DWINPAGE_MODIFY_PROGRAM);
        } break;
        case VP_Name_Wifi :
        case VP_Password_Wifi : {
            DWIN_SetPage((Dwin_t *)screen_obj,DWINPAGE_SETTING_WIFI);
        } break;
        default: break;
    }
}

void Keyboard(Screen_t *const screen_obj, screen_event_t *const screen_event) {
    
    /* The other Functions button */
    if (screen_event->data == 0xF3) { // Delete button
        if (screen_obj->Screen_keyboard.Index_String != 0) {
            --screen_obj->Screen_keyboard.Index_String;
        }
    } else if (screen_event->data == 0xF4) { // Caplock button
        /* Toggle status Caplock keyboard */
        screen_obj->Screen_keyboard.Caplock = !screen_obj->Screen_keyboard.Caplock; 
        DWIN_SetVariable_Icon((Dwin_t *)screen_obj,VP_ICON_CAPLOCK,screen_obj->Screen_keyboard.Caplock);
    } else if (screen_event->data == 0xF2) { // Space button
        // screen_obj->Screen_keyboard.String[screen_obj->Screen_keyboard.Index_String++] = 32;
    } else if (screen_event->data == 0xF1) { // Enter button
        // Check if the data is valid
        if (Screen_CheckInput_Keyboard(screen_obj) == Status_SUCCESS) {
            // Show data input to VP 
            DWIN_SetText((Dwin_t *)screen_obj,screen_obj->Screen_keyboard.VP_Text,screen_obj->Screen_keyboard.String,screen_obj->Screen_keyboard.Index_String);
            DWIN_SetPage((Dwin_t *)screen_obj,screen_obj->pre_page); // switch to previous page
        }
    }
    /* the data value is the character input -> add to string */
    else if (screen_event->data >= 0x41 && screen_event->data <= 0x5A) { // letter need check Caplock
        if (screen_obj->Screen_keyboard.Caplock == false) { // Caplock is on
            screen_obj->Screen_keyboard.String[screen_obj->Screen_keyboard.Index_String++] = screen_event->data + 0x20;
        } else {
            screen_obj->Screen_keyboard.String[screen_obj->Screen_keyboard.Index_String++] = screen_event->data;
        }
    } else {
        screen_obj->Screen_keyboard.String[screen_obj->Screen_keyboard.Index_String++] = screen_event->data;
    }

    /* Show string input keyboard */
    DWIN_SetText((Dwin_t *)screen_obj,VP_ShowString_Keyboard,screen_obj->Screen_keyboard.String,screen_obj->Screen_keyboard.Index_String);
}

#pragma endregion CALLBACK FUNCTION VP ADRRESS 

#pragma region FUNCTION EXCUTE SCREEN

// binary search index of VP address
uint8_t Index_VP(uint16_t vp, VP_item* VP_array, uint8_t Size_array) {
    int left = 0;
    int right = Size_array - 1;
    int mid;

    while (left <= right) {
        mid = (left + right) / 2;
        if (VP_array[mid].VP_address == vp) {
            return VP_array[mid].index;  // Found the vp
        } else if (VP_array[mid].VP_address > vp) {
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }

    return 255;  // VP not found
}

void Screen_init_handler_table(Screen_t *const obj_screen) {
    // table handler the callback function
    static handler_func a_handler_navigation_sig[MAX_VALUE_SIG_NAVIGATE] = {NULL};
    static handler_func a_handler_on_off_sig[MAX_VALUE_SIG_SELECTION] = {NULL};
    static handler_func a_handler_save_sig[MAX_VALUE_SAVE_INFORMATION] = {NULL};
    static handler_func a_handler_enter_sig[MAX_VALUE_SIG_ENTER] = {NULL};
    static handler_func a_handler_keyboard_sig[MAX_VALUE_KEYBOARD] = {NULL};
    static handler_func a_handler_num_keyboard[MAX_VALUE_NAVIGATION_KEYBOARD] = {NULL};
    static handler_func a_handler_full_keyboard[MAX_VALUE_NAVIGATION_KEYBOARD] = {NULL};

    static handler_func *handler_function_table[MAX_SIG_VP] = { a_handler_navigation_sig,
                                                                a_handler_on_off_sig ,
                                                                a_handler_keyboard_sig,
                                                                a_handler_num_keyboard,
                                                                a_handler_full_keyboard,
                                                                a_handler_save_sig,
                                                                a_handler_enter_sig };

    // Assign the callback function to handler table
    handler_function_table[SIG_NAVIGATION][NAVIGATION_SETTING_PAGE]         = Navigation_setting_page ;
    handler_function_table[SIG_NAVIGATION][NAVIGATION_HOME]                 = Navigation_home_page;
    handler_function_table[SIG_NAVIGATION][NAVIGATION_SETTING_PROGRAM]      = Navigation_setting_program;
    handler_function_table[SIG_NAVIGATION][NAVIGATION_SETTING_TIME]         = Navigation_setting_time;
    handler_function_table[SIG_NAVIGATION][NAVIGATION_SETTING_WIFI]         = Navigation_setting_wifi;
    handler_function_table[SIG_NAVIGATION][NAVIGATION_MODIFY_PROGRAM]       = Navigation_modify_program;
    handler_function_table[SIG_NAVIGATION][NAVIGATION_CHANGE_MAINPAGE]      = Navigation_Change_MainPage;
    handler_function_table[SIG_NAVIGATION][NAVIGATION_CHANGE_CASEDATA]      = Navigation_Change_CaseTest;

    handler_function_table[SIG_SELECTION][ON_OFF_DEV]                       = ON_OFF_Button;
    handler_function_table[SIG_SELECTION][SELECT_PROGRAM]                   = select_running_program;
    handler_function_table[SIG_SELECTION][SElECT_MODIFY_PROGRAM]            = select_modify_program;

    handler_function_table[SIG_NUM_KEYBOARD][VALUE_NAVIGATION_KEYBOARD]     = Navigation_num_keyboard;
    handler_function_table[SIG_FULL_KEYBOARD][VALUE_NAVIGATION_KEYBOARD]    = Navigation_full_keyboard;

    handler_function_table[SIG_SAVE][VALUE_SAVE_INFORMATION]                = Save_Information;

    handler_function_table[SIG_ENTER][ENTER_PASSWORD]                       = Enter_password;
    handler_function_table[SIG_ENTER][ENTER_NUM_KEYBOARD]                   = Enter_num_keyboard;

    handler_function_table[SIG_KEYBOARD][VALUE_KEBOARD]                     = Keyboard;

    // assign handler table to handler table of object
    obj_screen->p_handler_table = (void *)handler_function_table;
}

void Screen_excute_RX_function(Screen_t *const obj_screen,screen_event_t *const screen_e) {
    // do sth ...


	handler_func **handler_function_table = (handler_func **) obj_screen->p_handler_table;
    // excute the callback function
    if (screen_e->event == SIG_KEYBOARD) {  // ENVENT KEYBOARD
        handler_function_table[SIG_KEYBOARD][VALUE_KEBOARD](obj_screen, screen_e);   
    } else if (screen_e->event == SIG_NUM_KEYBOARD) {  // SWTICH TO NUMBER KEYBOARD
        handler_function_table[SIG_NUM_KEYBOARD][VALUE_NAVIGATION_KEYBOARD](obj_screen, screen_e);  
    } else if (screen_e->event == SIG_FULL_KEYBOARD) {
        handler_function_table[SIG_FULL_KEYBOARD][VALUE_NAVIGATION_KEYBOARD](obj_screen, screen_e);
    } else {
        handler_function_table[screen_e->event][screen_e->action](obj_screen, screen_e);
    }  
}

#pragma endregion FUNCTION EXCUTE SCREEN

#pragma region FUNCTION SCREEN 

Return_Status Screen_CheckInput_Keyboard(Screen_t *const obj_screen) {
    char string[obj_screen->Screen_keyboard.Index_String];
    char warm_string[20] = {0};
    float data_compare;
    // copy string
    memcpy(string,obj_screen->Screen_keyboard.String,obj_screen->Screen_keyboard.Index_String);
    /* compare data with limit */
    data_compare = atof(string);
    if (data_compare > obj_screen->Screen_condition->max_value) {
        // Create warm string
        snprintf(warm_string,sizeof(warm_string),"MAX: %.2f", obj_screen->Screen_condition->max_value);
        // Display warning
        DWIN_SetText((Dwin_t *)obj_screen,VP_ShowWarning_Keyboard,warm_string,sizeof(warm_string));
        return Status_ERROR;
    } else if (data_compare < obj_screen->Screen_condition->min_value) {
        // Create warm string
        snprintf(warm_string,sizeof(warm_string),"MIN: %.2f", obj_screen->Screen_condition->min_value);
        // Display warning
        DWIN_SetText((Dwin_t *)obj_screen,VP_ShowWarning_Keyboard,warm_string,sizeof(warm_string));
        return Status_ERROR;
    } 
    return Status_SUCCESS; 
}

/**
 * @brief Function used to setup icon with data test
 * 
 * @param obj_screen : object screen
 * @param data_pin : array 2d data test
 */
void Screen_GetIcon_Pin(Screen_t *const screen_obj, char **data_pin) {
    uint8_t num_pin = screen_obj->Program_Testx[screen_obj->IC_Testerx[screen_obj->curr_device].selected_Program_Index].num_pin;
    
    uint8_t map_array[128] = {0};
    map_array['G'] = ICON_GND;
    map_array['V'] = ICON_VCC;
    /* Create the array icon data pin */
    screen_obj->IC_Testerx[screen_obj->curr_device].config_pin = malloc(num_pin * sizeof(uint8_t));

    /* Convert data to icon pin config */
    for (uint8_t Pin_x = 0; Pin_x < num_pin; Pin_x++) {
        if (Pin_x >= 8) {
            map_array['0'] = ICON_OUTPUT;
            map_array['1'] = ICON_OUTPUT;
            map_array['L'] = ICON_INPUT;
            map_array['H'] = ICON_INPUT;
        } else {
            map_array['0'] = ICON_INPUT;
            map_array['1'] = ICON_INPUT;
            map_array['L'] = ICON_OUTPUT;
            map_array['H'] = ICON_OUTPUT;
        }
        screen_obj->IC_Testerx[screen_obj->curr_device].config_pin[Pin_x] = map_array[(uint8_t)(data_pin[0][Pin_x])];
    }
}

void Screen_SetIcon_Pin(Screen_t *const obj_screen) {

}

void Screen_GetIcon_Result(Screen_t *const screen_obj, char **result) {
    uint8_t num_pin = screen_obj->Program_Testx[screen_obj->IC_Testerx[screen_obj->curr_device].selected_Program_Index].num_pin;
    uint8_t num_case = screen_obj->Program_Testx[screen_obj->IC_Testerx[screen_obj->curr_device].selected_Program_Index].num_case;
    uint8_t map_array[128] = {0};
    map_array['0'] = ICON_LOGIC_LOW;
    map_array['1'] = ICON_LOGIC_HIGH;
    map_array['L'] = ICON_CLOCK_LOW;
    map_array['H'] = ICON_CLOCK_HIGH;
    map_array['V'] = ICON_LOGIC_NONE;
    map_array['G'] = ICON_LOGIC_NONE;
    /* Create the array icon data pin */
    screen_obj->IC_Testerx[screen_obj->curr_device].data_result = malloc(num_case * sizeof(char *));
    for (uint8_t Case_x = 0; Case_x < num_case; Case_x++) {
        screen_obj->IC_Testerx[screen_obj->curr_device].data_result[Case_x] = malloc(num_pin * sizeof(char));
    }

    /* Convert data to icon pin config */
    for (uint8_t Case_x = 0; Case_x < num_case; Case_x++) {
        for (uint8_t Pin_x = 0; Pin_x < num_pin; Pin_x++) {
        	screen_obj->IC_Testerx[screen_obj->curr_device].data_result[Case_x][Pin_x] = map_array[(uint8_t)(result[Case_x][Pin_x])];
        }
    }
}

void Screen_Init_Variable(Screen_t *const obj_screen) {
    obj_screen->curr_device = DEVICE_1;

    obj_screen->Program_Testx[PROGRAM_TEST1].Name_IC = "74HC4051";
    obj_screen->Program_Testx[PROGRAM_TEST1].Description_IC = "Description IC 74HC4051";
    obj_screen->Program_Testx[PROGRAM_TEST1].num_IC = "5";

    obj_screen->Program_Testx[PROGRAM_TEST2].Name_IC = "74HC4052";
    obj_screen->Program_Testx[PROGRAM_TEST2].Description_IC = "Description IC 74HC4052";
    obj_screen->Program_Testx[PROGRAM_TEST2].num_IC = "10";

    obj_screen->Program_Testx[PROGRAM_TEST3].Name_IC = "74HC4053";
    obj_screen->Program_Testx[PROGRAM_TEST3].Description_IC = "Description IC 74HC4053";
    obj_screen->Program_Testx[PROGRAM_TEST3].num_IC = "15";

    obj_screen->Program_Testx[PROGRAM_TEST4].Name_IC = "74HC4054";
    obj_screen->Program_Testx[PROGRAM_TEST4].Description_IC = "Description IC 74HC4054";
    obj_screen->Program_Testx[PROGRAM_TEST4].num_IC = "20";

    obj_screen->Program_Testx[PROGRAM_TEST1].Name_Program = "Program 1";
    obj_screen->Program_Testx[PROGRAM_TEST2].Name_Program = "Program 2";
    obj_screen->Program_Testx[PROGRAM_TEST3].Name_Program = "Program 3";
    obj_screen->Program_Testx[PROGRAM_TEST4].Name_Program = "Program 4";

    obj_screen->IC_Testerx[DEVICE_1].NameIC_Tester = "IC TESTER 1";
    obj_screen->IC_Testerx[DEVICE_2].NameIC_Tester = "IC TESTER 2";
    obj_screen->IC_Testerx[DEVICE_3].NameIC_Tester = "IC TESTER 3";

    memcpy(obj_screen->Wifi_setting.Name_Wifi,"Wifi TEST 1",strlen("Wifi TEST 1"));
    memcpy(obj_screen->Wifi_setting.Password_Wifi,"PASSword 1234",strlen("PASSword 1234"));

    obj_screen->IC_Testerx[DEVICE_1].curr_PageMain = DWINPAGE_MAIN;
    obj_screen->IC_Testerx[DEVICE_2].curr_PageMain = DWINPAGE_MAIN;
    obj_screen->IC_Testerx[DEVICE_3].curr_PageMain = DWINPAGE_MAIN; 

    obj_screen->IC_Testerx[DEVICE_1].state = false;
    obj_screen->IC_Testerx[DEVICE_2].state = false;
    obj_screen->IC_Testerx[DEVICE_3].state = false;

    DWIN_SetText((Dwin_t *)obj_screen,VP_Program_Name_1,obj_screen->Program_Testx[PROGRAM_TEST1].Name_Program,strlen(obj_screen->Program_Testx[PROGRAM_TEST1].Name_Program));
    DWIN_SetText((Dwin_t *)obj_screen,VP_Program_Name_2,obj_screen->Program_Testx[PROGRAM_TEST2].Name_Program,strlen(obj_screen->Program_Testx[PROGRAM_TEST2].Name_Program));
    DWIN_SetText((Dwin_t *)obj_screen,VP_Program_Name_3,obj_screen->Program_Testx[PROGRAM_TEST3].Name_Program,strlen(obj_screen->Program_Testx[PROGRAM_TEST3].Name_Program));
    DWIN_SetText((Dwin_t *)obj_screen,VP_Program_Name_4,obj_screen->Program_Testx[PROGRAM_TEST4].Name_Program,strlen(obj_screen->Program_Testx[PROGRAM_TEST4].Name_Program));

    DWIN_SetText((Dwin_t *)obj_screen,VP_Name_Tester,obj_screen->IC_Testerx[DEVICE_1].NameIC_Tester,strlen(obj_screen->IC_Testerx[DEVICE_1].NameIC_Tester));

    obj_screen->Program_Testx[PROGRAM_TEST1].num_pin = 16;
    obj_screen->Program_Testx[PROGRAM_TEST1].num_case = 8;

    /* Screen */
    obj_screen->Screen_keyboard.Caplock = false;
    obj_screen->Screen_keyboard.Index_String = 0;

    Screen_ShowData_Mainpage(obj_screen,0);
    DWIN_SetVariable_Icon((Dwin_t *)obj_screen,VP_ICON_ON_OFF,obj_screen->IC_Testerx[obj_screen->curr_device].state);
}

void Screen_ShowData_Mainpage(Screen_t *const screen_obj, uint8_t index_program) {
    uint8_t a_select_program[MAX_PROGRAM_TEST];
    uint8_t size_Name_IC = strlen("IC Name: ") + strlen((char *)screen_obj->Program_Testx[index_program].Name_IC);
    uint8_t size_Description_IC = strlen("IC Description: ") + strlen((char *)screen_obj->Program_Testx[index_program].Description_IC);
    uint8_t size_Num_IC = strlen("Number of ICs: ") + strlen((char *)screen_obj->Program_Testx[index_program].num_IC);

    char *Text_Name_IC = malloc(size_Name_IC + 1); // Null 
    char *Text_Description_IC = malloc(size_Description_IC + 1);
    char *Text_Num_IC = malloc(size_Num_IC + 1);

    strcpy(Text_Name_IC,"IC Name: ");
    strcat(Text_Name_IC,(char *)screen_obj->Program_Testx[index_program].Name_IC);
    strcpy(Text_Description_IC,"IC Description: ");
    strcat(Text_Description_IC,(char *)screen_obj->Program_Testx[index_program].Description_IC);
    strcpy(Text_Num_IC,"Number of ICs: ");
    strcat(Text_Num_IC,(char *)screen_obj->Program_Testx[index_program].num_IC);

    memset(a_select_program,1,MAX_PROGRAM_TEST);
    // Get the program selected
    a_select_program[index_program] = 0 ;
    screen_obj->IC_Testerx[screen_obj->curr_device].selected_Program_Index = index_program;

    // Show data in screen
    DWIN_SetArray_Icon((Dwin_t *)screen_obj,VP_ICON_SELECT_PROGRAM,a_select_program,MAX_PROGRAM_TEST);
    DWIN_SetText((Dwin_t *)screen_obj,VP_Name_IC,Text_Name_IC,size_Name_IC);
    DWIN_SetText((Dwin_t *)screen_obj,VP_Description_IC,Text_Description_IC,size_Description_IC);
    DWIN_SetText((Dwin_t *)screen_obj,VP_Num_IC_Test,Text_Num_IC,size_Num_IC);
    
    /* free malloc */
    free(Text_Name_IC);
    free(Text_Description_IC);
    free(Text_Num_IC);
}

void Screen_SetInfo_Pin(Screen_t *const screen_obj, char **array_data) {
    /* Free the old data */
    // for (uint8_t pin_x = 0; pin_x < screen_obj->Program_Testx[PROGRAM_TEST1].num_pin; pin_x++) {
    //     if (screen_obj->Program_Testx[PROGRAM_TEST1].data_pin[pin_x] != NULL) {
    //         free(screen_obj->Program_Testx[PROGRAM_TEST1].data_pin[pin_x]);
    //     } 
    // }
    uint16_t Address_VP = VP_Name_Pin;
    screen_obj->Program_Testx[PROGRAM_TEST1].data_pin = malloc( screen_obj->Program_Testx[PROGRAM_TEST1].num_pin * sizeof(char *) );
    /* Get new data */
    for (uint8_t pin_x = 0; pin_x < screen_obj->Program_Testx[PROGRAM_TEST1].num_pin; pin_x++) {
        screen_obj->Program_Testx[PROGRAM_TEST1].data_pin[pin_x] = strdup(array_data[pin_x]);
        DWIN_SetText((Dwin_t *)screen_obj,Address_VP,screen_obj->Program_Testx[PROGRAM_TEST1].data_pin[pin_x],strlen(screen_obj->Program_Testx[PROGRAM_TEST1].data_pin[pin_x]));
        Address_VP = Address_VP + 0x10;
    }
}

#pragma endregion FUNCTION SCREEN
