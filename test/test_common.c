
#ifdef CEEDLING_TEST

#include "unity.h"

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
    RegWrite_Bits(&regVal, 0, 0, 32);
    TEST_ASSERT_EQUAL_UINT32(0x0000000, regVal);
}

#endif // TEST
