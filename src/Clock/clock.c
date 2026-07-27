#include "clock.h"

#define MAX_SYSTEM_CLOCK_FREQ 80000000


static ClockReturn End_SystemClock_NodeFunc(uint32_t input_freq, uint32_t * const output_freq, uint32_t iter_num, ClockUserEnd_t *object, Node_t *prev_node);
static ClockReturn Mux_SYSDIV_NodeFunc(uint32_t input_freq, uint32_t * const output_freq, uint32_t iter_num, ClockUserEnd_t *object, Node_t *prev_node);
static ClockReturn SysDiv_NodeFunc(uint32_t input_freq, uint32_t * const output_freq, uint32_t iter_num, ClockUserEnd_t *object, Node_t *prev_node);
static ClockReturn Mux_BYPASS_NodeFunc(uint32_t input_freq, uint32_t * const output_freq, uint32_t iter_num, ClockUserEnd_t *object, Node_t *prev_node);
static ClockReturn Mux_DIV400_NodeFunc(uint32_t input_freq, uint32_t * const output_freq, uint32_t iter_num, ClockUserEnd_t *object, Node_t *prev_node);
static ClockReturn Divide2_NodeFunc(uint32_t input_freq, uint32_t * const output_freq, uint32_t iter_num, ClockUserEnd_t *object, Node_t *prev_node);
static ClockReturn PLL_NodeFunc(uint32_t input_freq, uint32_t * const output_freq, uint32_t iter_num, ClockUserEnd_t *object, Node_t *prev_node);
static ClockReturn Mux_OSCRC1_NodeFunc(uint32_t input_freq, uint32_t * const output_freq, uint32_t iter_num, ClockUserEnd_t *object, Node_t *prev_node);
static ClockReturn Mux_OSCRC2_NodeFunc(uint32_t input_freq, uint32_t * const output_freq, uint32_t iter_num, ClockUserEnd_t *object, Node_t *prev_node);
static ClockReturn PrecisionOSC_NodeFunc(uint32_t input_freq, uint32_t * const output_freq, uint32_t iter_num, ClockUserEnd_t *object, Node_t *prev_node);
static ClockReturn PrecisionOSC_by_4_NodeFunc(uint32_t input_freq, uint32_t * const output_freq, uint32_t iter_num, ClockUserEnd_t *object, Node_t *prev_node);
static ClockReturn MainOSC_NodeFunc(uint32_t input_freq, uint32_t * const output_freq, uint32_t iter_num, ClockUserEnd_t *object, Node_t *prev_node);
static ClockReturn InternalOSC_NodeFunc(uint32_t input_freq, uint32_t * const output_freq, uint32_t iter_num, ClockUserEnd_t *object, Node_t *prev_node);
static ClockReturn HibernationOSC_NodeFunc(uint32_t input_freq, uint32_t * const output_freq, uint32_t iter_num, ClockUserEnd_t *object, Node_t *prev_node);
static void Clock_ResetRegisters(ClockReg_t *reg);
static void Clock_ObjectInit(ClockUserEnd_t *object);
static ClockReturn Recurse_Func(Node_t *cur_nodeptr, uint32_t input_freq, Node_t *prev_nodeptr, ClockUserEnd_t *object);

Node_t NodeList[NodeList_Max] =
{
    {.Node = Root_PrecisionOSC,      .fp = PrecisionOSC_NodeFunc,        .NodeIter = 1,      .child_1 = &NodeList[Mux_OSCSRC2],           .child_2 = &NodeList[Mux_OSCSRC1],        .child_3 = NULL},
    {.Node = Root_PrecisionOSC_by_4, .fp = PrecisionOSC_by_4_NodeFunc,   .NodeIter = 1,      .child_1 = &NodeList[Mux_OSCSRC2],           .child_2 = NULL,                          .child_3 = NULL},
    {.Node = Root_MainOSC,           .fp = MainOSC_NodeFunc,             .NodeIter = 1,      .child_1 = &NodeList[Mux_OSCSRC1],           .child_2 = &NodeList[Mux_OSCSRC2],        .child_3 = NULL},
    {.Node = Root_InternalOSC,       .fp = InternalOSC_NodeFunc,         .NodeIter = 1,      .child_1 = &NodeList[Mux_OSCSRC2],           .child_2 = NULL,                          .child_3 = NULL},
    {.Node = Root_HibernationOSC,    .fp = HibernationOSC_NodeFunc,      .NodeIter = 1,      .child_1 = &NodeList[Mux_OSCSRC2],           .child_2 = NULL,                          .child_3 = NULL},
    {.Node = Node_SysDiv,            .fp = SysDiv_NodeFunc,              .NodeIter = 128,    .child_1 = &NodeList[Mux_SYSDIV],            .child_2 = NULL,                          .child_3 = NULL},
    {.Node = Node_Divide2,           .fp = Divide2_NodeFunc,             .NodeIter = 1,      .child_1 = &NodeList[Mux_DIV400],            .child_2 = NULL,                          .child_3 = NULL},
    {.Node = Node_PLL,               .fp = PLL_NodeFunc,                 .NodeIter = 1,      .child_1 = &NodeList[Node_Divide2],          .child_2 = &NodeList[Mux_DIV400],         .child_3 = NULL},
    {.Node = Mux_OSCSRC1,            .fp = Mux_OSCRC1_NodeFunc,          .NodeIter = 1,      .child_1 = &NodeList[Node_PLL],              .child_2 = NULL,                          .child_3 = NULL},
    {.Node = Mux_OSCSRC2,            .fp = Mux_OSCRC2_NodeFunc,          .NodeIter = 1,      .child_1 = &NodeList[Mux_BYPASS],            .child_2 = NULL,                          .child_3 = NULL},
    {.Node = Mux_DIV400,             .fp = Mux_DIV400_NodeFunc,          .NodeIter = 1,      .child_1 = &NodeList[Mux_BYPASS],            .child_2 = NULL,                          .child_3 = NULL},
    {.Node = Mux_BYPASS,             .fp = Mux_BYPASS_NodeFunc,          .NodeIter = 1,      .child_1 = &NodeList[Mux_SYSDIV],            .child_2 = &NodeList[Node_SysDiv],        .child_3 = NULL},
    {.Node = Mux_SYSDIV,             .fp = Mux_SYSDIV_NodeFunc,          .NodeIter = 1,      .child_1 = &NodeList[End_SystemClock],       .child_2 = NULL,                          .child_3 = NULL},
    {.Node = End_SystemClock,        .fp = End_SystemClock_NodeFunc,     .NodeIter = 1,      .child_1 = NULL,                             .child_2 = NULL,                          .child_3 = NULL}
};


static ClockReturn End_SystemClock_NodeFunc(uint32_t input_freq, uint32_t * const output_freq, uint32_t iter_num, ClockUserEnd_t *object, Node_t *prev_node)
{
    ClockReturn retval = Clock_NotFound;

    /* Check if the Expected Frequency is found and Handle Corner case of System Clock > 80MHz */
    if((input_freq == object->ExpectedFreq) && (object->ExpectedFreq <= MAX_SYSTEM_CLOCK_FREQ))
        retval = Clock_Found;
    else
        retval = Clock_NotFound;

    /* Find the Difference */
    int64_t diff = (int64_t)object->ExpectedFreq - (int64_t)input_freq;

    /* Take Modulus to remove the negative value in Error calculation */
    if(diff < 0)
        diff = (int64_t)-1 * diff;

    /* Copy the Best Matching Frequency, Error and Register Configuration */
    if(diff < object->BestError)
    {
        object->BestError = diff;
        object->BestClock = input_freq;
        object->BestMatchReg = object->TempReg;
    }

    /* Unused Parameters */
    (void)iter_num;
    (void)prev_node;
    (void)output_freq;

    return retval;
}

static ClockReturn SysDiv_NodeFunc(uint32_t input_freq, uint32_t * const output_freq, uint32_t iter_num, ClockUserEnd_t *object, Node_t *prev_node)
{
    if(object->TempReg.DIV400 == Clock_DIV400_NoSYSDIV2LSB)
    {
        /* Check if the configuration is Reserved for this MCU */
        if((iter_num >= 64) || ((object->TempReg.BYPASS2 == SysClock_from_PLL) && (iter_num < 2)))
        {
            *output_freq = 0; // Reserved
        }
        else
        {
            object->TempReg.SYSDIV2 = iter_num << 1;
            *output_freq = input_freq / (iter_num + 1);
        }

    }
    else if(object->TempReg.DIV400 == Clock_DIV400_UseSYSDIV2LSB)
    {
        /* Check if the configuration is Reserved for this MCU */
        if(((object->TempReg.BYPASS2 == SysClock_from_PLL) && (iter_num < 4)) ||
            ((object->TempReg.BYPASS2 == SysClock_from_OSCSource) && (iter_num < 2)))
        {
            *output_freq = 0; // Reserved
        }
        else
        {
            object->TempReg.SYSDIV2 = iter_num;
            *output_freq = input_freq / (iter_num + 1);
        }
    }
    else
    {
        ASSERT(0);
    }

    /* Unused Parameters */
    (void)prev_node;

    return Clock_Proceed;
}

static ClockReturn Divide2_NodeFunc(uint32_t input_freq, uint32_t * const output_freq, uint32_t iter_num, ClockUserEnd_t *object, Node_t *prev_node)
{
    *output_freq = input_freq / 2;

    /* Unused Parameters */
    (void)iter_num;
    (void)object;
    (void)prev_node;

    return Clock_NotFound; // Return Value has no impact in execution
}

static ClockReturn PLL_NodeFunc(uint32_t input_freq, uint32_t * const output_freq, uint32_t iter_num, ClockUserEnd_t *object, Node_t *prev_node)
{
    *output_freq = 400000000;

    /* Enable Power for PLL module */
    object->TempReg.PWRDN2 = PLL_PowerOn;

    /* Unused Parameters */
    (void)iter_num;
    (void)prev_node;
    (void)input_freq;

    return Clock_Proceed;
}

static ClockReturn Mux_SYSDIV_NodeFunc(uint32_t input_freq, uint32_t * const output_freq, uint32_t iter_num, ClockUserEnd_t *object, Node_t *prev_node)
{
    /* Pass the Input Frequency directly as Output as MUX will not change the Frequency */
    *output_freq = input_freq;

    if(prev_node == &NodeList[Node_SysDiv])
        object->TempReg.USESYSDIV = SysClock_Divided;
    else if(prev_node == &NodeList[Mux_BYPASS])
        object->TempReg.USESYSDIV = SysClock_Undivided;
    else
        ASSERT(0);

    /* Unused Parameters */
    (void)iter_num;

    return Clock_Proceed;
}

static ClockReturn Mux_BYPASS_NodeFunc(uint32_t input_freq, uint32_t * const output_freq, uint32_t iter_num, ClockUserEnd_t *object, Node_t *prev_node)
{
    if(prev_node == &NodeList[Mux_OSCSRC2])
    {
        object->TempReg.BYPASS2 = SysClock_from_OSCSource;
        object->TempReg.DIV400 = Clock_DIV400_NoSYSDIV2LSB;
        object->TempReg.PWRDN2 = PLL_PowerOff;
    }
    else if(prev_node == &NodeList[Mux_DIV400])
    {
        object->TempReg.BYPASS2 = SysClock_from_PLL;// PLL power and UseSYSDIV2LSB will be set in the corresponding node and mux
    }
    else
        ASSERT(0);

    /* Pass the Input Frequency directly as Output as MUX will not change the Frequency */
    *output_freq = input_freq;

    /* Unused Parameters */
    (void)iter_num;

    return Clock_Proceed;
}

static ClockReturn Mux_DIV400_NodeFunc(uint32_t input_freq, uint32_t * const output_freq, uint32_t iter_num, ClockUserEnd_t *object, Node_t *prev_node)
{
    if(prev_node == &NodeList[Node_Divide2])
    {
        object->TempReg.DIV400 = Clock_DIV400_NoSYSDIV2LSB;
    }
    else
    {
        object->TempReg.DIV400 = Clock_DIV400_UseSYSDIV2LSB;
    }

    /* Pass the Input Frequency directly as Output as MUX will not change the Frequency */
    *output_freq = input_freq;

    /* Unused Parameters */
    (void)iter_num;

    return Clock_Proceed;
}

static ClockReturn Mux_OSCRC1_NodeFunc(uint32_t input_freq, uint32_t * const output_freq, uint32_t iter_num, ClockUserEnd_t *object, Node_t *prev_node)
{
    /* Pass the Input Frequency directly as Output as MUX will not change the Frequency */
    *output_freq = input_freq;

    /* Unused Parameters */
    (void)iter_num;
    (void)object;
    (void)prev_node;

    return Clock_Proceed;
}

static ClockReturn Mux_OSCRC2_NodeFunc(uint32_t input_freq, uint32_t * const output_freq, uint32_t iter_num, ClockUserEnd_t *object, Node_t *prev_node)
{
    /* Pass the Input Frequency directly as Output as MUX will not change the Frequency */
    *output_freq = input_freq;

    /* Unused Parameters */
    (void)iter_num;
    (void)object;
    (void)prev_node;

    return Clock_Proceed;
}

static ClockReturn PrecisionOSC_NodeFunc(uint32_t input_freq, uint32_t * const output_freq, uint32_t iter_num, ClockUserEnd_t *object, Node_t *prev_node)
{
    /* Initialize the Clock Handler Object */
    Clock_ObjectInit(object);

    object->TempReg.USERCC2 = Clock_Override_RCC;  // Set USERCC2 bit to enable RCC2 register override RCC
    object->TempReg.OSCSRC2 = Precise_OSC; // Set Oscillator source in OSCSRC

    /* Pass the Input Frequency directly as Output as this is a Root Node */
    *output_freq = input_freq;

    /* Unused Parameters */
    (void)iter_num;
    (void)prev_node;

    return Clock_Proceed;
}

static ClockReturn PrecisionOSC_by_4_NodeFunc(uint32_t input_freq, uint32_t * const output_freq, uint32_t iter_num, ClockUserEnd_t *object, Node_t *prev_node)
{
    /* Initialize the Clock Handler Object */
    Clock_ObjectInit(object);

    object->TempReg.USERCC2 = Clock_Override_RCC;  // Set USERCC2 bit to enable RCC2 register override RCC
    object->TempReg.OSCSRC2 = Precise_OSC_by_4; // Set Oscillator source in OSCSRC

    /* Pass the Input Frequency directly as Output as this is a Root Node */
    *output_freq = input_freq;

    /* Unused Parameters */
    (void)iter_num;
    (void)prev_node;

    return Clock_Proceed;
}

static ClockReturn MainOSC_NodeFunc(uint32_t input_freq, uint32_t * const output_freq, uint32_t iter_num, ClockUserEnd_t *object, Node_t *prev_node)
{
    /* Initialize the Clock Handler Object */
    Clock_ObjectInit(object);

    object->TempReg.USERCC2 = Clock_Override_RCC;  // Set USERCC2 bit to enable RCC2 register override RCC
    object->TempReg.OSCSRC2 = Main_OSC; // Set Oscillator source in OSCSRC
    object->TempReg.XTAL = Clock_XTAL_16MHz;    // Set XTAL value if Main Oscillator is selected
    object->TempReg.MOSCDIS = Clock_MainOSC_Enabled;    // Reset MOSCDIS bit to enable Main Oscillator

    /* Pass the Input Frequency directly as Output as this is a Root Node */
    *output_freq = input_freq;

    /* Unused Parameters */
    (void)iter_num;
    (void)prev_node;
    
    return Clock_Proceed;
}

static ClockReturn InternalOSC_NodeFunc(uint32_t input_freq, uint32_t * const output_freq, uint32_t iter_num, ClockUserEnd_t *object, Node_t *prev_node)
{
    /* Initialize the Clock Handler Object */
    Clock_ObjectInit(object);

    object->TempReg.USERCC2 = Clock_Override_RCC;  // Set USERCC2 bit to enable RCC2 register override RCC
    object->TempReg.OSCSRC2 = Internal_OSC; // Set Oscillator source in OSCSRC

    /* Pass the Input Frequency directly as Output as this is a Root Node */
    *output_freq = input_freq;

    /* Unused Parameters */
    (void)iter_num;
    (void)prev_node;

    return Clock_Proceed;
}

static ClockReturn HibernationOSC_NodeFunc(uint32_t input_freq, uint32_t * const output_freq, uint32_t iter_num, ClockUserEnd_t *object, Node_t *prev_node)
{
    /* Initialize the Clock Handler Object */
    Clock_ObjectInit(object);

    object->TempReg.USERCC2 = Clock_Override_RCC;  // Set USERCC2 bit to enable RCC2 register override RCC
    object->TempReg.OSCSRC2 = Hibernation_OSC; // Set Oscillator source in OSCSRC

    /* Pass the Input Frequency directly as Output as this is a Root Node */
    *output_freq = input_freq;

    /* Unused Parameters */
    (void)iter_num;
    (void)prev_node;

    return Clock_Proceed;
}

static void Clock_ResetRegisters(ClockReg_t *reg)
{
    reg->BYPASS2 = SysClock_from_OSCSource;
    reg->DIV400 = Clock_DIV400_NoSYSDIV2LSB;
    reg->MOSCDIS = Clock_MainOSC_Disabled;
    reg->OSCSRC2 = Precise_OSC;
    reg->PWRDN2 = PLL_PowerOff;
    reg->SYSDIV2 = 0x1F;
    reg->USERCC2 = Clock_NoOverride_RCC;
    reg->USESYSDIV = SysClock_Undivided;
    reg->XTAL = Clock_XTAL_6MHz;
}

static void Clock_ObjectInit(ClockUserEnd_t * object)
{
    object->BestClock = 0;
    object->BestError = INT64_MAX;

    Clock_ResetRegisters(&object->TempReg);
    Clock_ResetRegisters(&object->BestMatchReg);
}

static ClockReturn Recurse_Func(Node_t * cur_nodeptr, uint32_t input_freq, Node_t *prev_nodeptr, ClockUserEnd_t *object)
{
    ClockReturn retval = Clock_NotFound;

    for(uint32_t iter = 0; iter < cur_nodeptr->NodeIter; iter++)
    {
        uint32_t ouput_freq = 0;

        /* Execute the Function if the node has any */
        if(cur_nodeptr->fp != NULL)
        {
            retval = cur_nodeptr->fp(input_freq, &ouput_freq, iter, object, prev_nodeptr);

            if(retval == Clock_Found)
                return Clock_Found;
        }

        /* Process Child Node 1 */
        if(cur_nodeptr->child_1 != NULL)
        {
            retval = Recurse_Func(cur_nodeptr->child_1, ouput_freq, cur_nodeptr, object);

            if(retval == Clock_Found)
                return Clock_Found;
        }

        /* Process Child Node 2 */
        if(cur_nodeptr->child_2 != NULL)
        {
            retval = Recurse_Func(cur_nodeptr->child_2, ouput_freq, cur_nodeptr, object);

            if(retval == Clock_Found)
                return Clock_Found;
        }

        /* Process Child Node 3 */
        if(cur_nodeptr->child_3 != NULL)
        {
            retval = Recurse_Func(cur_nodeptr->child_3, ouput_freq, cur_nodeptr, object);

            if(retval == Clock_Found)
                return Clock_Found;
        }
    }

    /* Expected Clock Frequency Cannot be set */
    return Clock_NotFound;
}

void Clock_Init(ClockSource_e clk_src, uint32_t expected_freq)
{
    ClockReturn retval = Clock_NotFound;
    ClockUserEnd_t Clock_Object;

    /* Update the Object with Expected Frequency */
    Clock_Object.ExpectedFreq = expected_freq;

    switch(clk_src)
    {
        case Precise_OSC:
            retval = Recurse_Func(&NodeList[Root_PrecisionOSC], 16000000, NULL, &Clock_Object);
            break;

        case Precise_OSC_by_4:
            retval = Recurse_Func(&NodeList[Root_PrecisionOSC_by_4], 4000000, NULL, &Clock_Object);
            break;

        case Main_OSC:
            retval = Recurse_Func(&NodeList[Root_MainOSC], 16000000, NULL, &Clock_Object);
            break;

        case Internal_OSC:
            retval = Recurse_Func(&NodeList[Root_InternalOSC], 30000, NULL, &Clock_Object);
            break;

        case Hibernation_OSC:
            retval = Recurse_Func(&NodeList[Root_HibernationOSC], 32768, NULL, &Clock_Object);
            break;

        default:
            ASSERT(0);
    }

    /* Assert to check if the Expected Clock Frequency is found */
    ASSERT(retval == Clock_Found);

    /* Configure the Clock Source */
    REG_WRITE(SYSCTL->RCC, Precise_OSC, 4, 2);

    /* Configure the PLL */
    REG_WRITE(SYSCTL->RCC, 1, 11, 1); // Disable PLL
    REG_WRITE(SYSCTL->RCC, 1, 0, 1); // Disable Main Oscillator
    REG_WRITE(SYSCTL->RCC, 0x15, 6, 5); // Say the Clock freq is 16Mhz
    REG_WRITE(SYSCTL->RCC, 0, 13, 1); // Power Up the PLL
    REG_WRITE(SYSCTL->RCC, 0x3, 23, 4); // Select System Divisor as 4 to generate 50Mhz
    REG_WRITE(SYSCTL->RCC, 1, 22, 1); //Use SYSDIV as source of System Clock

    /* Wait for PLL to Lock */
    while(!RegRead_Bits(&SYSCTL->RIS, 6, 1))
    ;

    /* Select the System Clock */
    REG_WRITE(SYSCTL->RCC, 0, 11, 1); // Enable PLL

    // /* Write the Registers */
    // /* Bypass PLL to set the configuration */
    // REG_WRITE(SYSCTL->RCC2, SysClock_from_OSCSource, 11, 1);
    // REG_WRITE(SYSCTL->RCC, SysClock_Undivided, 22, 1);

    // RegWrite_Bits_ASSERT(&SYSCTL->RCC, Clock_Object.BestMatchReg.USERCC2, 31, 1); // Use RCC2 register to Override
    // RegWrite_Bits_ASSERT(&SYSCTL->RCC, Clock_Object.BestMatchReg.MOSCDIS, 0, 1); // Main Oscillator Enable/Disable
    // RegWrite_Bits_ASSERT(&SYSCTL->RCC, Clock_Object.BestMatchReg.XTAL, 6, 5); // Choose the XTAL frequency of Main Oscillator

    // RegWrite_Bits_ASSERT(&SYSCTL->RCC2, Clock_Object.BestMatchReg.OSCSRC2, 4, 3); // Select the Clock Source
    // RegWrite_Bits_ASSERT(&SYSCTL->RCC2, Clock_Object.BestMatchReg.BYPASS2, 11, 1); // Select whether to use PLL or use from direct clock source
    // RegWrite_Bits_ASSERT(&SYSCTL->RCC2, Clock_Object.BestMatchReg.PWRDN2, 13, 1); // PLL power configuration
    // RegWrite_Bits_ASSERT(&SYSCTL->RCC2, Clock_Object.BestMatchReg.SYSDIV2, 22, 7); // System Divisor Value
    // RegWrite_Bits_ASSERT(&SYSCTL->RCC, Clock_Object.BestMatchReg.USESYSDIV, 22, 1); // Check whether to use System Divisor
    // RegWrite_Bits_ASSERT(&SYSCTL->RCC2, Clock_Object.BestMatchReg.DIV400, 30, 1); // Select whether to use SYSDIV2LSB

    // delayLoop(1000);
    // // /* Wait till PLL is locked */
    // // while(!RegRead_Bits_ASSERT(&SYSCTL->RIS, 6, 1))
    // // ;

    // // /* Write to PLL to start the Timer */
    // // REG_WRITE(SYSCTL->MISC, 1, 6, 1);

}