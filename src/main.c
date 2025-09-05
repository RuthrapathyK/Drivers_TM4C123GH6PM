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
  UART_Init(115200);

  while(1)
  {
    UART_sendString("Hello World\n");
    delayLoop(1000);
  }
}