#ifndef __COMMON_H
#define __COMMON_H

#include <stddef.h>
#include <stdbool.h>
#include "typedef.h"
#include "arm_cortex_m4f.h"
#include "TM4C123GH6PM.h"

#define ENABLED   1
#define DISABLED  0

#define RTOS_ENABLED ENABLED

#define PENDSV_TRIGGER  SCB->INTCTRL |= 1<<28
#define PENDSV_CLEAR    SCB->INTCTRL |= 1<<27
#define SYSTICK_TRIGGER SCB->INTCTRL |= 1<<26
#define SYSTICK_CLEAR   SCB->INTCTRL |= 1<<25

#define __INLINE__ inline __attribute__((always_inline))

#define SYSTEM_CLOCK_FREQ 16000000

#ifdef CEEDLING_TEST
  #include "unity.h"
  extern bool isAsserted;
  #define ASSERT(x) { \
    if(!(x)) \
      isAsserted = true; \
    else \
      isAsserted = false; \
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

__INLINE__ void RegWrite_Bits(volatile uint32_t * reg, uint32_t reg_val, uint8_t start_bit, uint8_t bit_length)
{
    /* Check the Input Parameters */
    ASSERT((reg != NULL) && (start_bit < 32) && (bit_length > 0) && (bit_length <= 32) && (start_bit + bit_length <= 32) && (reg_val < (1UL << bit_length)));
    
    uint32_t temp = *reg;

    /* Clear the Bits to 0 */
    temp &= ~(((uint32_t)0xFFFFFFFF >> (32 - bit_length)) << start_bit);

    /* Write the Value in specified bits */
    temp |= reg_val << start_bit;

    /* To avoid Unknown operation while clearing and writing new value, temp variable is used */
    *reg = temp;
}

__INLINE__ uint32_t RegRead_Bits(volatile uint32_t * reg, uint8_t start_bit, uint8_t bit_length)
{
    /* Check the Input Parameters */    
    ASSERT((bit_length <= 32) && (start_bit + bit_length <= 32) && (bit_length > 0));

    return ((*reg >> start_bit) & (uint32_t)(((uint64_t)1 << (uint64_t)bit_length) - (uint64_t)1));
}

void delayLoop(uint32_t mSec);

#endif