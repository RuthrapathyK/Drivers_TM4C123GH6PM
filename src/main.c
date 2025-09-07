#include "common.h"
#include "pinconfig.h"
#include "uart.h"
#include "nvic.h"

volatile uint32_t convData = 0;

void main()
{

  UART_Init(115200);
  Pin_Config(Port_PA, 0, PA0_U0RX);
  Pin_Config(Port_PA, 1, PA1_U0TX);
  
  UART_sendString("Hello World\n");

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

  /* Configure Sample Control Bits - Set End of Sequence bit */
  ADC0->SSCTL3 |= 1<<1;

  /* Enable The Sample Sequencer - SS3*/
  ADC0->ACTSS |= (1<<3);

  while(1)
  {
    /* Trigger Sampling in SS3*/
    ADC0->PSSI |= 1<<3;

    /* Wait till ADC conversion is completed */
    while(((ADC0->ACTSS >> 16) & 0x01))
    ;

    /* Read the ADC data*/
    convData = (ADC0->SSFIFO3 & 0x0FFF);

    UART_sendNumber(convData);
    UART_sendChar('\n');
  }
}