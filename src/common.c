
#include "common.h"
/*
 * @brief The function is used to create a blocking delay. It used traditional for loop
 * 
 * @param mSec delay time in approximate milli-seconds
 */
void delayLoop(uint32_t mSec)
{
  for (volatile uint32_t i = 0; i < mSec ; i++)
  {
    for(volatile uint32_t j =0; j < 1000; j++)
    {
      __asm("NOP");
    }
  }
}

__INLINE__ void RegWrite_Bits(volatile uint32_t * reg, uint32_t reg_val, uint8_t start_bit, uint8_t bit_length)
{
    /* Check the Input Parameters */
    ASSERT((reg != NULL) && (start_bit < 32) && (bit_length > 0) && (bit_length <= 32) && (start_bit + bit_length <= 32) && (reg_val < (1UL << bit_length)));
    
    /* Clear the Bits to 0 */
    *reg &= ~(((uint32_t)0xFFFFFFFF >> (32 - bit_length)) << start_bit);

    /* Write the Value in specified bits */
    *reg |= reg_val << start_bit;
}