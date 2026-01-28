
#ifdef CEEDLING_TEST

#include "unity.h"
#include <windows.h>

#include "common.h"

bool isAsserted = false;

void setUp(void)
{
}

void tearDown(void)
{
}

void test_common_RegWrite_Bits_0(void)
{ 
    uint32_t regVal = 0x5A5A5A5A;

    LARGE_INTEGER frequency, start, end;
    QueryPerformanceFrequency(&frequency); // Get counts per second   
    QueryPerformanceCounter(&start);

    for(int iter = 0; iter < 1; iter++)
    {
        RegWrite_Bits(&regVal, 0, 0, 32);
    }

    QueryPerformanceCounter(&end);   
    double elapsed_us = (double)(end.QuadPart - start.QuadPart) * 1000000.0 / frequency.QuadPart;   
    TEST_ASSERT_LESS_THAN_FLOAT(500.0, elapsed_us);
}

void test_common_RegWrite_Bits_1(void)
{ 
    uint32_t regVal = 0x5A5A5A5A;
    RegWrite_Bits(&regVal, 0, 0, 32);
    TEST_ASSERT_EQUAL_UINT32(0x0000000, regVal);
}

void test_common_delayLoop_0(void)
{
    LARGE_INTEGER frequency, start, end;
    QueryPerformanceFrequency(&frequency); // Get counts per second   
    QueryPerformanceCounter(&start);

    delayLoop(1000);

    QueryPerformanceCounter(&end);   
    double elapsed_us = (double)(end.QuadPart - start.QuadPart) * 1000000.0 / frequency.QuadPart;   
    TEST_ASSERT_LESS_THAN_FLOAT(10000.0, elapsed_us);
}

void test_common_RegRead_Bits_0(void)
{
    uint32_t regVal = 0x5A5A5A5A;

    TEST_ASSERT_EQUAL_UINT32(0x0, RegRead_Bits(&regVal, 0, 1));
    TEST_ASSERT_EQUAL_UINT32(0x1, RegRead_Bits(&regVal, 1, 1));
    TEST_ASSERT_EQUAL_UINT32(0x1, RegRead_Bits(&regVal, 30, 1));
    TEST_ASSERT_EQUAL_UINT32(0x0, RegRead_Bits(&regVal, 31, 1));
    TEST_ASSERT_EQUAL_UINT32(0x5A, RegRead_Bits(&regVal, 0, 8));
    TEST_ASSERT_EQUAL_UINT32(0x5A, RegRead_Bits(&regVal, 24, 8));
    TEST_ASSERT_EQUAL_UINT32(0x52, RegRead_Bits(&regVal, 5, 7));
    TEST_ASSERT_EQUAL_UINT32(0x5A5A5A5A, RegRead_Bits(&regVal, 0, 32));

    RegRead_Bits(&regVal, 0, 0);
    TEST_ASSERT_EQUAL_HEX(true, isAsserted);

    RegRead_Bits(&regVal, 3, 30);
    TEST_ASSERT_EQUAL_HEX(true, isAsserted);

    RegRead_Bits(&regVal, 2, 30);
    TEST_ASSERT_EQUAL_HEX(false, isAsserted);
}   
#endif // TEST
