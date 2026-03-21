#include "adc.h"


extern uint32_t SampleCount;
extern uint16_t adc_val[MAX_ADC_SAMPLE_SIZE];

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
    /* Clear Interrupt */
    //RegWrite_Bits(&ADC0->ISC, 1, 0, 1);
    ADC0->ISC = 1;
    /* Do next ADC trigger only when needed */
    if(SampleCount < MAX_ADC_SAMPLE_SIZE - 1)
    {
        /* Trigger SS0 in ADC module */
        //RegWrite_Bits(&ADC0->PSSI, 1, 0, 1); 
        ADC0->PSSI = 1;
    }

    /* Read Converted Data */
    //adc_val[SampleCount] = RegRead_Bits(&ADC0->SSFIFO0, 0, 12);
    adc_val[SampleCount] = ADC0->SSFIFO0;
    adc_val[SampleCount + 1] = ADC0->SSFIFO0;
    adc_val[SampleCount + 2] = ADC0->SSFIFO0;
    adc_val[SampleCount + 3] = ADC0->SSFIFO0;
    adc_val[SampleCount + 4] = ADC0->SSFIFO0;
    adc_val[SampleCount + 5] = ADC0->SSFIFO0;
    adc_val[SampleCount + 6] = ADC0->SSFIFO0;
    adc_val[SampleCount + 7] = ADC0->SSFIFO0;

    /* Increment the Counter */
    SampleCount += 8;
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

    /* Enable Dither */
    RegWrite_Bits(&adc_base->CTL, 1, 6, 1);

    /* Set Hardware Averaging */
    RegWrite_Bits(&adc_base->SAC, ADC_HWAveraging_Disabled, 0, 4);

    /* Configure Sample Sequencer Priorities */
    RegWrite_Bits(&adc_base->SSPRI, ADC_SSPriority_0, 0, 2);

    /* Disable Sample Sequencer */
    RegWrite_Bits(&adc_base->ACTSS, 0, 0, 1);

    /* Configure Trigger Event for SS0 as Processor */
    RegWrite_Bits(&adc_base->EMUX, ADC_TriggerSelect_Processor, 0, 4);

    /* Configure the No. of Samples to be 8 */
    RegWrite_Bits(&adc_base->SSCTL0, 1, 29, 1);

    /* Enable Interrupt for Sample 1 of SS0 */
    RegWrite_Bits(&adc_base->SSCTL0, 1, 30, 1);

    /* Configure Trigger Source pin for SS0 as AIN0 for 1st Sample */
    RegWrite_Bits(&adc_base->SSMUX0, ADC_SampleInput_AIN0, 0, 4);
    RegWrite_Bits(&adc_base->SSMUX0, ADC_SampleInput_AIN0, 4, 4); // 2nd Sample
    RegWrite_Bits(&adc_base->SSMUX0, ADC_SampleInput_AIN0, 8, 4); // 3rd Sample
    RegWrite_Bits(&adc_base->SSMUX0, ADC_SampleInput_AIN0, 12, 4); // 4th Sample
    RegWrite_Bits(&adc_base->SSMUX0, ADC_SampleInput_AIN0, 16, 4); // 5th Sample
    RegWrite_Bits(&adc_base->SSMUX0, ADC_SampleInput_AIN0, 20, 4); // 6th Sample
    RegWrite_Bits(&adc_base->SSMUX0, ADC_SampleInput_AIN0, 24, 4); // 7th Sample
    RegWrite_Bits(&adc_base->SSMUX0, ADC_SampleInput_AIN0, 28, 4); // 8th Sample

    /* Configure ADC Interrupt Mask */
    RegWrite_Bits(&adc_base->IM, 1, 0, 1);

    /* Enable Sample Sequencer 0*/
    RegWrite_Bits(&adc_base->ACTSS, 1, 0, 1);
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