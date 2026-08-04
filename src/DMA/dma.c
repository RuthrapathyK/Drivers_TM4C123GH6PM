#include "dma.h"
#include "../src/ADC/adc.h"

__attribute__((aligned(1024))) DMA_ChannelControl_t Channel_Control_Table[DMA_ChannelControl_Max][DMA_Channel_Max] = {0};

extern uint32_t volatile ADC0_SmpCnt;
extern uint32_t volatile ADC1_SmpCnt;
extern uint16_t adc0_val[MAX_ADC_SAMPLE_SIZE];
extern uint16_t adc1_val[MAX_ADC_SAMPLE_SIZE];
DMA_ControlWord_t ControlWord;

/**
 * @brief Gets the base address of the specified DMA module.
 *
 * Returns the memory base address for the given DMA module for register access.
 * Currently only DMA_0 is supported on this microcontroller.
 *
 * @param mod The DMA module identifier
 * @return Pointer to the DMA (UDMA) module's base address
 */
static UDMA_Type * DMA_getBase(DMA_Module_e mod)
{
    /* Check the Preconditions */
    ASSERT(mod < DMA_Max);

    UDMA_Type * retval = 0;

    /* Derive Base Address based on the Module Selected */
    switch (mod)
    {
        case DMA_0:
            retval = (UDMA_Type *)UDMA_BASE;
            break;
        
        default:
            ASSERT(0);
            break;
    }

    return retval;
}

void DMA_ChannelConfig(DMA_ChannelControl_e channelcontrol, DMA_Channel_e channel, uint32_t *srcPtr, uint32_t *dstPtr, DMA_ControlWord_t cntWord)
{
    Channel_Control_Table[channelcontrol][channel].Source_End_Pointer = srcPtr;
    Channel_Control_Table[channelcontrol][channel].Destination_End_Pointer = dstPtr;
    Channel_Control_Table[channelcontrol][channel].Control_Word = cntWord;
}

void DMA_Init(DMA_Module_e mod)
{
    /* Check the Preconditions */
    ASSERT(mod < DMA_Max);

    /* Reset DMA controller */
    RegWrite_Bits_ASSERT(&SYSCTL->SRDMA, 1, mod, 1);
    RegWrite_Bits_ASSERT(&SYSCTL->SRDMA, 0, mod, 1);

    /* Enable Clock for DMA controller */
    RegWrite_Bits_ASSERT(&SYSCTL->RCGCDMA, 1, mod, 1);

    /* Wait till DMA controller is Enabled */
    while(!RegRead_Bits(&SYSCTL->PRDMA, mod, 1))
    ;

    /* Get Base Address of DMA controller */
    UDMA_Type *dma_base = DMA_getBase(DMA_0);

    /* Enable DMA controller */
    REG_WRITE(dma_base->CFG, 1, 0, 1);
    
    /* Configure Base Address of Channel Control Table */
    RegWrite_Bits_ASSERT(&dma_base->CTLBASE, ((uint32_t)&Channel_Control_Table) >> 10, 10, 22);

    /* Disable the Channel */
    REG_WRITE(dma_base->ENACLR, 1, 14, 1); //Channel 14
    REG_WRITE(dma_base->ENACLR, 1, 24, 1); //Channel 24

    /* Configure the Channel Mapping */
    REG_WRITE(dma_base->CHMAP1, DMA_CH14_ENC0_ADC0SS0, 24, 4); // for ADC0 SS0 (i.e. Channel 14)
    REG_WRITE(dma_base->CHMAP3, DMA_CH24_ENC1_ADC1SS0, 0, 4); // for ADC0 SS0 (i.e. Channel 24)

    /* Configure the Channel Priority */
    REG_WRITE(dma_base->PRIOSET, DMA_ChannelPriority_High, 14, 1); // Channel 14 to High 
    REG_WRITE(dma_base->PRIOSET, DMA_ChannelPriority_Default, 24, 1); // Channel 24 to Default 

    /* Configue Channel to accept only Burst transfer request */
    REG_WRITE(dma_base->USEBURSTSET, 1, 14, 1); // Channel 14
    REG_WRITE(dma_base->USEBURSTSET, 1, 24, 1); // Channel 24

    /* Enable Channel in Channel Mask */
    REG_WRITE(dma_base->REQMASKCLR, 1, 14, 1);  // ADC0 SS0 request
    REG_WRITE(dma_base->REQMASKCLR, 1, 24, 1);  // ADC1 SS0 request
}

void DMA_EnableTransfer(DMA_Module_e mod)
{
    /* Check the Preconditions */
    ASSERT(mod < DMA_Max);

    /* Get Base Address of DMA controller */
    UDMA_Type *dma_base = DMA_getBase(DMA_0);

    ControlWord.SRCINC = DMA_ChannelControl_NoInc_Res;
    ControlWord.SRCSIZE = DMA_ChannelControl_HalfWord;
    ControlWord.DSTINC = DMA_ChannelControl_HalfWord;
    ControlWord.DSTSIZE = DMA_ChannelControl_HalfWord;
    ControlWord.ARBSIZE = DMA_ChannelControl_Arbitration_4;
    ControlWord.NXTUSEBURST = 0;
    ControlWord.XFERMODE = DMA_ChannelControl_XFERMode_PingPong;

    /* Configuration for ADC0 SS0 */
    if((MAX_ADC_SAMPLE_SIZE - ADC0_SmpCnt) > DMA_MAX_XFER_COUNT)
    {
        /* LOAD PRIMARY CHANNEL CONTROL STRUCTURE */
        ControlWord.XFERSIZE = DMA_MAX_XFER_COUNT - 1;

        /* Configure Control Structure of the Channel(i.e. Channel 14) */
        DMA_ChannelConfig(DMA_ChannelControl_Primary, DMA_Channel_14, (uint32_t *)&ADC0->SSFIFO0, (uint32_t *)&adc0_val[ADC0_SmpCnt + ControlWord.XFERSIZE], ControlWord);
        
        ADC0_SmpCnt += DMA_MAX_XFER_COUNT;

        /* LOAD SECONDARY CHANNEL CONTROL STRUCTURE */
        if((MAX_ADC_SAMPLE_SIZE - ADC0_SmpCnt) > DMA_MAX_XFER_COUNT)
        {
            ControlWord.XFERSIZE = DMA_MAX_XFER_COUNT - 1;

            /* Configure Control Structure of the Channel(i.e. Channel 14) */
            DMA_ChannelConfig(DMA_ChannelControl_Secondary, DMA_Channel_14, (uint32_t *)&ADC0->SSFIFO0, (uint32_t *)&adc0_val[ADC0_SmpCnt + ControlWord.XFERSIZE], ControlWord);
            
            ADC0_SmpCnt += DMA_MAX_XFER_COUNT;            
        }
        else
        {
            ControlWord.XFERSIZE = MAX_ADC_SAMPLE_SIZE - ADC0_SmpCnt - 1;

            /* Configure Control Structure of the Channel(i.e. Channel 14) */
            DMA_ChannelConfig(DMA_ChannelControl_Secondary, DMA_Channel_14, (uint32_t *)&ADC0->SSFIFO0, (uint32_t *)&adc0_val[ADC0_SmpCnt + ControlWord.XFERSIZE], ControlWord);

            ADC0_SmpCnt += (ControlWord.XFERSIZE + 1);
        }

    }
    else
    {
            ControlWord.XFERSIZE = MAX_ADC_SAMPLE_SIZE - ADC0_SmpCnt - 1;
            ControlWord.XFERMODE = DMA_ChannelControl_XFERMode_Basic;
            /* Configure Control Structure of the Channel(i.e. Channel 14) */
            DMA_ChannelConfig(DMA_ChannelControl_Primary, DMA_Channel_14, (uint32_t *)&ADC0->SSFIFO0, (uint32_t *)&adc0_val[ADC0_SmpCnt + ControlWord.XFERSIZE], ControlWord);

            ADC0_SmpCnt += (ControlWord.XFERSIZE + 1);        
    }

    ControlWord.SRCINC = DMA_ChannelControl_NoInc_Res;
    ControlWord.SRCSIZE = DMA_ChannelControl_HalfWord;
    ControlWord.DSTINC = DMA_ChannelControl_HalfWord;
    ControlWord.DSTSIZE = DMA_ChannelControl_HalfWord;
    ControlWord.ARBSIZE = DMA_ChannelControl_Arbitration_4;
    ControlWord.NXTUSEBURST = 0;
    ControlWord.XFERMODE = DMA_ChannelControl_XFERMode_PingPong;

    /* Configuration for ADC1 SS0 */
    if((MAX_ADC_SAMPLE_SIZE - ADC1_SmpCnt) > DMA_MAX_XFER_COUNT)
    {
        /* LOAD PRIMARY CHANNEL CONTROL STRUCTURE */
        ControlWord.XFERSIZE = DMA_MAX_XFER_COUNT - 1;

        /* Configure Control Structure of the Channel(i.e. Channel 24) */
        DMA_ChannelConfig(DMA_ChannelControl_Primary, DMA_Channel_24, (uint32_t *)&ADC1->SSFIFO0, (uint32_t *)&adc1_val[ADC1_SmpCnt + ControlWord.XFERSIZE], ControlWord);
        
        ADC1_SmpCnt += DMA_MAX_XFER_COUNT;

        /* LOAD SECONDARY CHANNEL CONTROL STRUCTURE */
        if((MAX_ADC_SAMPLE_SIZE - ADC1_SmpCnt) > DMA_MAX_XFER_COUNT)
        {
            ControlWord.XFERSIZE = DMA_MAX_XFER_COUNT - 1;

            /* Configure Control Structure of the Channel(i.e. Channel 24) */
            DMA_ChannelConfig(DMA_ChannelControl_Secondary, DMA_Channel_24, (uint32_t *)&ADC1->SSFIFO0, (uint32_t *)&adc1_val[ADC1_SmpCnt + ControlWord.XFERSIZE], ControlWord);
            
            ADC1_SmpCnt += DMA_MAX_XFER_COUNT;            
        }
        else
        {
            ControlWord.XFERSIZE = MAX_ADC_SAMPLE_SIZE - ADC1_SmpCnt - 1;

            /* Configure Control Structure of the Channel(i.e. Channel 24) */
            DMA_ChannelConfig(DMA_ChannelControl_Secondary, DMA_Channel_24, (uint32_t *)&ADC1->SSFIFO0, (uint32_t *)&adc1_val[ADC1_SmpCnt + ControlWord.XFERSIZE], ControlWord);

            ADC1_SmpCnt += (ControlWord.XFERSIZE + 1);
        }

    }
    else
    {
            ControlWord.XFERSIZE = MAX_ADC_SAMPLE_SIZE - ADC1_SmpCnt - 1;
            ControlWord.XFERMODE = DMA_ChannelControl_XFERMode_Basic;
            /* Configure Control Structure of the Channel(i.e. Channel 24) */
            DMA_ChannelConfig(DMA_ChannelControl_Primary, DMA_Channel_24, (uint32_t *)&ADC1->SSFIFO0, (uint32_t *)&adc1_val[ADC1_SmpCnt + ControlWord.XFERSIZE], ControlWord);

            ADC1_SmpCnt += (ControlWord.XFERSIZE + 1);        
    }

    /* Enable the Channel to Start the Transfer when requested */
    REG_WRITE(dma_base->ENASET, 1, 14, 1); // Channel 14
    REG_WRITE(dma_base->ENASET, 1, 24, 1); // Channel 24
}