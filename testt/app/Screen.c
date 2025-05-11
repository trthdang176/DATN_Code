#include "Screen.h"

Screen_t _Screen;  // screen object

/* Private prototype function */
void Screen_init_handler_table(Screen_t *const obj_screen);
void Screen_excute_RX_function(Screen_t *const obj_screen,screen_event_t *const screen_e);
void Screen_Icon_Pin(Screen_t *const obj_screen, uint8_t **data_pin);
void Screen_Icon_Result(Screen_t *const obj_screen, char **result);

Return_Status Screen_CheckInput_Keyboard(Screen_t *const obj_screen);


/* Array store the VP address signal */
VP_item lookup_VP_SIG[] = {
    {VP_Navigation_button,SIG_NAVIGATION},
    {VP_ON_OFF_button,SIG_ON_OFF},
    {VP_Navigation_keyboard,SIG_NAVIGATION_KEYBOARD},
    {VP_Keyboard,SIG_KEYBOARD}
};

/* The array store the VP address to check data input */
VP_item lookup_VP_check_datakeyboard[] = {
    {VP_Day,CONDITION_DAY},
    {VP_Month,CONDITION_MONTH},
    {VP_Year,CONDITION_YEAR},
    {VP_Minute,CONDITION_MINUTE},
    {VP_Hour,CONDITION_HOUR}
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

    // Init screen keyboard     
    _Screen.Screen_keyboard.VP_Text = 0xFFFF;
    _Screen.Screen_keyboard.Index_String = 0;
    _Screen.Screen_keyboard.Caplock = 0;   
    
    DWIN_SetPage((Dwin_t *)&_Screen,106);  // Go to first page when start
    DWIN_SetText((Dwin_t *)&_Screen,0x7000,(uint8_t *)"Dang tt", strlen("Dangtt"));
    DWIN_SetText((Dwin_t *)&_Screen,0x7010,(uint8_t *)"Dangtt1762003", sizeof("Dangtt1762003"));
    uint8_t text_show[] = {0x31,0x32};
    DWIN_SetText((Dwin_t *)&_Screen,0x9000,text_show,2);
    _Screen.pre_page = 106;

    // uint8_t data_clock[] = {1,1,0,1,0,1};
    // DWIN_Create_Basic_line((Dwin_t *)&_Screen,0x1800,25,200,data_clock,sizeof(data_clock));
    // HAL_Delay(200);
    // uint8_t data_clock2[] = {1,0,1,0,1,0,1,0,1,0};
    // DWIN_Create_Basic_line((Dwin_t *)&_Screen,0x1800,25,200,data_clock2,sizeof(data_clock2));
    // HAL_Delay(200);
    // uint8_t data_clock3[] = {0,1,0,1,0,1,0,1,0,1,1,1};
    // DWIN_Create_Basic_line((Dwin_t *)&_Screen,0x1800,25,200,data_clock3,sizeof(data_clock3));
    // uint8_t data[15] = {1,1,0,0,1,0,0,1,1,0,0,0,1,1,0};
    // DWIN_Create_Basic_line((Dwin_t *)&_Screen,0x1800,25,200,data,sizeof(data));
    // uint8_t current = 0;

    // for(uint8_t i = 0; i < 15; i++) {
    //     DWIN_Create_Basic_line((Dwin_t *)&_Screen,0x1800,25,200,data,current);
    //     ++current;
    //     HAL_Delay(500);
    // }

    _Screen.Status_Dev_1 = false;
}

// USED IN RX UART CALLBACK FUNCTION WHEN RECEIVE DATA
void Screen_RX_data(uint8_t *RX_Buffer) {
    DWIN_Listen((Dwin_t *)&_Screen, RX_Buffer);
}

void Screen_RX_Callback(uint16_t Vpaddress, uint16_t lastByte, uint8_t ASCII_Code) { 
    screen_event_t screen_event;

    screen_event.event = Index_VP(Vpaddress,lookup_VP_SIG,SIZE_LOOKUP_VP_SIG);
    screen_event.data = lastByte;
    screen_event.ASCII_Code = ASCII_Code;

    // call to respective function
    Screen_excute_RX_function(&_Screen,&screen_event);
}


#pragma region CALLBACK FUNCTION VP ADRRESS 

void Navigation_setting_page(Screen_t *const screen_obj, screen_event_t *const screen_event) {
    DWIN_SetPage((Dwin_t *)screen_obj,DWINPAGE_SETTING);
}

void Navigation_home_page(Screen_t *const screen_obj, screen_event_t *const screen_event) {
    // Show data

}   

void Navigation_Next_MainPage(Screen_t *const screen_obj, screen_event_t *const screen_event) {

    // Change current device
    screen_obj->curr_device++;

    // Show data with correspond device index
    switch (screen_obj->IC_Testerx[screen_obj->curr_device].curr_PageMain) {
        case DWINPAGE_MAIN : {
            // Show info 

        } break;
        case DWINPAGE_MAIN_FINISH: {

        } break;
        case DWINPAGE_MAIN_DETAIL : {

        } break;
        case DWINPAGE_MAIN_GRAPH : {

        } break;
    }
}

void Navigation_Next_PrevPage(Screen_t *const screen_obj, screen_event_t *const screen_event) {

}

void ON_OFF_Button(Screen_t *const screen_obj, screen_event_t *const screen_event) {
    // do sth
    screen_obj->Status_Dev_1 = !screen_obj->Status_Dev_1;
    // change icon
    // DWIN_SetVariable_Icon((Dwin_t *)screen_obj,VP_ON_OFF_Icon_1,ON_ICON);
    //    HAL_Delay(1000);
    DWIN_SetVariable_Icon((Dwin_t *)screen_obj,VP_ON_OFF_Icon_1,screen_obj->Status_Dev_1);

}

void Navigation_keyboard(Screen_t *const screen_obj, screen_event_t *const screen_event) {
    // Switch screen keyboard will excute in screen

    // get the datavalue -> the VP will show string input
    screen_obj->Screen_keyboard.VP_Text = screen_event->data;

    // Reset the string
    screen_obj->Screen_keyboard.Index_String = 0;
    memset(screen_obj->Screen_keyboard.String,0,sizeof(screen_obj->Screen_keyboard.String));
    /* Show string input keyboard */
    DWIN_SetText((Dwin_t *)screen_obj,VP_ShowString_Keyboard,screen_obj->Screen_keyboard.String,sizeof(screen_obj->Screen_keyboard.String));
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
        if (screen_obj->Screen_keyboard.Caplock) { // Caplock is on
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
    for (int i = left; i <= right; i++) {
        mid = (left + right)/2;
        if (VP_array[mid].VP_address == vp) {
            return VP_array[mid].index;     // Return the index corresponding to the VP
        } else if (VP_array[mid].VP_address > vp) {
            right = mid - 1;
        } else if (VP_array[mid].VP_address < vp) {
            left = mid + 1;
        }
    }
    return 255; // VP input is not valid
}

void Screen_init_handler_table(Screen_t *const obj_screen) {
    // table handler the callback function
    static handler_func a_handler_navigation_sig[MAX_VALUE_SIG_NAVIGATE] = {NULL};
    static handler_func a_handler_on_off_sig[MAX_VALUE_SIG_ON_OFF] = {NULL};
    static handler_func a_handler_keyboard_sig[MAX_VALUE_KEYBOARD] = {NULL};
    static handler_func a_handler_navigation_keyboard[MAX_VALUE_NAVIGATION_KEYBOARD] = {NULL};

    static handler_func *handler_function_table[MAX_SIG_VP] = { a_handler_navigation_sig,
                                                                a_handler_on_off_sig ,
                                                                a_handler_keyboard_sig,
                                                                a_handler_navigation_keyboard  };

    // Assign the callback function to handler table
    handler_function_table[SIG_NAVIGATION][NAVIGATION_SETTING_PAGE]     = Navigation_setting_page ;
    handler_function_table[SIG_NAVIGATION][NAVIGATION_HOME]             = Navigation_home_page;
    handler_function_table[SIG_NAVIGATION][NAVIGATION_NEXT_MAINPAGE]    = Navigation_Next_MainPage;
    handler_function_table[SIG_NAVIGATION][NAVIGATION_PREV_MAINPAGE]    = Navigation_Next_PrevPage;
    handler_function_table[SIG_ON_OFF][ON_OFF_DEV_1]                    = ON_OFF_Button;

    handler_function_table[SIG_NAVIGATION_KEYBOARD][VALUE_NAVIGATION_KEYBOARD] = Navigation_keyboard;

    handler_function_table[SIG_KEYBOARD][VALUE_KEBOARD]             = Keyboard;

    // assign handler table to handler table of object
    obj_screen->p_handler_table = (void *)handler_function_table;
}

void Screen_excute_RX_function(Screen_t *const obj_screen,screen_event_t *const screen_e) {
    // do sth ...


	handler_func **handler_function_table = (handler_func **) obj_screen->p_handler_table;
    // excute the callback function
    if (screen_e->event == SIG_KEYBOARD) {  // ENVENT KEYBOARD
        handler_function_table[SIG_KEYBOARD][VALUE_KEBOARD](obj_screen, screen_e);   
    } else if (screen_e->event ==SIG_NAVIGATION_KEYBOARD) {  // SWTICH TO KEYBOARD BUTTON
        handler_function_table[SIG_NAVIGATION_KEYBOARD][VALUE_NAVIGATION_KEYBOARD](obj_screen, screen_e);  
    } else {
        handler_function_table[screen_e->event][screen_e->data](obj_screen, screen_e);
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
        DWIN_SetText((Dwin_t *)obj_screen,VP_ShowWarning_Keyboard,(uint8_t *)warm_string,sizeof(warm_string));
        return Status_ERROR;
    } else if (data_compare < obj_screen->Screen_condition->min_value) {
        // Create warm string
        snprintf(warm_string,sizeof(warm_string),"MIN: %.2f", obj_screen->Screen_condition->min_value);
        // Display warning
        DWIN_SetText((Dwin_t *)obj_screen,VP_ShowWarning_Keyboard,(uint8_t *)warm_string,sizeof(warm_string));
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
void Screen_Icon_Pin(Screen_t *const obj_screen, uint8_t **data_pin) {
    uint8_t map_array[128] = {0};
    map_array['0'] = ICON_INPUT;
    map_array['1'] = ICON_INPUT;
    map_array['L'] = ICON_OUTPUT;
    map_array['H'] = ICON_OUTPUT;
    map_array['G'] = ICON_GND;
    map_array['V'] = ICON_VCC;
    /* Convert data to icon pin config */
    for (uint8_t Case_x = 0; Case_x < obj_screen->Program_Testx[obj_screen->curr_device].num_case; Case_x++) {
        for (uint8_t Pin_x = 0; Pin_x < obj_screen->Program_Testx[obj_screen->curr_device].num_pin; Pin_x++) {
            obj_screen->IC_Testerx[obj_screen->curr_device].config_pin[Case_x][Pin_x] = map_array[(uint8_t)(data_pin[Case_x][Pin_x])];
        }
    }
}

void Screen_Icon_Result(Screen_t *const obj_screen, char **result) {
    uint8_t map_array[128] = {0};
    map_array['0'] = ICON_LOGIC_LOW;
    map_array['1'] = ICON_LOGIC_HIGH;
    /* Convert data to icon pin config */
    for (uint8_t Case_x = 0; Case_x < obj_screen->Program_Testx[obj_screen->curr_device].num_case; Case_x++) {
        for (uint8_t Pin_x = 0; Pin_x < obj_screen->Program_Testx[obj_screen->curr_device].num_pin; Pin_x++) {
            if (result[Case_x][Pin_x] == 'V' || result[Case_x][Pin_x] == 'G') continue; // Skip Vcc and Gnd pin
            obj_screen->IC_Testerx[obj_screen->curr_device].data_result[Case_x][Pin_x] = map_array[(uint8_t)(result[Case_x][Pin_x])];
        }
    }
}

#pragma endregion FUNCTION SCREEN


