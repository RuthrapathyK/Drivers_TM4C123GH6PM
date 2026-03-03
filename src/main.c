#include "common.h"
#include "../src/UART/uart.h"
#include "../src/PinMux/pinconfig.h"
#include "../src/NVIC/nvic.h"
#include "../src/Queue/queue.h"

#define UART_MAX_QUEUE_COUNT 10

uint16_t UART_QBuffer[UART_MAX_QUEUE_COUNT] = {0};
Queue_t UART_QHandler;

void BoardPins_Init(void)
{
  Pin_Config(Port_PA, 0, PA0_U0RX);
  Pin_Config(Port_PA, 1, PA1_U0TX);
}
void main()
{

  /* Init UART */
  UART_Init(UART_0, 115200);

  /* Initialize the Queue for UART */
  Queue_Init(&UART_QHandler, (uint8_t *)UART_QBuffer, sizeof(UART_QBuffer[0]), sizeof(UART_QBuffer) / sizeof(UART_QBuffer[0]));

  /* Init the Pin Configurations */
  BoardPins_Init();

  /* Init Interrupts */
  NVIC_enableInterrupt(UART_0_IRQ);
  __enable_irq();

  while(1)
  {
    /* Sendback the Received Character */
    while(Queue_isEmpty(&UART_QHandler) == Queue_NotEmpty)
    {
      /* Extract Data from Queue*/
      uint16_t received_char = 0;
      Queue_Dequeue(&UART_QHandler, (uint8_t *)&received_char);

      /* Transmit the Received Character */
      UART_sendChar((received_char & 0xFF));      
      
      // if(Queue_getOverflow_State(&UART_QHandler) == Queue_Overflow)
      // {
      //   //Queue_fullFlush(&UART_QHandler);
      //   Queue_setOverflow_State(&UART_QHandler, Queue_NoOverflow);
      //   UART_sendString("\nOverFlow Occured\n");
      // }

      /* Intentional Delay*/
      delayLoop(1000);
    }
  }
}