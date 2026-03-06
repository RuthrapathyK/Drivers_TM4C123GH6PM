#include "common.h"
#include "../src/UART/uart.h"
#include "../src/PinMux/pinconfig.h"
#include "../src/NVIC/nvic.h"
#include "../src/Queue/queue.h"

uint16_t UART_RX_QBuffer[20] = {0};
uint8_t UART_TX_QBuffer[20] = {0};

Queue_t UART_RX_QHandler;
Queue_t UART_TX_QHandler;

UART_config_t UART0_Handler;

void BoardPins_Init(void)
{
  Pin_Config(Port_PA, 0, PA0_U0RX);
  Pin_Config(Port_PA, 1, PA1_U0TX);
}
void main()
{
  /* Get Default Configurations*/
  UART_getDefaultConfig(&UART0_Handler);

  /* Init UART */
  UART_Init(UART_0, &UART0_Handler);

  /* Initialize the Queue for UART */
  Queue_Init(&UART_RX_QHandler, (uint8_t *)UART_RX_QBuffer, sizeof(UART_RX_QBuffer[0]), sizeof(UART_RX_QBuffer) / sizeof(UART_RX_QBuffer[0]));
  Queue_Init(&UART_TX_QHandler, (uint8_t *)UART_TX_QBuffer, sizeof(UART_TX_QBuffer[0]), sizeof(UART_TX_QBuffer) / sizeof(UART_TX_QBuffer[0]));

  /* Init the Pin Configurations */
  BoardPins_Init();

  /* Init Interrupts */
  NVIC_enableInterrupt(UART_0_IRQ);
  __enable_irq();

  while(1)
  {
      delayLoop(1000);
      UART_sendString_NonBlocking(&UART_TX_QHandler, "For large amounts of input, the linear access time of linked lists is prohibitive. In this chapter we look at a simple data structure for which the running time of most operations is O(log n ) on average. We also sketch a conceptually simple modification to this data structure that guarantees the above time bound in the worst case and discuss a second modification that essentially gives an O(log n ) running time per operation for a long sequence of instructions.\n");

      /* Sendback the Received Character */
      if(Queue_isEmpty(&UART_RX_QHandler) == Queue_NotEmpty)
      {
        /* Extract Data from Queue*/
        uint16_t received_char = 0;
        Queue_Dequeue(&UART_RX_QHandler, (uint8_t *)&received_char);

        /* Transmit the Received Character */
        UART_sendString_NonBlocking(&UART_TX_QHandler, (char *)&received_char);      
        
        if(Queue_getOverflow_State(&UART_RX_QHandler) == Queue_Overflow)
        {
          //Queue_fullFlush(&UART_RX_QHandler);
          Queue_setOverflow_State(&UART_RX_QHandler, Queue_NoOverflow);
          UART_sendString("\nOverFlow Occured\n");
        }

        /* Intentional Delay*/
        delayLoop(1000);
    }
  }
}