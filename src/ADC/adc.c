#include "adc.h"

extern uint16_t adc_val[MAX_ADC_SAMPLE_SIZE];
extern volatile bool isTransferDone;

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

void ADC0_Sequence_0_handler(void)
{
    /* Disable SS0 Sample Sequencer to stop Continuos Conversion */
    ADC0->ACTSS = 0; // Direct Register write is needed to achieve the Maximum Sampling Rate 

    /* Clear Interrupt */
    ADC0->ISC = 1; // Direct Register write is needed to achieve the Maximum Sampling Rate

    isTransferDone = true;
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
    RegWrite_Bits(&adc_base->PC, ADC_SampleRate_1000ksps, 0, 4);

    /* Enable Dither */
    RegWrite_Bits(&adc_base->CTL, 1, 6, 1);

    /* Set Hardware Averaging */
    RegWrite_Bits(&adc_base->SAC, ADC_HWAveraging_Disabled, 0, 4);

    /* Configure Sample Sequencer Priorities */
    RegWrite_Bits(&adc_base->SSPRI, ADC_SSPriority_0, 0, 2);

    /* Disable Sample Sequencer */
    RegWrite_Bits(&adc_base->ACTSS, 0, 0, 1);

    /* Configure Trigger Event for SS0 as Processor */
    RegWrite_Bits(&adc_base->EMUX, ADC_TriggerSelect_Always, 0, 4);

    /* Configure the No. of Samples to be 8 */
    RegWrite_Bits(&adc_base->SSCTL0, 1, 29, 1);

    /* Enable Interrupt for Samples of SS0 */
    RegWrite_Bits(&adc_base->SSCTL0, 1, 14, 1); // 4th Sample 
    RegWrite_Bits(&adc_base->SSCTL0, 1, 30, 1); // 8th Sample    

    /* Configure Trigger Source pin for SS0 samples */
    RegWrite_Bits(&adc_base->SSMUX0, ADC_SampleInput_AIN5, 0, 4); // 1st Sample
    RegWrite_Bits(&adc_base->SSMUX0, ADC_SampleInput_AIN5, 4, 4); // 2nd Sample
    RegWrite_Bits(&adc_base->SSMUX0, ADC_SampleInput_AIN5, 8, 4); // 3rd Sample
    RegWrite_Bits(&adc_base->SSMUX0, ADC_SampleInput_AIN5, 12, 4); // 4th Sample
    RegWrite_Bits(&adc_base->SSMUX0, ADC_SampleInput_AIN5, 16, 4); // 5th Sample
    RegWrite_Bits(&adc_base->SSMUX0, ADC_SampleInput_AIN5, 20, 4); // 6th Sample
    RegWrite_Bits(&adc_base->SSMUX0, ADC_SampleInput_AIN5, 24, 4); // 7th Sample
    RegWrite_Bits(&adc_base->SSMUX0, ADC_SampleInput_AIN5, 28, 4); // 8th Sample

    /* Disable ADC Interrupt Mask as DMA will generate Interrupt of this ADC peripheral */
    RegWrite_Bits(&adc_base->IM, 0, 0, 1);
}

uint16_t ADC_ReadRaw(ADC_Module_e mod)
{
    /* Get the Base Address of the ADC Module */
    ADC0_Type *adc_base = ADC_getBase(mod);

    /* Trigger SS0 in ADC module */
    RegWrite_Bits(&adc_base->PSSI, 1, 0, 1);

    /* Read the Status of Busy Bit */
    while(RegRead_Bits(&adc_base->SSFSTAT0, 8, 1))
    ;

    /* Read the Conversion Results and return */
    return RegRead_Bits(&adc_base->SSFIFO0, 0, 12);
}

void ADC_TriggerConversion(ADC_Module_e mod)
{
    /* Get the Base Address of the ADC Module */
    ADC0_Type *adc_base = ADC_getBase(mod);

    /* Trigger SS0 in ADC module */
    RegWrite_Bits(&adc_base->PSSI, 1, 0, 1);   
}