#ifndef __COMMON_H
#define __COMMON_H

#include <stddef.h>
#include <stdbool.h>
#include "typedef.h"
#include "arm_cortex_m4f.h"
#include "TM4C123GH6PM.h"

#define PENDSV_TRIGGER  SCB->INTCTRL |= 1<<28
#define PENDSV_CLEAR    SCB->INTCTRL |= 1<<27
#define SYSTICK_TRIGGER SCB->INTCTRL |= 1<<26
#define SYSTICK_CLEAR   SCB->INTCTRL |= 1<<25

#define ASSERT(x) { \
  if(!(x)) {\
    __asm("BKPT");\
    while(1){ \
    }\
  }\
}


inline void __attribute__((always_inline))__enable_irq(void)
{
  __asm("CPSIE I");
}

inline void __attribute__((always_inline))__disable_irq(void)
{
  __asm("CPSID I");
}

/**
 * @brief The function is used to create a blocking delay. It used traditional for loop
 * 
 * @param mSec delay time in approximate milli-seconds
 */
void delayLoop(uint32_t mSec)
{
  for (volatile uint32_t i = 0; i < mSec ; i++)
  {
    for(volatile uint32_t j =0; j < 500; j++)
    {
      __asm("NOP");
    }
  }
}
#endif