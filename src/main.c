#include "common.h"
#include "../src/UART/uart.h"
#include "../src/PinMux/pinconfig.h"

void BoardPins_Init(void)
{
  Pin_Config(Port_PA, 0, PA0_U0RX);
  Pin_Config(Port_PA, 1, PA1_U0TX);
}
void main()
{
  UART_Init(115200);

  BoardPins_Init();

  while(1)
  {
    /* Receive Character in UART */
    uint8_t ch = 0;

    /* Wait till RX buffer is not empty */
    while(((UART0->FR >> 4) & 0x01))
    ;

    ch = UART0->DR;
    if(((ch >> 8) & 0x0F) != 0)
      ASSERT(0);

    /* Sendback the Received Character */
    UART_sendChar(ch);    
  }
}