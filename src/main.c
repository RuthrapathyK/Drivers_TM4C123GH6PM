#include "common.h"

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
  char num_arr[15] = {0};
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
    num /= 10;  // Reove the digit
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
void UART_Init(uint32_t baudrate)
{
  /* Enable Clock for UART0 module */
  SYSCTL->RCGCUART |= 1<<0;

  /* Enable Clock for GPIO Port A */
  SYSCTL->RCGCGPIO |= 1<<0;

  /* Choose the Alternate Function as UART for Pins PA0 - Rx and PA1 - Tx */
  GPIOA->AFSEL |= (1<<1 | 1<<0);
  GPIOA->DEN |= (1<<1 | 1<<0);
  GPIOA->PCTL &= ~(0x0F<<0 | 0x0F<<4);
  GPIOA->PCTL |= (1<<0 | 1<< 4);

  /* Disable UART */
  UART0->CTL &= ~(1<<0);

  /* Configure Baudrate */
  float Baud_Val = (float)(16000000.0f / (8.0f * baudrate)); // Derive value to be written in Register 
  uint16_t Baud_Integer = (uint16_t)Baud_Val; // Derive the Integer part of the Value
  uint8_t Baud_Fraction = (uint8_t)((((float)Baud_Val - (float)Baud_Integer) * 64.0f) + 0.5f); // Derive the Fraction part of the Value
  UART0->IBRD = Baud_Integer;
  UART0->FBRD = Baud_Fraction;

  /* Configure Stopbit, Parity, FIFOs, Word Length */
  UART0->LCRH |= 0x3 << 5;

  /* Set prescaslar to be 8 */
  UART0->CTL |= (1<<5); // Select UART prescalar as 8
//  UART0->CTL |= (1<<7); // Enable LoopBack mode 

  /* Select UART module's clock source - System Clock(16MHz) */
  UART0->CC &= ~(0x0F << 0);

  /* Enable UART */
  UART0->CTL |= 1<<0;
}
void main()
{
  int32_t counter = 0;
  UART_Init(115200);

  while(1)
  {
    UART_sendNumber(counter++);
    counter %= 100;
    UART_sendChar('\n');
    delayLoop(1000);
  }
}