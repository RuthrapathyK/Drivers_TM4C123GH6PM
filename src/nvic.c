#include "nvic.h"


void NVIC_enableInterrupt(NVIC_Interrupt_e intr_num)
{
    if(intr_num >= 128)
    {
        NVIC->EN4 |= (1 << (intr_num - 128));
    }
    else if(intr_num >= 96)
    {
        NVIC->EN3 |= (1 << (intr_num - 96));
    }
    else if(intr_num >= 64)
    {
        NVIC->EN2 |= (1 << (intr_num - 64));
    }
    else if(intr_num >= 32)
    {
        NVIC->EN1 |= (1 << (intr_num - 32));
    }
    else
    {
        NVIC->EN0 |= (1 << intr_num);
    }
}

void NVIC_disableInterrupt(NVIC_Interrupt_e intr_num)
{
    if(intr_num >= 128)
    {
        NVIC->DIS4 |= (1 << (intr_num - 128));
    }
    else if(intr_num >= 96)
    {
        NVIC->DIS3 |= (1 << (intr_num - 96));
    }
    else if(intr_num >= 64)
    {
        NVIC->DIS2 |= (1 << (intr_num - 64));
    }
    else if(intr_num >= 32)
    {
        NVIC->DIS1 |= (1 << (intr_num - 32));
    }
    else
    {
        NVIC->DIS0 |= (1 << intr_num);
    }
}