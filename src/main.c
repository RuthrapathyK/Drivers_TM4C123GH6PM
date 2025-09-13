#include "common.h"
#include "pinconfig.h"
#include "uart.h"
#include "nvic.h"
#include "led.h"

#define SAMPLE_SIZE 2000

volatile uint32_t convData[SAMPLE_SIZE] = {0};

uint32_t volatile idx = 0;

void ADC0_Sequence_3_handler(void)
{
  /* Toggle Debug LED */
  LED_RED_TOGGLE;

  /* Clear the Interrupt */
  ADC0->ISC |= 1<<3;

  /* Read the ADC data*/
  convData[idx++] = (ADC0->SSFIFO3 & 0x0FFF);

  // /* Trigger Sampling in SS3*/
  // ADC0->PSSI |= 1<<3;

  // UART_sendNumber(convData[idx]);
  // UART_sendChar('\n');

  if(idx < SAMPLE_SIZE)
  {
    /* Trigger Sampling in SS3*/
    ADC0->PSSI |= 1<<3;
  }

}
void main()
{
  UART_Init(115200);
  Pin_Config(Port_PA, 0, PA0_U0RX);
  Pin_Config(Port_PA, 1, PA1_U0TX);

  UART_sendString("Hello World");

  LED_Init(LED_RED);

  /* Enable ADC0 clock */
  SYSCTL->RCGCADC |= 1<<0;

  /* Configure ADC Pin - PE3 as Analog */
  Pin_Config(Port_PE, 3, PE3_ANALOG_AIN0);

  /* Disable SS3 Sample Sequencer */
  ADC0->ACTSS &= ~(1<<3);
  
  /* Configure Trigger Event for the sample sequencer - Processor */
  ADC0->EMUX &= ~(0xF << 12);

  /* Configure Input Source for the Sample Sequencer - AIN0 */
  ADC0->SSMUX3 &= ~(0xF <<0);

  /* Enable Interrupt of SS3 */
  ADC0->IM |= 1<<3;
  ADC0->SSCTL3 |= 1<<2;

  /* Configure Sample Control Bits - Set End of Sequence bit */
  ADC0->SSCTL3 |= 1<<1;

  /* Enable The Sample Sequencer - SS3*/
  ADC0->ACTSS |= (1<<3);

  /* Enable NVIC Interrupt for ADC0 - SS3 */
  NVIC_enableInterrupt(ADC_0_SEQ_3_IRQ);

  /* Trigger Sampling in SS3*/
  ADC0->PSSI |= 1<<3;

  while(1)
  {

    if(idx >= SAMPLE_SIZE)
    {
      LED_RED_OFF;
      for(uint32_t iter = 0; iter < SAMPLE_SIZE; iter++)
      {
        UART_sendNumber(convData[iter]);
        UART_sendChar('\n');
      }
      idx = 0;

      /* Trigger Sampling in SS3*/
      ADC0->PSSI |= 1<<3;
    }

  }
}