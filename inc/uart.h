#ifndef __UART_H
#define __UART_H

#include "common.h"

void UART_sendChar(char ch);
void UART_sendString(char * str);
void UART_sendNumber(int32_t num);
void UART_Init(uint32_t baudrate);
uint8_t UART_receiveChar(void);
void UART_receiveString(uint8_t * strBuf);

#endif