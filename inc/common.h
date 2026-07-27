#ifndef __COMMON_H
#define __COMMON_H

#include <stddef.h>
#include <stdbool.h>
#include "typedef.h"
#include "arm_cortex_m4f.h"
#include "TM4C123GH6PM.h"


/************************************** Features in the Project ****************************************************** */

#define ENABLED   1
#define DISABLED  0

#define RTOS_ENABLED ENABLED

/************************************** RTOS ****************************************************** */
#define PENDSV_TRIGGER  SCB->INTCTRL |= 1<<28
#define PENDSV_CLEAR    SCB->INTCTRL |= 1<<27
#define SYSTICK_TRIGGER SCB->INTCTRL |= 1<<26
#define SYSTICK_CLEAR   SCB->INTCTRL |= 1<<25

#define __INLINE__ inline __attribute__((always_inline))

/************************************** System Clock ******************************************** */
#define SYSTEM_CLOCK_FREQ 50000000

/********************************* Assertions **************************************** */
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

#define STRING(x) #x
#define STRING_LINE(x) STRING(x) // These MACROS are used to convert __LINE__ which is an interger to String. It is MACRO Stringification

#define CASSERT(cond) _Static_assert((cond), "Static Assertion Error in " __FILE__ ":" STRING_LINE(__LINE__))
/************************************** Bit Banding ***********************************************/
#define SRAM_BASE    0x20000000
#define SRAM_BB_BASE 0x22000000

#define PERIPH_BASE 0x40000000
#define PERIPH_BB_BASE 0x42000000

#define SRAM_OFFSET(SRAM_ADDR) (SRAM_ADDR - SRAM_BASE)
#define SRAM_SET_BIT(SRAM_ADDR, SRAM_BIT) (*((volatile uint32_t *)((SRAM_BB_BASE) + (SRAM_OFFSET((uint32_t)&SRAM_ADDR) * 32u) + (SRAM_BIT * 4u))) = 1);\
                                            CASSERT(SRAM_BIT < 32)
#define SRAM_CLEAR_BIT(SRAM_ADDR, SRAM_BIT) (*((volatile uint32_t *)((SRAM_BB_BASE) + (SRAM_OFFSET((uint32_t)&SRAM_ADDR) * 32u) + (SRAM_BIT * 4u))) = 0);\
                                              CASSERT(SRAM_BIT < 32)
#define SRAM_READ_BIT(SRAM_ADDR, SRAM_BIT) (*((volatile uint32_t *)((SRAM_BB_BASE) + (SRAM_OFFSET((uint32_t)&SRAM_ADDR) * 32u) + (SRAM_BIT * 4u))));\
                                              CASSERT(SRAM_BIT < 32)


#define PERIPH_OFFSET(PERIPH_ADDR) (PERIPH_ADDR - PERIPH_BASE)
#define REG_SET_BIT(PERIPH_ADDR, PERIPH_BIT) (*((volatile uint32_t *)((PERIPH_BB_BASE) + (PERIPH_OFFSET((uint32_t)&PERIPH_ADDR) * 32u) + (PERIPH_BIT * 4u))) = 1);\
                                              CASSERT(PERIPH_BIT < 32)
#define REG_CLEAR_BIT(PERIPH_ADDR, PERIPH_BIT) (*((volatile uint32_t *)((PERIPH_BB_BASE) + (PERIPH_OFFSET((uint32_t)&PERIPH_ADDR) * 32u) + (PERIPH_BIT * 4u))) = 0);\
                                                CASSERT(PERIPH_BIT < 32)
#define REG_READ_BIT(PERIPH_ADDR, PERIPH_BIT) (*((volatile uint32_t *)((PERIPH_BB_BASE) + (PERIPH_OFFSET((uint32_t)&PERIPH_ADDR) * 32u) + (PERIPH_BIT * 4u))));\
                                                CASSERT(PERIPH_BIT < 32)

/**
 * @brief Writes bits at a specified position in a register.
 *
 * Modifies specific bits within a register by clearing the target bit range and writing
 * the provided value at the specified position. Preserves all other bits in the register.
 * Validates all parameters to ensure bit operations remain within register bounds(With Static Assertions).
 *
 * @param REG_ADDR Register on which value to modify
 * @param REG_VAL Value to write at the specified bit position
 * @param START_BIT Starting bit position (0-31)
 * @param BIT_LEN Number of bits to write (1-32, must not exceed register width)
 */
void RegWrite_Bits(volatile uint32_t * reg, uint32_t reg_val, uint8_t start_bit, uint8_t bit_length);

#define REG_WRITE(REG_ADDR, REG_VAL, START_BIT, BIT_LEN) RegWrite_Bits(&REG_ADDR, REG_VAL, START_BIT, BIT_LEN);\
                                                         CASSERT((START_BIT < 32) && (BIT_LEN > 0) && (BIT_LEN <= 32) && (START_BIT + BIT_LEN <= 32))

/**
 * @brief Writes bits at a specified position in a register in Time constraint way.
 *
 * Modifies specific bits within a register by clearing the target bit range and writing
 * the provided value at the specified position. Preserves all other bits in the register.
 * Validates all parameters to ensure bit operations remain within register bounds(With Static Assertions).
 *
 * @param REG_ADDR Register on which value to modify
 * @param REG_VAL Value to write at the specified bit position
 * @param START_BIT Starting bit position (0-31)
 * @param BIT_LEN Number of bits to write (1-32, must not exceed register width)
 */
#define REG_WRITE_ASM(REG_ADDR, REG_VAL, START_BIT, BIT_LEN)   __asm volatile("CPSID i\n\t" /* Disable Global Interrupts */ \
                                                                    "LDR R6,[%0]\n\t" /* Load the Value of the Register into R6*/ \
                                                                    "BFI R6, %1, %2, %3\n\t" /* Insert the Data into the Value */ \
                                                                    "STR R6, [%0]\n\t" /* Write the New value to the Register */ \
                                                                    "CPSIE i" /* Enable Gloabl Interrupts */ \
                                                                    : /* No Output Operands */ \
                                                                    :"r"(&REG_ADDR),"r"(REG_VAL), "X"(START_BIT),  "X"(BIT_LEN) /* Input Operands */ \
                                                                    :"r6" , "memory"); /* Clobber Registers and Memory*/\
                                                                    CASSERT((START_BIT < 32) && (BIT_LEN > 0) && (BIT_LEN <= 32) && (START_BIT + BIT_LEN <= 32))
/**
 * @brief Reads bits from a specified position in a register.
 *
 * Extracts and returns the bits at the specified position from a register, shifting them
 * to the least significant bit position. Validates parameters including bit range.
 *
 * @param REG_ADDR Register to read
 * @param START_BIT Starting bit position (0-31)
 * @param BIT_LEN Number of bits to read (1-32, must not exceed register width)
 * @return The extracted bits value, right-aligned to bit 0
 */
uint32_t RegRead_Bits(volatile uint32_t * reg, uint8_t start_bit, uint8_t bit_length);

#define REG_READ(REG_ADDR, START_BIT, BIT_LEN) RegRead_Bits(&REG_ADDR, START_BIT, BIT_LEN);\
                                               CASSERT((BIT_LEN <= 32) && (START_BIT + BIT_LEN <= 32) && (BIT_LEN > 0)) //Here comiplation assertions are 

/**
 * @brief Writes bits at a specified position in a register.
 *
 * Modifies specific bits within a register by clearing the target bit range and writing
 * the provided value at the specified position. Preserves all other bits in the register.
 * Validates all parameters to ensure bit operations remain within register bounds.(With Dynamic Assertions)
 *
 * @param reg Pointer to the register to modify
 * @param reg_val Value to write at the specified bit position
 * @param start_bit Starting bit position (0-31)
 * @param bit_length Number of bits to write (1-32, must not exceed register width)
 */
void RegWrite_Bits_ASSERT(volatile uint32_t * reg, uint32_t reg_val, uint8_t start_bit, uint8_t bit_length);

/**
 * @brief Reads bits from a specified position in a register.
 *
 * Extracts and returns the bits at the specified position from a register, shifting them
 * to the least significant bit position. Validates parameters including bit range.(With Dynamic Assertions)
 *
 * @param reg Pointer to the register to read
 * @param start_bit Starting bit position (0-31)
 * @param bit_length Number of bits to read (1-32, must not exceed register width)
 * @return The extracted bits value, right-aligned to bit 0
 */
uint32_t RegRead_Bits_ASSERT(volatile uint32_t * reg, uint8_t start_bit, uint8_t bit_length);

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