#include "adc.h"

/**
 * @brief Gets the base address of the specified UART module.
 *
 * Returns the memory base address for the given UART module for register access.
 *
 * @param mod The UART module identifier
 * @return Pointer to the UART module's base address
 */
static ADC0_Type* ADC_getBase(ADC_Module_e mod)
{
  /* Check Preconditions */
  ASSERT(mod < ADC_Module_Max);

  ADC0_Type * retval = 0;

  /* Get Base Address of the corresponding ADC module */
  switch(mod)
  {
    case ADC_0:
        retval = (ADC0_Type *)ADC0_BASE;
        break;
    case ADC_1:
        retval = (ADC0_Type *)ADC1_BASE;
        break;
    default:
        ASSERT(0);
  }

  return retval;
}


void ADC_Init(ADC_Module_e mod)
{
    /* Reset the ADC module */
    RegWrite_Bits(&SYSCTL->SRADC, 1, mod, 1);
    RegWrite_Bits(&SYSCTL->SRADC, 0, mod, 1);

    /* Enable Clock for UART0 module */
    RegWrite_Bits(&SYSCTL->RCGCADC, 1, mod, 1);

    /* Wait till UART module is Enabled */
    while(!RegRead_Bits(&SYSCTL->PRADC, mod, 1))
    ;

    /* Get the Base Address of the ADC Module */
    ADC0_Type *adc_base = ADC_getBase(mod);

    /* Configure Clock Source for ADC Module */
    RegWrite_Bits(&adc_base->CC, ADC_ClockSource_Either, 0, 4);
    
    /* Configure Sampling Rate of the ADC Module */
    RegWrite_Bits(&adc_base->PC, ADC_SampleRate_125ksps, 0, 4);

    /* Configure Sample Sequencer Priorities */

    /* Configure Sample Sequencer */

}