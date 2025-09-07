#include "common.h"
#include "pinconfig.h"
#include "uart.h"
#include "nvic.h"

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
uint8_t counter = 0;
void UART0_handler(void)
{
  UART0->ICR |= (1<<5 | 1<<4); 
  counter++;
}

void main()
{
  uint8_t rCharacter[20];

  Pin_Config(Port_PA, 0, PA0_U0RX);
  Pin_Config(Port_PA, 1, PA1_U0TX);

  UART_Init(115200);
  UART0->IM |= (1<<5 | 1<<4);
  NVIC_enableInterrupt(UART_0_IRQ);
  UART_sendString("UART Initialized\n");

  while(1)
  {
    UART_receiveString(rCharacter);
    UART_sendString(rCharacter);
  }
}