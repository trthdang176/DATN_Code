#ifndef __DWIN_HMI_H__
#define __DWIN_HMI_H__

#include "../Core/Inc/main.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define CMD_HEAD1 0x5A
#define CMD_HEAD2 0xA5
#define CMD_WRITE 0x82
#define CMD_READ  0x83

#define TIMEOUT_TRANSMIT 50

typedef void (*pListenDWIN)(uint16_t Vpaddress, uint8_t lowByte, uint8_t highByte);

typedef struct {
	UART_HandleTypeDef *pUart;

    bool p_crc_ena ;
    pListenDWIN p_ListenDWIN_Callback;

} Dwin_t;

void DWIN_SetPage(Dwin_t *pDwin, uint8_t page);
void DWIN_SetText(Dwin_t *pDwin, uint16_t VP_address, char *textData, uint16_t datalen);
void DWIN_ClearText(Dwin_t *pDwin, uint16_t VP_address);
void DWIN_SetColorText(Dwin_t *pDwin, uint16_t SP_address, uint16_t color);
void DWIN_SetVariable_Icon(Dwin_t *pDwin, uint16_t VP_address, uint16_t data ); 
void DWIN_SetArray_Icon(Dwin_t *pDwin, uint16_t VP_address_begin, uint8_t *data_array, uint8_t data_length );


void DWIN_Create_Basic_line(Dwin_t *pDwin, uint16_t VP_address, uint16_t x_base, uint16_t y_base, uint8_t *array_data, uint8_t size);
void DWIN_SetWidth_Basic_line(Dwin_t *pDwin, uint16_t SP_Address, uint8_t width);
void DWIN_Create_Single_line(Dwin_t *pDwin, uint16_t VP_address, uint16_t x_1, uint16_t y_1, uint16_t x_2, uint16_t y_2, uint16_t color);

uint8_t DWIN_GetPage(Dwin_t *pDwin);
void DWIN_SetCallback(Dwin_t *pDwin, pListenDWIN dwin_callback);
void DWIN_Listen(Dwin_t *pDwin, uint8_t *RX_Buffer);

#endif /* __DWIN_HMI_H__ */
