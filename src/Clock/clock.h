#ifndef CLOCK_H_INCLUDED
#define CLOCK_H_INCLUDED

#include "common.h"

typedef enum{
    Main_OSC = 0x0,
    Precise_OSC = 0x1,
    Precise_OSC_by_4 = 0x2,
    Internal_OSC = 0x3,
    Hibernation_OSC = 0x7
}ClockSource_e;

typedef enum{
    SysClock_from_PLL = 0,
    SysClock_from_OSCSource = 1
}ClockPLL_e;

typedef enum{
    PLL_PowerOn = 0,
    PLL_PowerOff = 1
}ClockPLL_Power_e;

typedef enum{
    SysClock_Undivided = 0,
    SysClock_Divided = 1
}Clock_SysDiv_e;

typedef enum{
    Clock_MainOSC_Enabled = 0,
    Clock_MainOSC_Disabled = 1
}Clock_MainOSC_e;

typedef enum{
    Clock_DIV400_NoSYSDIV2LSB = 0,
    Clock_DIV400_UseSYSDIV2LSB = 1
}Clock_DIV400_e;

typedef enum{
    Clock_NoOverride_RCC = 0,
    Clock_Override_RCC = 1
}Clock_OverRideRCC_e;

typedef enum{
    Clock_XTAL_5MHz = 0x09,
    Clock_XTAL_5_12MHz = 0x0A,
    Clock_XTAL_6MHz = 0x0B,
    Clock_XTAL_6_144MHz = 0x0C,
    Clock_XTAL_7_3728MHz = 0x0D,
    Clock_XTAL_8MHz = 0x0E,
    Clock_XTAL_8_192MHz = 0x0F,
    Clock_XTAL_10MHz = 0x10,
    Clock_XTAL_12MHz = 0x11,
    Clock_XTAL_12_288MHz = 0x12,
    Clock_XTAL_13_56MHz = 0x13,
    Clock_XTAL_14_31818MHz = 0x14,
    Clock_XTAL_16MHz = 0x15,
    Clock_XTAL_16_384MHz = 0x16,
    Clock_XTAL_18MHz = 0x17,
    Clock_XTAL_20MHz = 0x18,
    Clock_XTAL_24MHz = 0x19,
    Clock_XTAL_25MHz = 0x1A
}Clock_XTAL_e;

typedef enum{
    Root_PrecisionOSC = 0,
    Root_PrecisionOSC_by_4,
    Root_MainOSC,
    Root_InternalOSC,
    Root_HibernationOSC,
    Node_SysDiv,
    Node_Divide2,
    Node_PLL,
    Mux_OSCSRC1,
    Mux_OSCSRC2,
    Mux_DIV400,
    Mux_BYPASS,
    Mux_SYSDIV,
    End_SystemClock,
    NodeList_Max
}NodeList_e;

typedef enum{
    Clock_Found,
    Clock_NotFound,
    Clock_Proceed
}ClockReturn;

typedef struct{
    ClockSource_e OSCSRC2;
    ClockPLL_e BYPASS2;
    ClockPLL_Power_e PWRDN2;
    Clock_SysDiv_e USESYSDIV;
    uint8_t SYSDIV2;
    Clock_MainOSC_e MOSCDIS;
    Clock_XTAL_e XTAL;
    Clock_DIV400_e DIV400;
    Clock_OverRideRCC_e USERCC2;
}ClockReg_t;

typedef struct{
    ClockReg_t BestMatchReg;
    ClockReg_t TempReg;
    uint32_t BestClock;
    int64_t BestError;
    uint32_t ExpectedFreq;
}ClockUserEnd_t;

typedef struct Node Node_t;

struct Node{
    NodeList_e Node;    // Member is used for Debugging
    ClockReturn (*fp)(uint32_t input_freq, uint32_t * const output_freq, uint32_t iter_num, ClockUserEnd_t *object, Node_t *prev_node); // Function Private to the Node
    uint32_t NodeIter;    // Number of Possible Iterations the Node may loop
    struct Node *child_1; // Contains the address of the Child node if any or NULL if no child node
    struct Node *child_2; // Contains the address of the Child node if any or NULL if no child node
    struct Node *child_3; // Contains the address of the Child node if any or NULL if no child node
};

void Clock_Init(ClockSource_e clk_src, uint32_t expected_freq);
uint32_t Clock_Get_SysClkFreq(void);

#endif // CLOCK_H_INCLUDED
