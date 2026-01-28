
#ifdef CEEDLING_TEST

#include "unity.h"
#include <windows.h>

#include "common.h"

uint32_t regVal = 0x5A5A5A5A;

void setUp(void)
{
}

void tearDown(void)
{
}

void test_common_0(void)
{ 
    LARGE_INTEGER frequency, start, end;
    QueryPerformanceFrequency(&frequency); // Get counts per second   
    QueryPerformanceCounter(&start);

    for(int iter = 0; iter < 100000; iter++)
    {
        RegWrite_Bits(&regVal, 0, 0, 32);
    }

    QueryPerformanceCounter(&end);   
    double elapsed_us = (double)(end.QuadPart - start.QuadPart) * 1000000.0 / frequency.QuadPart;   
    TEST_ASSERT_LESS_THAN_FLOAT(0.0, elapsed_us);

    TEST_ASSERT_EQUAL_UINT32(0x0000000, regVal);
}

#endif // TEST
