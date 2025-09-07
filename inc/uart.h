#ifndef __UART_H
#define __UART_H

#include "common.h"

/**
 * @brief Initializes UART0 with the specified baud rate.
 *
 * Configures UART0 for 8-bit, no parity, 1 stop bit, FIFO enabled, and sets the baud rate.
 * Uses system clock as source and prescaler of 8.
 *
 * @param baudrate Desired baud rate (e.g., 115200)
 */
void UART_Init(uint32_t baudrate);

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