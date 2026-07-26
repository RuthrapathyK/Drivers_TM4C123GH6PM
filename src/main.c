#include "common.h"
#include "../src/UART/uart.h"
#include "../src/PinMux/pinconfig.h"
#include "../src/NVIC/nvic.h"
#include "../src/Queue/queue.h"
#include "../src/ADC/adc.h"
#include "../src/GPIO/gpio.h"
#include "../src/DMA/dma.h"
#include "../src/Timer/timer.h"
#include "../src/PWM/pwm.h"
#include "../src/Clock/clock.h"

uint16_t UART_RX_QBuffer[20] = {0};
uint8_t UART_TX_QBuffer[20] = {0};

Queue_t UART_RX_QHandler;
Queue_t UART_TX_QHandler;

UART_config_t UART0_Handler;

uint16_t adc_val[MAX_ADC_SAMPLE_SIZE] = {0};
volatile bool isTransferDone = false;

extern uint32_t volatile ProcessedSample_Count;

uint32_t test[3] = {0,};
/**
 * @brief Initializes the board pins for UART communication.
 *
 * Configures GPIO pins PA0 and PA1 as UART0 RX and TX pins respectively
 * by setting up the pin multiplexing for serial communication.
 */
void Pins_Init(void)
{
  /* Configure UART Pins */
  Pin_Config(Port_PA, 0, PA0_U0RX);
  Pin_Config(Port_PA, 1, PA1_U0TX);

  /* Configure ADC Pins */
  Pin_Config(Port_PD, 2, PD2_ANALOG_AIN5);

  /* Configure PWM Pins */
  Pin_Config(Port_PB, 6, PB6_M0PWM0);
}

/**
 * @brief Initializes all board peripherals.
 *
 * Configures and enables UART0, ADC0, DMA0 modules and GPIO test pin (Port F1).
 * Must be called during system initialization to set up core communication and
 * data acquisition interfaces.
 */
void Peripheral_Init(void)
{
  /* Init UART */
  UART_getDefaultConfig(&UART0_Handler);
  UART_Init(UART_0, &UART0_Handler);

  /* Init ADC */
//  ADC_Init(ADC_0);

  /* Init DMA */
//  DMA_Init(DMA_0);

  /* Init Timer */
  //TIM_Init();

  /* Init Test Pin */
  GPIO_Init(PF1, GPIO_DigitalOutput, GPIO_State_OFF);
}

/**
 * @brief Initializes board communication services.
 *
 * Creates and configures receive and transmit queues for UART0 communication.
 * These queues are used for non-blocking interrupt-driven UART data transfer.
 */
void Services_Init(void)
{
  /* Initialize the Queue for UART */
  Queue_Init(&UART_RX_QHandler, (uint8_t *)UART_RX_QBuffer, sizeof(UART_RX_QBuffer[0]), sizeof(UART_RX_QBuffer) / sizeof(UART_RX_QBuffer[0]));
  Queue_Init(&UART_TX_QHandler, (uint8_t *)UART_TX_QBuffer, sizeof(UART_TX_QBuffer[0]), sizeof(UART_TX_QBuffer) / sizeof(UART_TX_QBuffer[0]));
}

/**
 * @brief Initializes NVIC interrupt configuration.
 *
 * Disables global interrupts, enables UART0 and ADC0 sequence 0 interrupts in the NVIC,
 * then re-enables global interrupts. Must be called after peripheral initialization
 * to enable interrupt-driven peripheral operation.
 */
void Interrupt_Init(void)
{
  /* Disable Global Interrupt of the Processor */
  __disable_irq();

  /* Enable NVIC peripheral Interrupt */
  NVIC_enableInterrupt(UART_0_IRQ);
  NVIC_enableInterrupt(ADC_0_SEQ_0_IRQ);
  NVIC_enableInterrupt(TIMER_0A_IRQ);

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
  /* Init System Clock*/
  Clock_Init(Precise_OSC, SYSTEM_CLOCK_FREQ);

  /* Init Peripherals needed */
  Peripheral_Init();

  /* Init the Pin Configurations */
  Pins_Init();
 
  /* Init other Services */
  Services_Init();

  /* Init Interrupts */
  Interrupt_Init();
  
  while(1)
  {
    UART_sendString("Hello World\n");
    // GPIO_setPin(PF1);

    // /* Clear Counter */
    // isTransferDone = false;
    // ProcessedSample_Count = 0;

    // /* Flush the FIFO so that unread data will not cause Overflow */
    // ADC_FlushFIFO(ADC_SampleSequencer_0);

    // /* Enable DMA transfers */
    // DMA_EnableTransfer(DMA_0);

    // /* Enable PWM to Start the Conversion - Counter will not restart on every trigger */
    // PWM_Init();
    
    // /* Wait till Configured No of Conversions are completed */
    // while(!isTransferDone)
    // ;

    // /* Check any Underflow or Overflow has happened in the Sample collection */
    // ADC_SynchronizationCheck();
    
    // GPIO_clearPin(PF1);

    // for(uint32_t iter = 0; iter < MAX_ADC_SAMPLE_SIZE; iter++)
    // {
    //   UART_sendString_NonBlocking(&UART_TX_QHandler, "$$P-,");
    //   UART_sendNumber_NonBlocking(&UART_TX_QHandler, (int32_t)adc_val[iter]);
    //   UART_sendString_NonBlocking(&UART_TX_QHandler,";");
    //   adc_val[iter] = 0;
    // }
    delayLoop(1000);
  }

  return 0;
}