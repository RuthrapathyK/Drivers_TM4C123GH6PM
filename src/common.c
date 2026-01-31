
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
