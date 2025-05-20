#include "DWIN_HMI.h"

void DWIN_SetPage(Dwin_t *pDwin, uint8_t page)
{
    uint8_t sendBuffer[10] = {CMD_HEAD1, CMD_HEAD2, 0x07, CMD_WRITE, 0x00, 0x84, 0x5A, 0x01, 0x00, page};

    HAL_UART_Transmit(pDwin->pUart,sendBuffer,sizeof(sendBuffer),1000);
}

void DWIN_SetText(Dwin_t *pDwin, uint16_t VP_address, uint8_t *textData, uint16_t datalen) {
    uint8_t ffEnding[2] = { 0xFF, 0xFF };
    uint8_t sendBuffer[8 + datalen];

    uint8_t startCMD[] = { CMD_HEAD1, CMD_HEAD2, (uint8_t)(datalen + 5), CMD_WRITE,
                           (uint8_t)((VP_address >> 8) & 0xFF), (uint8_t)(VP_address & 0xFF) };
    
    memcpy(sendBuffer, startCMD, sizeof(startCMD));
    memcpy(sendBuffer + 6, textData, datalen);
    memcpy(sendBuffer + (6 + datalen), ffEnding, sizeof(ffEnding));

    HAL_UART_Transmit(pDwin->pUart,sendBuffer,sizeof(sendBuffer),1000);
}

void DWIN_ClearText(Dwin_t *pDwin, uint16_t VP_address) {
    DWIN_SetText(pDwin,VP_address,"",strlen(""));
}

void DWIN_SetColorText(Dwin_t *pDwin, uint16_t SP_address, uint16_t color) {
    SP_address = SP_address | 3; // offset color text

    uint8_t sendBuffer[8] ={ CMD_HEAD1, CMD_HEAD2, 0x05, CMD_WRITE, 
    (uint8_t)((SP_address >> 8) & 0xFF), (uint8_t)((SP_address)&0xFF), 
    (uint8_t)((color >> 8) & 0xFF), (uint8_t)((color)&0xFF) };

    HAL_UART_Transmit(pDwin->pUart,sendBuffer,sizeof(sendBuffer),1000);
}

void DWIN_SetVariable_Icon(Dwin_t *pDwin, uint16_t VP_address, uint16_t data ) {
    uint8_t sendBuffer[8] ={ CMD_HEAD1, CMD_HEAD2, 0x05, CMD_WRITE,
    (uint8_t)((VP_address >> 8) & 0xFF), (uint8_t)(VP_address & 0xFF),
    (uint8_t)((data >> 8) & 0xFF), (uint8_t)(data & 0xFF) };

    HAL_UART_Transmit(pDwin->pUart,sendBuffer,sizeof(sendBuffer),1000);
}

void DWIN_SetArray_Icon(Dwin_t *pDwin, uint16_t VP_address_begin, uint8_t *data_array, uint8_t data_length ) {
    uint16_t Total_Data_Length = data_length * 2;
    uint8_t sendBuffer[6 +Total_Data_Length];
    uint8_t startCMD[] = { CMD_HEAD1, CMD_HEAD2, (uint8_t)(Total_Data_Length + 3), CMD_WRITE,
                           (uint8_t)((VP_address_begin >> 8) & 0xFF), (uint8_t)(VP_address_begin & 0xFF) };

    memcpy(sendBuffer, startCMD, sizeof(startCMD));
    /* Convert data array uint8_t to uint16_t */
    for (uint16_t i = 0; i < data_length; i++) {
        sendBuffer[6 + (i * 2)] = 0x00;              // High byte 
        sendBuffer[6 + (i * 2) + 1] = data_array[i]; // Low byte is data
    }

    HAL_UART_Transmit(pDwin->pUart,sendBuffer,sizeof(sendBuffer),1000);
}

void DWIN_Create_Basic_line(Dwin_t *pDwin, uint16_t VP_address, uint16_t x_base, uint16_t y_base, uint8_t *array_data, uint8_t size) {
    uint8_t data_length = 11 + (16*size);
    uint16_t num_connection = (size * 4) - 1;
    uint8_t sendBuffer[255] = { CMD_HEAD1, CMD_HEAD2, data_length, CMD_WRITE,
                            (uint8_t)((VP_address >> 8) & 0xFF), (uint8_t)(VP_address & 0xFF), 0x00, 0x02,
                            (uint8_t)((num_connection >> 8) & 0xFF), (uint8_t)(num_connection & 0xFF), 0x01, 0xED };
    uint8_t End_bit[2] = {0xFF, 0x00};
    uint16_t x1,y1,x2,y2,x3,y3,x4,y4 = 0;
    uint8_t index = 0;
    

    // Create the send Buffer 
    for (uint8_t i = 0; i < size; i++) {
        y1 = y2 = y3 = y4 = y_base;
        x1 = x_base + (30 * index);
        ++index; 
        x2 = x_base + (30 * index);
        ++index;
        x3 = x2;
        x4 = x_base + (30 * index);
        if (array_data[i]) { // HIGH CLOCK
            y1 = y2 = y3 = y4 = y_base - 30;  
        }
        // Create clock buffer
        uint8_t Clock_Buffer[] = { (x1 >> 8) & 0xFF, (x1 & 0xFF), (y1 >> 8) & 0xFF, (y1 & 0xFF),
                        (x2 >> 8) & 0xFF, (x2 & 0xFF), (y2 >> 8) & 0xFF, (y2 & 0xFF), 
                        (x3 >> 8) & 0xFF, (x3 & 0xFF), (y3 >> 8) & 0xFF, (y3 & 0xFF), 
                        (x4 >> 8) & 0xFF, (x4 & 0xFF), (y4 >> 8) & 0xFF, (y4 & 0xFF), };

        // add to sendBuffer 
        memcpy(sendBuffer + (12 + 16 * i) ,Clock_Buffer,sizeof(Clock_Buffer));
    }
    // End bit 
    memcpy(sendBuffer + (data_length + 1),End_bit,2);


    // Send data
    HAL_UART_Transmit(pDwin->pUart,sendBuffer,sizeof(sendBuffer),1000);
}

void DWIN_SetWidth_Basic_line(Dwin_t *pDwin, uint16_t SP_Address, uint8_t width) {
    SP_Address = SP_Address | 7; // offset width line 
    uint8_t sendBuffer[] = {CMD_HEAD1, CMD_HEAD2, 0x05, CMD_WRITE , 
    (uint8_t)((SP_Address >> 8) & 0xFF), (uint8_t)(SP_Address & 0xFF),
    (uint8_t)(0x00), (uint8_t)(width & 0xFF) };
    HAL_UART_Transmit(pDwin->pUart,sendBuffer,sizeof(sendBuffer),500);
}

uint8_t DWIN_GetPage(Dwin_t *pDwin)
{
    uint8_t dataLen = 0x04;
    UNUSED(dataLen);

    return 0;
}

void CRC_Control(Dwin_t *pDwin, bool status)
{
    if( status )
    {
        pDwin->p_crc_ena = true;
    } else
    {
        pDwin->p_crc_ena = false;
    }
}

// Set DWIN callback function when receive data 
void DWIN_SetCallback(Dwin_t *pDwin, pListenDWIN dwin_callback)
{
    pDwin->p_ListenDWIN_Callback = dwin_callback;
}

void DWIN_Listen(Dwin_t *pDwin, uint8_t *RX_Buffer)
{
    uint8_t datalength;
    uint8_t message_ASCII;
    uint16_t VPaddress;
    uint16_t lastByte;


    if( RX_Buffer[0] == 0x5A && RX_Buffer[1] == 0xA5 ) 
    {
        if( RX_Buffer[3] == 0x83 )   // READ INSTRUCTION 
        {
            datalength = RX_Buffer[2];
            datalength += 2; // Plus with Frame header
            // get vp address
            VPaddress = (uint16_t)(RX_Buffer[4] << 8 | RX_Buffer[5]);
            // get return key value
            lastByte = (uint16_t)(RX_Buffer[datalength-1] << 8 | RX_Buffer[datalength]);
            // Get message ASCII
            message_ASCII = RX_Buffer[datalength];

            // Call back function
            if( pDwin->p_ListenDWIN_Callback != NULL )
            {
                pDwin->p_ListenDWIN_Callback(VPaddress,lastByte,message_ASCII);
            }
        }
    }

    
}
