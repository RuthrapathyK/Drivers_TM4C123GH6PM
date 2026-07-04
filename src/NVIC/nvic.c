#include "nvic.h"

/**
 * @brief Enables the given interrupt in the NVIC.
 * 
 * This function sets the corresponding bit in the appropriate NVIC ENx register
 * to enable the interrupt specified by intr_num.
 * 
 * @param intr_num Interrupt number (see NVIC_Interrupt_e)
 */
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

/**
 * @brief Disables the given interrupt in the NVIC.
 * 
 * This function sets the corresponding bit in the appropriate NVIC DISx register
 * to disable the interrupt specified by intr_num.
 * 
 * @param intr_num Interrupt number (see NVIC_Interrupt_e)
 */
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

/**
 * @brief Sets the priority of the given interrupt in the NVIC.
 * 
 * This function writes the specified priority value into the corresponding
 * NVIC priority register byte for the interrupt number.
 * 
 * @param intr_num Interrupt number (see NVIC_Interrupt_e)
 * @param prio Priority value to assign (see NVIC_Priority_e)
 */
void NVIC_SetPriority(NVIC_Interrupt_e intr_num, NVIC_Priority_e prio)
{
    uint8_t *reg_addr = (uint8_t *)&NVIC->PRI0;

    reg_addr[intr_num] &= 0x1F; // Clear the already set priority bits
    reg_addr[intr_num] |= prio << 5; // Set the new priority bits
}

/**
 * @brief Gets the priority of the given interrupt in the NVIC.
 * 
 * This function reads the priority value from the corresponding NVIC priority
 * register byte for the interrupt number.
 * 
 * @param intr_num Interrupt number (see NVIC_Interrupt_e)
 * @return Priority value of the interrupt (see NVIC_Priority_e)
 */
NVIC_Priority_e NVIC_GetPriority(NVIC_Interrupt_e intr_num)
{
    volatile uint8_t *reg_addr = (uint8_t *)&NVIC->PRI0;
    
    return reg_addr[intr_num] >> 5;
}