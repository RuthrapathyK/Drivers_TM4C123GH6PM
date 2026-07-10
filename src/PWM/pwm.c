#include "pwm.h"

void PWM_Init(void)
{
    /* Reset the PWM module - 0*/
    REG_WRITE(SYSCTL->SRPWM, 1, 0, 1);
    REG_WRITE(SYSCTL->SRPWM, 0, 0, 1);

    /* Enable Clock for Timer module - 0*/
    REG_WRITE(SYSCTL->RCGCPWM, 1, 0, 1);

    /* Wait till Timer module - 0 is Enabled */
    while(!RegRead_Bits(&SYSCTL->PRPWM, 0, 1))
    ;

    /* Select the Clock Source for PWM module - System Clock/Predivided System Clock*/
    REG_WRITE(SYSCTL->RCC, 0, 20, 1); // Dont use Predivided System Clock

    /* PWM Module Configuration */
    REG_WRITE(PWM0->_0_CTL, 0, 0, 1); //Disbale the PWM0 generator
    REG_WRITE(PWM0->_0_INTEN, 1, 9, 1); // ADC trigger is enabled on Event Load

    /* PWM Timer configuration */
    REG_WRITE(PWM0->_0_CTL, 0, 1, 1);      // Select Count Down mode
    REG_WRITE(PWM0->_0_CTL, 1, 2, 1);      // Counter Always run when in Debug Mode also
    REG_WRITE(PWM0->_0_LOAD, 142, 0, 16); // Load Value

    /* PWM Comparator configuration */
    REG_WRITE(PWM0->_0_CMPA, 64, 0, 16); // Comparartor A value
    REG_WRITE(PWM0->_0_CMPB, 18000, 0, 16); //Comparartor B value - Trigger will not occur


    /* PWM Signal Generator Configuration */
    REG_WRITE(PWM0->_0_GENA, PWM_EvtAct_DoNothing, 0, 2);  // Event Zero -Do nothing
    REG_WRITE(PWM0->_0_GENA, PWM_EvtAct_DriveHigh, 2, 2);  // Event Load - Drive PWMA high
    REG_WRITE(PWM0->_0_GENA, PWM_EvtAct_DoNothing, 4, 2);  // Event Comp A Up -Do nothing
    REG_WRITE(PWM0->_0_GENA, PWM_EvtAct_DriveLow, 6, 2);   // Event Comp A Down -Drive PWMA low
    REG_WRITE(PWM0->_0_GENA, PWM_EvtAct_DoNothing, 8, 2);  // Event Comp B Up -Do nothing
    REG_WRITE(PWM0->_0_GENA, PWM_EvtAct_DoNothing, 10, 2); // Event Comp B Down -Do nothing

    REG_WRITE(PWM0->_0_GENB, PWM_EvtAct_DoNothing, 0, 2);  // Event Zero -Do nothing
    REG_WRITE(PWM0->_0_GENB, PWM_EvtAct_DoNothing, 2, 2);  // Event Load - Do nothing
    REG_WRITE(PWM0->_0_GENB, PWM_EvtAct_DoNothing, 4, 2);  // Event Comp A Up -Do nothing
    REG_WRITE(PWM0->_0_GENB, PWM_EvtAct_DoNothing, 6, 2);   // Event Comp A Down -Do nothing
    REG_WRITE(PWM0->_0_GENB, PWM_EvtAct_DoNothing, 8, 2);  // Event Comp B Up -Do nothing
    REG_WRITE(PWM0->_0_GENB, PWM_EvtAct_DoNothing, 10, 2); // Event Comp B Down -Do nothing

    /* PWM Deadband Generator Configuration */
    REG_WRITE(PWM0->_0_DBCTL, 0, 0, 1); // Disable Deadband generator

    /* PWM Output Control Logic Configuration */
    REG_WRITE(PWM0->ENABLE, 1, 0, 1); // PWM0A signal is passed to the Pin

    /* Enable the PWM Generator */
    REG_WRITE(PWM0->_0_CTL, 1, 0, 1);
}