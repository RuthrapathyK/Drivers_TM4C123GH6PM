#ifndef __ADC_H
#define __ADC_H

#include "common.h"

typedef enum{
    ADC_0 = 0,
    ADC_1 = 1,
    ADC_Module_Max
}ADC_Module_e;

typedef enum{
    ADC_SampleRate_125ksps = 0x1,
    ADC_SampleRate_250ksps = 0x3,
    ADC_SampleRate_500ksps = 0x5,
    ADC_SampleRate_1000ksps = 0x7,
}ADC_SampleRate_e;

typedef enum{
    ADC_ClockSource_Either = 0x0,
    ADC_ClockSource_PIOSC = 0x1
}ADC_ClockSource_e;

typedef struct 
{
    uint8_t dummy;
}ADC_Config_t;

void ADC_Init(ADC_Module_e mod);

#endif