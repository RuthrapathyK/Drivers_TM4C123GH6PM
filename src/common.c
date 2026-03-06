
#include "common.h"

/**
 * @brief Creates a blocking delay for approximate milliseconds.
 *
 * Implements a busy-wait delay using nested loops with NOP instructions. The actual delay
 * duration depends on the system clock frequency and compiler optimization settings.
 *
 * @param mSec Approximate delay duration in milliseconds
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
