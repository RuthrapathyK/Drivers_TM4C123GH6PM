#include "pinconfig.h"

void Pin_Config(Pincfg_Port_e port, uint8_t pin, uint8_t func)
{
    GPIOA_Type * gpioBase = NULL;

    ASSERT((port < Port_Max) && (pin < 8) && (func < PORTPIN_MAX_FUNCTIONALITY));

    /* Enable Clock for that GPIO Port */
    SYSCTL->RCGCGPIO |= (1<<port);

    /* Map the Base address according to GPIO Port */
    switch(port)
    {
        case Port_PA:
            gpioBase = (GPIOA_Type *)GPIOA_BASE;
            break;
        case Port_PB:
            gpioBase = (GPIOA_Type *)GPIOB_BASE;
            break;
        case Port_PC:
            gpioBase = (GPIOA_Type *)GPIOC_BASE;
            break;
        case Port_PD:
            gpioBase = (GPIOA_Type *)GPIOD_BASE;
            break;
        case Port_PE:
            gpioBase = (GPIOA_Type *)GPIOE_BASE;
            break;
        case Port_PF:
            gpioBase = (GPIOA_Type *)GPIOF_BASE;
            break;
        default:
            ASSERT(0);
            break;
    }

    if(func == PORTPIN_GPIO_FUNCTION)
    {
        /* Disable Alternate Functionality for that Pin */
        gpioBase->AFSEL &= ~(1<<pin);

        /* Enable Digital Functionality */
        gpioBase->AMSEL &= ~(1<<pin);
        gpioBase->DEN |= 1<<pin;
    }
    else if(func == PORTPIN_ANALOG_FUNCTION)
    {
        /* Disable Alternate Functionality for that Pin */
        gpioBase->AFSEL &= ~(1<<pin);

        /* Enable Analog Functionality */
        gpioBase->DEN &= ~(1<<pin);
        gpioBase->AMSEL |= (1<<pin);
    }
    else /* Alternate Function */
    {
        /* Select Alternate function for the Pin */
        gpioBase->AFSEL |= 1<<pin;

        /* Enable Digital alternate Function */
        gpioBase->DEN |= 1<<pin;

        /* Set the appropriate alternate function */
        gpioBase->PCTL &= ~(0x0F << (pin * 4));
        gpioBase->PCTL |= (func << (pin * 4));
    }

}