
#ifdef CEEDLING_TEST

#include "unity.h"
#include <windows.h>

#include "common.h"

bool isAsserted = false;

/**
 * @brief Test setup function - called before each test case.
 *
 * Initializes test environment and clears state for test execution.
 */
void setUp(void)
{
}

/**
 * @brief Test teardown function - called after each test case.
 *
 * Cleans up resources and test state after test execution.
 */
void tearDown(void)
{
}

/**
 * @brief Performance test for RegWrite_Bits - timing measurement.
 *
 * Measures the execution time of RegWrite_Bits and verifies it completes
 * within 500 microseconds on the test platform.
 */
void test_common_RegWrite_Bits_0(void)
{ 
    uint32_t regVal = 0x5A5A5A5A;

    LARGE_INTEGER frequency, start, end;
    QueryPerformanceFrequency(&frequency); // Get counts per second   
    QueryPerformanceCounter(&start);

    for(int iter = 0; iter < 1; iter++)
    {
        REG_WRITE(regVal, 0, 0, 32);
    }

    QueryPerformanceCounter(&end);   
    double elapsed_us = (double)(end.QuadPart - start.QuadPart) * 1000000.0 / frequency.QuadPart;   
    TEST_ASSERT_LESS_THAN_FLOAT(500.0, elapsed_us);
}

/**
 * @brief Functional test for RegWrite_Bits - full 32-bit write.
 *
 * Verifies that RegWrite_Bits correctly writes all 32 bits to a register,
 * clearing the entire register value from 0x5A5A5A5A to 0x00000000.
 */
void test_common_RegWrite_Bits_1(void)
{ 
    uint32_t regVal = 0x5A5A5A5A;
    REG_WRITE(regVal, 0, 0, 32);
    TEST_ASSERT_EQUAL_UINT32(0x0000000, regVal);
}

/**
 * @brief Performance test for delayLoop - timing measurement.
 *
 * Measures the execution time of delayLoop(1000) and verifies the delay
 * is approximately 1 second or less (10000 microseconds threshold).
 */
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

/**
 * @brief Functional test for RegRead_Bits - various bit positions and widths.
 *
 * Comprehensively tests RegRead_Bits with different bit positions (0, 1, 30, 31)
 * and widths (1, 8, 7, 32 bits) to verify correct bit extraction and masking.
 * Also validates parameter checking for invalid bit ranges.
 */
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
