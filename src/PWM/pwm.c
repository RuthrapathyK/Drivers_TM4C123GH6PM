#include "pwm.h"

void PWM_Init(void)
{
    /* Reset the PWM module - 0*/
    REG_WRITE(SYSCTL->SRTIMER, 1, 0, 1);
    REG_WRITE(SYSCTL->SRTIMER, 0, 0, 1);

    /* Select the Clock Source foe PWM module - System Clock/Predivided System Clock*/

    /* Enable Clock for Timer module - 0*/
    REG_WRITE(SYSCTL->RCGCTIMER, 1, 0, 1);

    /* Wait till Timer module - 0 is Enabled */
    while(!RegRead_Bits(&SYSCTL->PRTIMER, 0, 1))
    ;

    /* Select the PWM generator is Count Down or Count Up/Down mode */
}