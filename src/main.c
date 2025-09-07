#include "common.h"
#include "pinconfig.h"
#include "uart.h"

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

void main()
{
  uint8_t rCharacter[20];

  Pin_Config(Port_PA, 0, PA0_U0RX);
  Pin_Config(Port_PA, 1, PA1_U0TX);

  UART_Init(115200);

  UART_sendString("External Loopback mode\n");

  while(1)
  {
    UART_receiveString(rCharacter);
    UART_sendString(rCharacter);
  }
}