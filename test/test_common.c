
#ifdef CEEDLING_TEST

#include "unity.h"

#include "common.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_common_NeedToImplement(void)
{
    uint32_t regVal = 0xFFFFFFFF;
    RegWrite_Bits(&regVal, 0, 0, 32);
    TEST_ASSERT_EQUAL_UINT32(0x0000000, regVal);
}

#endif // TEST
