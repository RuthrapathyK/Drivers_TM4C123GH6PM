#include "common.h"
#include "../src/UART/uart.h"
#include "../src/PinMux/pinconfig.h"
#include "../src/NVIC/nvic.h"
#include "../src/Queue/queue.h"
#include "../src/ADC/adc.h"
#include "../src/GPIO/gpio.h"
#include "../src/DMA/dma.h"

uint16_t UART_RX_QBuffer[20] = {0};
uint8_t UART_TX_QBuffer[20] = {0};

Queue_t UART_RX_QHandler;
Queue_t UART_TX_QHandler;

UART_config_t UART0_Handler;

uint16_t adc_val[MAX_ADC_SAMPLE_SIZE] = {0};
volatile bool isTransferDone = false;

/**
 * @brief Initializes the board pins for UART communication.
 *
 * Configures GPIO pins PA0 and PA1 as UART0 RX and TX pins respectively
 * by setting up the pin multiplexing for serial communication.
 */
void BoardPins_Init(void)
{
  /* Configure UART Pins */
  Pin_Config(Port_PA, 0, PA0_U0RX);
  Pin_Config(Port_PA, 1, PA1_U0TX);

  /* Configure ADC Pins */
  Pin_Config(Port_PD, 2, PD2_ANALOG_AIN5);
}

void BoardPeripheral_Init(void)
{
  /* Init UART */
  UART_getDefaultConfig(&UART0_Handler);
  UART_Init(UART_0, &UART0_Handler);

  /* Init ADC */
  ADC_Init(ADC_0);

  /* Init DMA */
  DMA_Init(DMA_0);

  /* Init Test Pin */
  GPIO_Init(PF1, GPIO_DigitalOutput, GPIO_State_OFF);
}

void BoardServices_Init(void)
{
  /* Initialize the Queue for UART */
  Queue_Init(&UART_RX_QHandler, (uint8_t *)UART_RX_QBuffer, sizeof(UART_RX_QBuffer[0]), sizeof(UART_RX_QBuffer) / sizeof(UART_RX_QBuffer[0]));
  Queue_Init(&UART_TX_QHandler, (uint8_t *)UART_TX_QBuffer, sizeof(UART_TX_QBuffer[0]), sizeof(UART_TX_QBuffer) / sizeof(UART_TX_QBuffer[0]));
}

void BoardInterrupt_Init(void)
{
  /* Disable Global Interrupt of the Processor */
  __disable_irq();

  /* Enable NVIC peripheral Interrupt */
  NVIC_enableInterrupt(UART_0_IRQ);
  NVIC_enableInterrupt(ADC_0_SEQ_0_IRQ);

  /* Enable Global Interrupt of the Processor */
  __enable_irq();
}
/**
 * @brief Main entry point of the application.
 *
 * Initializes UART communication, configures the receive and transmit queues,
 * sets up GPIO pins and interrupts, then enters an infinite loop that receives
 * characters from UART, echoes them back, and monitors for queue overflow conditions.
 */
int main()
{
  /* Init Peripherals needed */
  BoardPeripheral_Init();

  /* Init the Pin Configurations */
  BoardPins_Init();
 
  /* Init other Services */
  BoardServices_Init();

  /* Init Interrupts */
  BoardInterrupt_Init();

  while(1)
  {
    GPIO_setPin(PF1);

    /* Clear Counter */
    isTransferDone = false;

    /* Enable DMA transfers */
    DMA_EnableTransfer(DMA_0);

    /* Enable SS0 Sample Sequencer to start Continuos Conversion */
    ADC0->ACTSS = 1;
    
    /* Wait till Configured No of Conversions are completed */
    while(!isTransferDone)
    ;

    /* Check for any Overflow/Underflow conditions */
    if(RegRead_Bits(&ADC0->OSTAT, 0, 1))
    {
      ASSERT(0);
    }
    else if(RegRead_Bits(&ADC0->USTAT, 0, 1))
    {
     ASSERT(0);
    }
    
    /* Flush the ADC FIFO */
    while(!RegRead_Bits(&ADC0->SSFSTAT0, 8, 1))
    {
      uint32_t temp = ADC0->SSFIFO0;
    }
    GPIO_clearPin(PF1);

    for(uint32_t iter = 0; iter < MAX_ADC_SAMPLE_SIZE; iter++)
    {
      UART_sendString_NonBlocking(&UART_TX_QHandler, "$$P-,");
      UART_sendNumber_NonBlocking(&UART_TX_QHandler, (int32_t)adc_val[iter]);
      UART_sendString_NonBlocking(&UART_TX_QHandler,";");
    }
    delayLoop(1000);
  }

  return 0;
}