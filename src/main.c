#include "common.h"
#include "pinconfig.h"
#include "uart.h"
#include "nvic.h"
#include "led.h"
#include "filter.h"

#define SAMPLE_SIZE 500
#define AVERAGE_SIZE 1

#define TESTPIN_HIGH LED_RED_ON
#define TESTPIN_LOW LED_RED_OFF

volatile uint16_t convData[SAMPLE_SIZE] = {0};
uint32_t volatile smp_idx = 0;
uint32_t volatile trg_cnt = 0;
uint32_t avg_accm = 0;

void ADC0_Sequence_3_handler(void)
{
  TESTPIN_HIGH;
  /* Clear the Interrupt */
  ADC0->ISC |= 1<<3;

  /* Increament the Trigger Counter */
  trg_cnt++;

  /* Check if the Total ADC trigger is within the range */
  if(trg_cnt < SAMPLE_SIZE * AVERAGE_SIZE)
  {
    /* Trigger Sampling in SS3*/
    ADC0->PSSI |= 1<<3;
  }

  if(trg_cnt % AVERAGE_SIZE == 0)
  {
    /* Accumulate the Current ADC raw reading */
    avg_accm += ADC0->SSFIFO3 & 0x0FFFu;

    /* Average and store it as a sample data */
    convData[smp_idx] = avg_accm / AVERAGE_SIZE;

    /* Increament the Samples Index */
    smp_idx++;

    /* Reset the accumulator */
    avg_accm = 0;
  }
  else
  {
    /* Accumulate the Current ADC raw reading */
    avg_accm += ADC0->SSFIFO3 & 0x0FFFu;
  }
  TESTPIN_LOW;
}
void main()
{
  UART_Init(115200);
  Pin_Config(Port_PA, 0, PA0_U0RX);
  Pin_Config(Port_PA, 1, PA1_U0TX);
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

    if(trg_cnt >= SAMPLE_SIZE * AVERAGE_SIZE)
    {
      /* Clear the Index value */
      trg_cnt = 0;
      smp_idx = 0;

      /* Send the Stored value through UART */
      for(uint32_t iter = 0; iter < SAMPLE_SIZE; iter++)
      {
        UART_sendString("$$P-auto,");
        UART_sendNumber(convData[iter]);
        UART_sendChar(';');
        
        // /* Send the Moving Average Filtered data */
        // UART_sendNumber(Filter_Moving_Average(convData[iter]));
        // UART_sendChar(';');
      }

      /* Trigger Sampling in SS3*/
      ADC0->PSSI |= 1<<3;
    }

  }
}