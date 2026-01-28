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

#define __INLINE__ inline __attribute__((always_inline))

#define SYSTEM_CLOCK_FREQ 16000000

void RegWrite_Bits(volatile uint32_t * reg, uint32_t reg_val, uint8_t start_bit, uint8_t bit_length);
void delayLoop(uint32_t mSec);

#ifdef CEEDLING_TEST
  #define ASSERT(x) { \
    if(!(x)){ \
      while(1){ \
      }\
    }\
  }
#else
  #define ASSERT(x) { \
    if(!(x)){ \
      __asm("BKPT");\
      while(1){ \
      }\
    }\
  }
#endif
#endif