#include "adc.h"
#include "../src/DMA/dma.h"
#include "../src/UART/uart.h"

extern uint16_t adc0_val[MAX_ADC_SAMPLE_SIZE];
extern uint16_t adc1_val[MAX_ADC_SAMPLE_SIZE];
extern volatile bool isTransferDone;
extern DMA_ControlWord_t ControlWord;
extern DMA_ChannelControl_t Channel_Control_Table[DMA_ChannelControl_Max][DMA_Channel_Max];
uint32_t volatile ADC0_SmpCnt = 0;
uint32_t volatile ADC1_SmpCnt = 0;

/**
 * @brief Gets the base address of the specified ADC module.
 *
 * Returns the memory base address for the given ADC module for register access.
 *
 * @param mod The ADC module identifier
 * @return Pointer to the ADC module's base address
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

/**
 * @brief Interrupt handler for ADC0 Sample Sequencer 0.
 *
 * Clears the ADC interrupt, increments the sample count by 1024 samples,
 * and disables continuous sampler when maximum sample size is reached.
 * Additionally manages DMA channel reconfiguration for ping-pong buffering
 * at intermediate sample count milestones to support continuous acquisition.
 */
void ADC0_Sequence_0_handler(void)
{
    /* Clear Interrupt */
    REG_SET_BIT(ADC0->ISC, 0); // Direct Register write is needed to achieve the Maximum Sampling Rate

    if(ADC0_SmpCnt >= MAX_ADC_SAMPLE_SIZE)
    {
        if((Channel_Control_Table[DMA_ChannelControl_Primary][DMA_Channel_14].Control_Word.XFERSIZE == 0) && (Channel_Control_Table[DMA_ChannelControl_Secondary][DMA_Channel_14].Control_Word.XFERSIZE == 0))
        {
            #ifdef PWM_TRIGGER_ENABLE
                /* Disable PWM to Stop the Conversion */
                REG_CLEAR_BIT(PWM0->_0_CTL, 0); // Direct Register write is needed to achieve the Maximum Sampling Rate 
            #endif
            #ifdef CONTINUOUS_TRIGGER_ENABLE
                /* Disable ADC to Stop the Conversion */
                REG_CLEAR_BIT(ADC0->ACTSS, 0); // Direct Register write is needed to achieve the Maximum Sampling Rate 
            #endif
        }
        else
        {
            /* Wait till all the Scheduled DMA transfers are completed */
        }
    }
    else
    {
        if((MAX_ADC_SAMPLE_SIZE - ADC0_SmpCnt) > DMA_MAX_XFER_COUNT)
        {
            ControlWord.XFERSIZE = DMA_MAX_XFER_COUNT - 1;

            if(Channel_Control_Table[DMA_ChannelControl_Primary][DMA_Channel_14].Control_Word.XFERSIZE == 0)
                DMA_ChannelConfig(DMA_ChannelControl_Primary, DMA_Channel_14, (uint32_t *)&ADC0->SSFIFO0, (uint32_t *)&adc0_val[ADC0_SmpCnt + ControlWord.XFERSIZE], ControlWord);
            else if(Channel_Control_Table[DMA_ChannelControl_Secondary][DMA_Channel_14].Control_Word.XFERSIZE == 0)
                DMA_ChannelConfig(DMA_ChannelControl_Secondary, DMA_Channel_14, (uint32_t *)&ADC0->SSFIFO0, (uint32_t *)&adc0_val[ADC0_SmpCnt + ControlWord.XFERSIZE], ControlWord);

            ADC0_SmpCnt += DMA_MAX_XFER_COUNT;
        }
        else
        {
            ControlWord.XFERSIZE = MAX_ADC_SAMPLE_SIZE - ADC0_SmpCnt - 1;

            if(Channel_Control_Table[DMA_ChannelControl_Primary][DMA_Channel_14].Control_Word.XFERSIZE == 0)
                DMA_ChannelConfig(DMA_ChannelControl_Primary, DMA_Channel_14, (uint32_t *)&ADC0->SSFIFO0, (uint32_t *)&adc0_val[ADC0_SmpCnt + ControlWord.XFERSIZE], ControlWord);
            else if(Channel_Control_Table[DMA_ChannelControl_Secondary][DMA_Channel_14].Control_Word.XFERSIZE == 0)
                DMA_ChannelConfig(DMA_ChannelControl_Secondary, DMA_Channel_14, (uint32_t *)&ADC0->SSFIFO0, (uint32_t *)&adc0_val[ADC0_SmpCnt + ControlWord.XFERSIZE], ControlWord);

            ADC0_SmpCnt += (ControlWord.XFERSIZE + 1);  
        }
    }
}

void ADC1_Sequence_0_handler(void)
{
    /* Clear Interrupt */
    REG_SET_BIT(ADC1->ISC, 0); // Direct Register write is needed to achieve the Maximum Sampling Rate

    if(ADC1_SmpCnt >= MAX_ADC_SAMPLE_SIZE)
    {
        if((Channel_Control_Table[DMA_ChannelControl_Primary][DMA_Channel_24].Control_Word.XFERSIZE == 0) && (Channel_Control_Table[DMA_ChannelControl_Secondary][DMA_Channel_24].Control_Word.XFERSIZE == 0))
        {
            isTransferDone = true;
        }
        else
        {
            /* Wait till all the Scheduled DMA transfers are completed */
        }
    }
    else
    {
        if((MAX_ADC_SAMPLE_SIZE - ADC1_SmpCnt) > DMA_MAX_XFER_COUNT)
        {
            ControlWord.XFERSIZE = DMA_MAX_XFER_COUNT - 1;

            if(Channel_Control_Table[DMA_ChannelControl_Primary][DMA_Channel_24].Control_Word.XFERSIZE == 0)
                DMA_ChannelConfig(DMA_ChannelControl_Primary, DMA_Channel_24, (uint32_t *)&ADC1->SSFIFO0, (uint32_t *)&adc1_val[ADC1_SmpCnt + ControlWord.XFERSIZE], ControlWord);
            else if(Channel_Control_Table[DMA_ChannelControl_Secondary][DMA_Channel_24].Control_Word.XFERSIZE == 0)
                DMA_ChannelConfig(DMA_ChannelControl_Secondary, DMA_Channel_24, (uint32_t *)&ADC1->SSFIFO0, (uint32_t *)&adc1_val[ADC1_SmpCnt + ControlWord.XFERSIZE], ControlWord);

            ADC1_SmpCnt += DMA_MAX_XFER_COUNT;
        }
        else
        {
            ControlWord.XFERSIZE = MAX_ADC_SAMPLE_SIZE - ADC1_SmpCnt - 1;

            if(Channel_Control_Table[DMA_ChannelControl_Primary][DMA_Channel_24].Control_Word.XFERSIZE == 0)
                DMA_ChannelConfig(DMA_ChannelControl_Primary, DMA_Channel_24, (uint32_t *)&ADC1->SSFIFO0, (uint32_t *)&adc1_val[ADC1_SmpCnt + ControlWord.XFERSIZE], ControlWord);
            else if(Channel_Control_Table[DMA_ChannelControl_Secondary][DMA_Channel_24].Control_Word.XFERSIZE == 0)
                DMA_ChannelConfig(DMA_ChannelControl_Secondary, DMA_Channel_24, (uint32_t *)&ADC1->SSFIFO0, (uint32_t *)&adc1_val[ADC1_SmpCnt + ControlWord.XFERSIZE], ControlWord);

            ADC1_SmpCnt += (ControlWord.XFERSIZE + 1);  
        }
    }
}

/**
 * @brief Initializes the ADC module with default configuration.
 *
 * Configures the specified ADC module with clock settings (125 ksps sampling rate),
 * sample sequencer 0 with 8 samples from AIN5 input, processor trigger mode,
 * hardware averaging disabled, and dither enabled. Disables interrupt masking
 * for DMA-driven operation. Must be called before triggering conversions.
 *
 * @param mod The ADC module to initialize
 */
void ADC_Init(ADC_Module_e mod, ADC_PhaseLag_e phase)
{
    /* Reset the ADC module */
    RegWrite_Bits_ASSERT(&SYSCTL->SRADC, 1, mod, 1);
    RegWrite_Bits_ASSERT(&SYSCTL->SRADC, 0, mod, 1);

    /* Enable Clock for ADC module */
    RegWrite_Bits_ASSERT(&SYSCTL->RCGCADC, 1, mod, 1);

    /* Wait till ADC module is Enabled */
    while(!RegRead_Bits(&SYSCTL->PRADC, mod, 1))
    ;

    /* Get the Base Address of the ADC Module */
    ADC0_Type *adc_base = ADC_getBase(mod);

    /* Configure Clock Source for ADC Module */
    REG_WRITE(adc_base->CC, ADC_ClockSource_Either, 0, 4);
    
    /* Configure Sampling Rate of the ADC Module */
    REG_WRITE(adc_base->PC, ADC_SampleRate_1000ksps, 0, 4);

    /* Enable Dither */
    REG_WRITE(adc_base->CTL, ADC_Dither_Enable, 6, 1);

    /* Set Hardware Averaging */
    REG_WRITE(adc_base->SAC, ADC_HWAveraging_Disabled, 0, 4);

    /* Configure Sample Sequencer Priorities */
    REG_WRITE(adc_base->SSPRI, ADC_SSPriority_0, 0, 2);

    /* Disable Sample Sequencer */
    REG_WRITE(adc_base->ACTSS, 0, 0, 1);
#ifdef PWM_TRIGGER_ENABLE
    /* Configure Trigger Event for SS0 as General Purpose Timer */
    REG_WRITE(adc_base->EMUX, ADC_TriggerSelect_PWM_0, 0, 4);
#endif
#ifdef CONTINUOUS_TRIGGER_ENABLE
    /* Configure Trigger Event Continuous */
    REG_WRITE(adc_base->EMUX, ADC_TriggerSelect_Always, 0, 4);
#endif
    /* Configure the No. of Samples to be 8 */
    REG_WRITE(adc_base->SSCTL0, 1, 29, 1);

    /* Enable Interrupt for Samples of SS0 */
    REG_WRITE(adc_base->SSCTL0, 1, 14, 1); // 4th Sample 
    REG_WRITE(adc_base->SSCTL0, 1, 30, 1); // 8th Sample    

    /* Configure Trigger Source pin for SS0 samples */
    REG_WRITE(adc_base->SSMUX0, ADC_SampleInput_AIN5, 0, 4); // 1st Sample
    REG_WRITE(adc_base->SSMUX0, ADC_SampleInput_AIN5, 4, 4); // 2nd Sample
    REG_WRITE(adc_base->SSMUX0, ADC_SampleInput_AIN5, 8, 4); // 3rd Sample
    REG_WRITE(adc_base->SSMUX0, ADC_SampleInput_AIN5, 12, 4); // 4th Sample
    REG_WRITE(adc_base->SSMUX0, ADC_SampleInput_AIN5, 16, 4); // 5th Sample
    REG_WRITE(adc_base->SSMUX0, ADC_SampleInput_AIN5, 20, 4); // 6th Sample
    REG_WRITE(adc_base->SSMUX0, ADC_SampleInput_AIN5, 24, 4); // 7th Sample
    REG_WRITE(adc_base->SSMUX0, ADC_SampleInput_AIN5, 28, 4); // 8th Sample

    /* Configure the Phase Lag from the Trigger */
    RegWrite_Bits(&adc_base->SPC, phase, 0, 4);

    /* Disable ADC Interrupt Mask as DMA will generate Interrupt of this ADC peripheral */
    REG_WRITE(adc_base->IM, 0, 0, 1);

#ifdef PWM_TRIGGER_ENABLE
    /* Enable SS0 Sample Sequencer to Wait for PWM to Trigger the Conversion */
    REG_WRITE(adc_base->ACTSS, 1, 0, 1);
#endif

#ifdef CONTINUOUS_TRIGGER_ENABLE
    /* Disable SS0 Sample Sequencer */
    REG_WRITE(adc_base->ACTSS, 0, 0, 1);
#endif
}

/**
 * @brief Reads raw 12-bit ADC conversion result in blocking mode.
 *
 * Triggers ADC sample sequencer 0 conversion and waits until the conversion
 * completes. Returns the raw 12-bit conversion result from the ADC FIFO.
 *
 * @param mod The ADC module to read from
 * @return Raw 12-bit ADC conversion value (0-4095)
 */
uint16_t ADC_ReadRaw(ADC_Module_e mod)
{
    /* Get the Base Address of the ADC Module */
    ADC0_Type *adc_base = ADC_getBase(mod);

    /* Trigger SS0 in ADC module */
    REG_WRITE(adc_base->PSSI, 1, 0, 1);

    /* Read the Status of Busy Bit */
    while(RegRead_Bits(&adc_base->SSFSTAT0, 8, 1))
    ;

    /* Read the Conversion Results and return */
    return RegRead_Bits(&adc_base->SSFIFO0, 0, 12);
}

/**
 * @brief Triggers ADC sample sequencer 0 conversion.
 *
 * Non-blocking trigger of ADC sample sequencer 0. Used in DMA-driven continuous
 * conversion mode to initiate conversions that transfer data via DMA instead of
 * polling. The completion can be monitored via the interrupt handler.
 *
 * @param mod The ADC module to trigger
 */
void ADC_TriggerConversion(ADC_Module_e mod)
{
    /* Get the Base Address of the ADC Module */
    ADC0_Type *adc_base = ADC_getBase(mod);

    /* Trigger SS0 in ADC module */
    REG_WRITE(adc_base->PSSI, 1, 0, 1);   
}

/**
 * @brief Checks for ADC overflow and underflow conditions.
 *
 * Verifies ADC0 operation for any overflow or underflow errors that may indicate
 * data loss or synchronization issues. Triggers an assertion if either condition
 * is detected, halting execution for debugging purposes.
 */
void ADC_SynchronizationCheck(void)
{  
    /* Check for any Overflow */
    if(RegRead_Bits(&ADC0->OSTAT, 0, 1))
    {
      ASSERT(0);
    }

    /* Underflow conditions */
    if(RegRead_Bits(&ADC0->USTAT, 0, 1))
    {
     ASSERT(0);
    }
}

/**
 * @brief Flushes the FIFO of the specified ADC sample sequencer.
 *
 * Empties the FIFO buffer for the specified sample sequencer by reading and
 * discarding all pending data until the FIFO is empty. Useful for clearing
 * stale data before starting new conversions or after error conditions.
 *
 * @param seq The ADC sample sequencer FIFO to flush
 */
void ADC_FlushFIFO(ADC_SampleSequencer_e seq)
{
    volatile uint32_t *reg_stat = 0;
    uint32_t temp = 0;

    /* Get the Address of Sample Sequencer Status Register */
    switch(seq)
    {
        case ADC_SampleSequencer_0:
            reg_stat = &ADC0->SSFSTAT0;
            break;
        case ADC_SampleSequencer_1:
            reg_stat = &ADC0->SSFSTAT1;
            break;
        case ADC_SampleSequencer_2:
            reg_stat = &ADC0->SSFSTAT2;
            break;
        case ADC_SampleSequencer_3:
            reg_stat = &ADC0->SSFSTAT3;
            break;
        default:
            ASSERT(0);
            break;
    }

    /* Flush the ADC FIFO */
    while(!RegRead_Bits(reg_stat, 8, 1))
    {
      temp = ADC0->SSFIFO0;
    }

    /* Unused Variable */
    (void)temp;
}