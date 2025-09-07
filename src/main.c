#include "common.h"
#include "pinconfig.h"
#include "uart.h"
#include "nvic.h"



uint8_t counter = 0;

/**
 * @brief The function is used to create a blocking delay. It used traditional for loop
 * 
 * @param mSec delay time in approximate milli-seconds
 */
void delayLoop(uint32_t mSec)
{
  for (volatile uint32_t i = 0; i < mSec ; i++)
  {
    for(volatile uint32_t j =0; j < 500; j++)
    {
      __asm("NOP");
    }
  }
}

void UART0_handler(void)
{
  /* Clear the Interrupt */
  UART0->ICR |= (1<<5 | 1<<4); 

  counter++;
}

void main()
{
  uint8_t rCharacter[20];

  /* Configure UART pins */
  Pin_Config(Port_PA, 0, PA0_U0RX);
  Pin_Config(Port_PA, 1, PA1_U0TX);

  /* Initialize UART */
  UART_Init(115200);
  UART0->IM |= (1<<5 | 1<<4);// Enable Interrupt from UART module

  /* Enable NVIC interrupt of UART */
  NVIC_enableInterrupt(UART_0_IRQ);

  UART_sendString("UART Initialized\n");

  while(1)
  {
    UART_receiveString(rCharacter);
    UART_sendString(rCharacter);
  }
}