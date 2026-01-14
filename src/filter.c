#include "filter.h"

#define MVA_SAMPLE_WINDOW_SIZE 20

uint16_t Filter_Moving_Average(uint16_t cur_sample)
{
    static uint8_t buf_ptr = 0;
    static uint16_t circ_buffer[MVA_SAMPLE_WINDOW_SIZE] = {0};
    static bool isBufferFilled = false;
    static uint32_t Total_Sum = 0;

    /* Check if the Initial Buffer is filled */
    if(isBufferFilled == false)
    {
        circ_buffer[buf_ptr] = cur_sample;
        Total_Sum += cur_sample;
        buf_ptr++;

        if(buf_ptr >= MVA_SAMPLE_WINDOW_SIZE)
        {
            isBufferFilled = true;
            buf_ptr = 0;
        }
        
        return cur_sample;
    }
    else
    {
        Total_Sum -= circ_buffer[buf_ptr];
        Total_Sum += cur_sample;
        circ_buffer[buf_ptr] = cur_sample;
        buf_ptr++;
        buf_ptr %= MVA_SAMPLE_WINDOW_SIZE; // Rollover the Pointer

        return Total_Sum/MVA_SAMPLE_WINDOW_SIZE;
    }
}