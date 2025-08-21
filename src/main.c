#include "common.h"

uint8_t comp_status = 0;

void main()
{
  /* Enable Clock for Analog Comparator */
  SYSCTL->RCGCACMP |= 1<<0;

  /* Enable Clock for the GPIO Ports C and F */
  SYSCTL->RCGCGPIO |= (1<<5 | 1<<2);

  /* Configure the GPIO Pins for Analog Comparator's Input and Output */
  GPIOC->AFSEL &= ~(1<<6); GPIOC->DIR &= ~(1<<6); GPIOC->ODR &= ~(1<<6); 
  GPIOC->DEN &= ~(1<<6); GPIOC->PUR &= ~(1<<6); GPIOC->PDR &= ~(1<<6);  // Configure PC6(C0+) as Comaparator Input
  
  GPIOC->AFSEL &= ~(1<<7); GPIOC->DIR &= ~(1<<7); GPIOC->ODR &= ~(1<<7); 
  GPIOC->DEN &= ~(1<<7); GPIOC->PUR &= ~(1<<7); GPIOC->PDR &= ~(1<<7);  // Configure PC7(C0-) as Comparator Input
  
  GPIOF->AFSEL |= 1<<0; GPIOF->ODR &= ~(1<<0); GPIOF->DEN |= 1<<0; // Configure PF0(C0o) as Comparator Output
  GPIOF->PCTL |= 9<<0;

  /* Configure Internal Reference Voltage to 1.65V */
  COMP->ACREFCTL = 0x30C;

  /* Configure Comparator to use Internal Reference Voltage */
  COMP->ACCTL0 = 0x40C;

  /* Delay for 10us */
  for(uint32_t i = 0; i < 1000000; i++)
  ;

  while(1)
  {
     /* Read the Comparator Output Value */
    comp_status = ((COMP->ACSTAT0 >> 1) & 0x01);
  }
}