#include "common.h"

volatile uint8_t counter = 5;

void main()
{
  volatile uint8_t counter_2 = 6;

  while(1)
  {
    counter++;
    counter_2++;
  }
}