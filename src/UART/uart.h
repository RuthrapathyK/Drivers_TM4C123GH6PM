#ifndef __UART_H
#define __UART_H

#include "common.h"

#define UART_MAX_QUEUE_COUNT 10

typedef struct{
  int32_t rIdx;
  int32_t pIdx;
  uint8_t *Buffer;
}UART_Queue_t;

typedef enum{
  UART_0,
  UART_1,
  UART_2,
  UART_3,
  UART_4,
  UART_5,
  UART_6,
  UART_7,
  UART_Module_Max
}UART_Module_e;

typedef enum{
  UART_Even_Parity,
  UART_Odd_Parity,
  UART_No_Parity
}UART_Parity_e;

typedef enum{
  UART_StopBit_One,
  UART_StopBit_Two
}UART_StopBit_e;

typedef struct{
    uint32_t UART_BaudRate;
    UART_StopBit_e UART_StopBit;
    UART_Parity_e UART_Parity;
}UART_config_t;
/**
 * @brief Initializes UART0 with the specified baud rate.
 *
 * Configures UART0 for 8-bit, no parity, 1 stop bit, FIFO enabled, and sets the baud rate.
 * Uses system clock as source and prescaler of 8.
 *
 * @param baudrate Desired baud rate (e.g., 115200)
 */
void UART_Init(UART_Module_e mod, uint32_t baudrate);

/**
 * @brief Sends a single character over UART0.
 *
 * Waits until the transmit FIFO is not full, then writes the character.
 *
 * @param ch Character to send
 */
void UART_sendChar(char ch);

/**
 * @brief Sends a null-terminated string over UART0.
 *
 * Sends each character in the string using UART_sendChar.
 *
 * @param str Pointer to the string to send
 */
void UART_sendString(char * str);

/**
 * @brief Sends a signed 32-bit integer as ASCII characters over UART0.
 *
 * Converts the number to a string and sends it using UART_sendString.
 *
 * @param num Number to send
 */
void UART_sendNumber(int32_t num);

/**
 * @brief Receives a single character from UART0.
 *
 * Waits until the receive FIFO is not empty, then returns the character.
 *
 * @return Received character
 */
uint8_t UART_receiveChar(void);

/**
 * @brief Receives a string from UART0 until newline.
 *
 * Reads characters into strBuf until '\n' is received, then null-terminates the buffer.
 *
 * @param strBuf Pointer to buffer for received string
 */
void UART_receiveString(uint8_t * strBuf);

#endif