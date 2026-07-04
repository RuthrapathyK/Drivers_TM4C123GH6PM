#include "timer.h"
#include "../src/GPIO/gpio.h"
#include "../src/UART/uart.h"
#include "../src/ADC/adc.h"

void Timer_0A_16_32_handler(void)
{
    /* Clear Interrupt Status */
    REG_WRITE(TIMER0->ICR, 1, 0, 1);
}

void TIM_Init(void)
{
    /* Reset the Timer module - 0*/
    REG_WRITE(SYSCTL->SRTIMER, 1, 0, 1);
    REG_WRITE(SYSCTL->SRTIMER, 0, 0, 1);

    /* Enable Clock for Timer module - 0*/
    REG_WRITE(SYSCTL->RCGCTIMER, 1, 0, 1);

    /* Wait till Timer module - 0 is Enabled */
    while(!RegRead_Bits(&SYSCTL->PRTIMER, 0, 1))
    ;

    /* Disbale Timer Module - 0*/
    REG_WRITE(TIMER0->CTL, 0, 0, 1);

    /* Write Configurations of Timer Module */
    REG_WRITE(TIMER0->CFG, 4, 0, 3);  // Select 16-bit Timer without concatenation
    REG_WRITE(TIMER0->TAMR, 2, 0, 2); // Periodic mode
    REG_WRITE(TIMER0->TAMR, 0, 4, 1); // Downcount mode
    
    /* Configure Prescalar */
    REG_WRITE(TIMER0->TAPR, 16, 0, 8);

    /* Load Start Value to start counting */
    REG_WRITE(TIMER0->TAILR, 8, 0, 16); // Load the value to generate 10 us Once interrupt

    /* Configure Interrupt Mask */
    REG_WRITE(TIMER0->IMR, 0, 0, 1); // Timeout Interrupt Mask

    /* Configure Timer to Trigger ADC conversion */
    REG_WRITE(TIMER0->CTL, 1, 5, 1);

    /* Enable Timer Module - 0*/
    //REG_WRITE(TIMER0->CTL, 1, 0, 1);    
}
