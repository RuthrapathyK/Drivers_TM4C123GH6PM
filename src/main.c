#include "common.h"
#include "../src/UART/uart.h"
#include "../src/PinMux/pinconfig.h"
#include "../src/NVIC/nvic.h"

extern UART_Queue_t UART_Queue;

void BoardPins_Init(void)
{
  Pin_Config(Port_PA, 0, PA0_U0RX);
  Pin_Config(Port_PA, 1, PA1_U0TX);
}
void main()
{

  /* Init UART */
  UART_Init(UART_0, 115200);

  /* Init the Pin Configurations */
  BoardPins_Init();

  /* Init Interrupts */
  NVIC_enableInterrupt(UART_0_IRQ);
  __enable_irq();

  while(1)
  {
    /* Sendback the Received Character */
    while(UART_Queue.Count)
    {

      delayLoop(10);
      /* Transmit the Received Character */
      UART_sendChar(UART_Queue.Buffer[UART_Queue.StartIdx]);

      /* Increament the Start Index */
      UART_Queue.StartIdx++;

      /* Roundoff the Start Index */
      UART_Queue.StartIdx %=  UART_Queue.MaxCount;

      /* Reduce the Count Value */
      UART_Queue.Count--;
    }
  }
}