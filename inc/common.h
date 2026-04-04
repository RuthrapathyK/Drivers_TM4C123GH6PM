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

/**
 * @brief Writes bits at a specified position in a register.
 *
 * Modifies specific bits within a register by clearing the target bit range and writing
 * the provided value at the specified position. Preserves all other bits in the register.
 * Validates all parameters to ensure bit operations remain within register bounds.
 *
 * @param reg Pointer to the register to modify
 * @param reg_val Value to write at the specified bit position
 * @param start_bit Starting bit position (0-31)
 * @param bit_length Number of bits to write (1-32, must not exceed register width)
 */
void RegWrite_Bits(volatile uint32_t * reg, uint32_t reg_val, uint8_t start_bit, uint8_t bit_length);
/**
 * @brief Reads bits from a specified position in a register.
 *
 * Extracts and returns the bits at the specified position from a register, shifting them
 * to the least significant bit position. Validates parameters including bit range.
 *
 * @param reg Pointer to the register to read
 * @param start_bit Starting bit position (0-31)
 * @param bit_length Number of bits to read (1-32, must not exceed register width)
 * @return The extracted bits value, right-aligned to bit 0
 */
uint32_t RegRead_Bits(volatile uint32_t * reg, uint8_t start_bit, uint8_t bit_length);

/**
 * @brief Creates a blocking delay for approximate milliseconds.
 *
 * Implements a busy-wait delay using nested loops with NOP instructions. The actual delay
 * duration depends on the system clock frequency and compiler optimization settings.
 *
 * @param mSec Approximate delay duration in milliseconds
 */
void delayLoop(uint32_t mSec);

#endif