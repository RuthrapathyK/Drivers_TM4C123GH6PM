#include "uart.h"

extern void delayLoop(uint32_t mSec);

void UART_Init(uint32_t baudrate)
{
  /* Enable Clock for UART0 module */
  SYSCTL->RCGCUART |= 1<<0;

  /* Calculate Baudrate */
  float Baud_Val = (float)(16000000.0f / (8.0f * baudrate)); // Derive value to be written in Register 
  uint16_t Baud_Integer = (uint16_t)Baud_Val; // Derive the Integer part of the Value
  uint8_t Baud_Fraction = (uint8_t)((((float)Baud_Val - (float)Baud_Integer) * 64.0f) + 0.5f); // Derive the Fraction part of the Value
  
  /* Disable UART */
  UART0->CTL &= ~(1<<0);
  
  /* Write the Baudrate */
  UART0->IBRD = Baud_Integer;
  UART0->FBRD = Baud_Fraction;

  /* Configure Stopbit, Parity, FIFOs, Word Length */
  UART0->LCRH |= 0x3 << 5;

  /* Set prescaslar to be 8 */
  UART0->CTL |= (1<<5); // Select UART prescalar as 8

  /* Select UART module's clock source - System Clock(16MHz) */
  UART0->CC &= ~(0x0F << 0);

  /* Enable UART */
  UART0->CTL |= 1<<0;
}

void UART_sendChar(char ch)
{
    /* Wait till Transmission is completed */
    while(((UART0->FR >> 3) & 0x01))
    ;
    UART0->DR = ch;
}

void UART_sendString(char * str)
{

  while(*str)
  {
    UART_sendChar(*str);
    str++;
  }
}

void UART_sendNumber(int32_t num)
{
  char num_arr[20] = {0};
  uint8_t first_idx = 0, last_idx = 0;
  char temp_char = 0;

  /* Check whether the input is non-printable */
  ASSERT(num != -2147483648)  

  /* Check if the number is negative */
  if(num < 0)
  {
    UART_sendChar('-'); // Send minus character first
    num *= -1; // Make the number positive
  }

  do
  {
    num_arr[last_idx] = (num % 10)+'0'; // Store the digit as character
    num /= 10;  // Remove the digit
    last_idx++;
  }while(num);

  num_arr[last_idx] = '\0'; // Place NULL character at the end of the string array

  last_idx--; // Point the index to the last character of the num_arr

  /* Swap the Characters */
  while(last_idx > first_idx)
  {
    temp_char = num_arr[first_idx];
    num_arr[first_idx] = num_arr[last_idx];
    num_arr[last_idx] = temp_char;

    last_idx--;
    first_idx++;
  }

  UART_sendString(num_arr);
}

uint8_t UART_receiveChar(void)
{
    /* Wait till TX buffer is empty */
    while(((UART0->FR >> 4) & 0x01))
    ;
    return (UART0->DR & 0xFF);
}

void UART_receiveString(uint8_t * strBuf)
{
    uint8_t rChar = 0;

    do{
        rChar = UART_receiveChar();
        *strBuf = rChar;
        strBuf++;
    }while(rChar != '\n');

    *strBuf = '\0';
}