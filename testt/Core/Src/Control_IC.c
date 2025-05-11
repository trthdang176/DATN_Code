#include "Control_IC.h"

uint16_t DSAx[] = {DSA1,DSA2,DSA3};
uint16_t DSBx[] = {DSB1,DSB2,DSB3};
uint16_t CPx[] =  {CP1,CP2,CP3};
uint16_t MRx[] =  {MR1,MR2,MR3};
//uint8_t  OEx[] =  {OE1_Bit,OE2_Bit,OE3_Bit};

uint8_t LEx_bit[Latch_IC_MAX] = {LE1_Bit,LE2_Bit,LE3_Bit,LE4_Bit,LE5_Bit,LE6_Bit,LE7_Bit};

Pin_IC_Test_Mapping PIN_OEx[] = {
	{GPIOC,GPIO_PIN_10},
    {GPIOA,GPIO_PIN_8},
    {GPIOC,GPIO_PIN_8}
};

Pin_IC_Test_Mapping PIN_LEx[] = {
    {GPIOE,GPIO_PIN_15},
    {GPIOE,GPIO_PIN_14},
    {GPIOE,GPIO_PIN_11},
    {GPIOE,GPIO_PIN_13},
    {GPIOE,GPIO_PIN_12},
    {GPIOE,GPIO_PIN_10}
};


// Input pin IC Test
Pin_IC_Test_Mapping Pin_IC_Test[] = { 
    {GPIOC,GPIO_PIN_7},
    {GPIOC,GPIO_PIN_6},
    {GPIOD,GPIO_PIN_15},
    {GPIOD,GPIO_PIN_14},
    {GPIOD,GPIO_PIN_13},
    {GPIOD,GPIO_PIN_12},
    {GPIOD,GPIO_PIN_11},
    {GPIOD,GPIO_PIN_10}
};

// Output pin IC Test
Pin_IC_Test_Mapping PIN_IC_Test_Output[] = {
    {GPIOD,GPIO_PIN_7},
    {GPIOD,GPIO_PIN_6},
    {GPIOD,GPIO_PIN_5},
    {GPIOD,GPIO_PIN_4},
    {GPIOD,GPIO_PIN_3},
    {GPIOD,GPIO_PIN_2},
    {GPIOD,GPIO_PIN_1},
    {GPIOD,GPIO_PIN_0}
};


uint8_t SHIFT_ICx[Shift_IC_MAX] = {Shift_IC1,Shift_IC2,Shift_IC3};
GPIO_TypeDef* GPIO_PORT_IC[] = {GPIOE,GPIOE,GPIOB};

void Control_IC_begin(void)
{
    // Enable Shift Register IC
    // HAL_GPIO_WritePin(GPIO_PORT_IC[0],MRx[0],GPIO_PIN_SET);
    // HAL_GPIO_WritePin(GPIO_PORT_IC[1],MRx[1],GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIO_PORT_IC[2],MRx[2],GPIO_PIN_SET);

    // Enable Shift Level IC 
    HAL_GPIO_WritePin(TXS_OE1_GPIO_Port,TXS_OE1_Pin,GPIO_PIN_SET);
    HAL_GPIO_WritePin(TXS_OE2_GPIO_Port,TXS_OE2_Pin,GPIO_PIN_SET);
    HAL_GPIO_WritePin(TXS_OE3_GPIO_Port,TXS_OE3_Pin,GPIO_PIN_SET);
    HAL_GPIO_WritePin(TXS_OE4_GPIO_Port,TXS_OE4_Pin,GPIO_PIN_SET);
}

void shift_out(uint8_t num, uint8_t data)
{
    for(uint8_t i =0; i < 8; i++ )
    {
        HAL_GPIO_WritePin(GPIO_PORT_IC[num],CPx[num],GPIO_PIN_RESET);   // Clock LOW
        delay_us(1);
        // Data
        HAL_GPIO_WritePin(GPIO_PORT_IC[num],DSAx[num], (data >> i) & 0x01 );
        HAL_GPIO_WritePin(GPIO_PORT_IC[num],DSBx[num], (data >> i) & 0x01 );
        delay_us(1);
        HAL_GPIO_WritePin(GPIO_PORT_IC[num],CPx[num],GPIO_PIN_SET);   // Clock HIGH
    }
}

/**
 * @brief Control 74HC164D , the output of 74HC164D is the OE and LE of 74HC164D
 * 
 */
void Latch_IC_begin(void)
{
    // Disable all Latch pin
	for (uint8_t i = 0; i< 6; i++) {
		HAL_GPIO_WritePin(PIN_LEx[i].Port_x,PIN_LEx[i].PIN_x,GPIO_PIN_RESET);
	}

	// Disable Latch IC Output IC test
	for(uint8_t i = 0; i < 3; i++) {
		HAL_GPIO_WritePin(PIN_OEx[i].Port_x,PIN_OEx[i].PIN_x,GPIO_PIN_SET);
	}
}

void Control_Vcc_pin(uint8_t data)
{
    shift_out(Shift_IC3,data);
}


// Control the pin IO TEST
void WritePin_ICTest(uint16_t pin) {
    // HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,GPIO_PIN_SET); // D0 
    // HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_SET); // D1 

}



void Control_Input_IC_Test(uint8_t num, uint16_t data) {
    /* Control the Latch IC with num */
    HAL_GPIO_WritePin(PIN_LEx[num].Port_x,PIN_LEx[num].PIN_x,GPIO_PIN_SET);

    /* Set the input data */
    for (uint8_t i = 0; i < 8; i++) {
        HAL_GPIO_WritePin(Pin_IC_Test[i].Port_x,Pin_IC_Test[i].PIN_x,(GPIO_PinState)(data >> i) & (0x01));
    }

    /* Lock Latch IC */
    HAL_GPIO_WritePin(PIN_LEx[num].Port_x,PIN_LEx[num].PIN_x,GPIO_PIN_RESET);

}

void Control_Output_IC_Test(uint8_t num, uint8_t *array_data) {
    /* Control the Latch IC with num */
    HAL_GPIO_WritePin(PIN_LEx[num].Port_x,PIN_LEx[num].PIN_x,GPIO_PIN_SET);
    HAL_GPIO_WritePin(PIN_OEx[num-3].Port_x,PIN_OEx[num-3].PIN_x,GPIO_PIN_RESET);


    /* Get the output data */
    if (num == Latch_IC4) {
        for (uint8_t i = 0; i < 8; i++) {
            array_data[i] = HAL_GPIO_ReadPin(PIN_IC_Test_Output[i].Port_x,PIN_IC_Test_Output[i].PIN_x);
        }
    } else if (num == Latch_IC5) {
        for (uint8_t i = 0; i < 4; i++) {
            array_data[i+8] = HAL_GPIO_ReadPin(PIN_IC_Test_Output[i].Port_x,PIN_IC_Test_Output[i].PIN_x);
        }
    } else if (num == Latch_IC6) {
        for (uint8_t i = 0; i < 8; i++) {
            array_data[i+12] = HAL_GPIO_ReadPin(PIN_IC_Test_Output[i].Port_x,PIN_IC_Test_Output[i].PIN_x);
        }
    }
    
    /* Lock Latch IC */
    HAL_GPIO_WritePin(PIN_LEx[num].Port_x,PIN_LEx[num].PIN_x,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(PIN_OEx[num-3].Port_x,PIN_OEx[num-3].PIN_x,GPIO_PIN_SET);

}

void Control_Program_IC_Test(char *data, uint8_t numPin) {
    /* Get the data INPUT IC */
    uint16_t data_input = 0;
    for (uint8_t i = 0; i < numPin; i++) {
        switch (data[i]) {
            /* INPUT HIGH */
            case 'V' :
            case '1' : {
                data_input |= (1u << i);
            } break;
            /* INPUT LOW */
            case 'L' :
            case 'H' :
            case 'G' :
            case '0' : {
                data_input = data_input & ~(1u << i) ;
            } break;
            default :
                break;
             
        }
    }

    /* Control input IC Test */
    Control_Input_IC_Test(Latch_IC1,(uint8_t)(data_input&0xFF));
    Control_Input_IC_Test(Latch_IC1,(uint8_t)(data_input>>8));
}
