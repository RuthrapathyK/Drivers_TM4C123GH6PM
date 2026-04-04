
#ifdef CEEDLING_TEST

#include "unity.h"
#include <windows.h>

#include "queue.h"

bool isAsserted = false;

/**
 * @brief Test setup function - called before each test case.
 *
 * Initializes test environment for queue tests.
 */
void setUp(void)
{
}

/**
 * @brief Test teardown function - called after each test case.
 *
 * Cleans up resources and test state after queue test execution.
 */
void tearDown(void)
{
}

/**
 * @brief Test queue enqueue/dequeue with bit-field struct elements.
 *
 * Verifies that Queue_Init, Queue_Enqueue, and Queue_Dequeue correctly handle
 * complex data types containing bit-fields, preserving bit-field values through
 * the queue transfer operation.
 */
void test_queue_1(void)
{ 
    typedef struct{
        uint8_t  data1:5;
        uint16_t data2:4;
        uint32_t data3:5;
        uint16_t data4:3;
    }test_t;

    test_t test_buff[10];
    Queue_t test_handler;

    Queue_Init(&test_handler, (uint8_t *)test_buff, sizeof(test_buff[0]), sizeof(test_buff) / sizeof(test_buff[0]));

    test_t Obj_1 = { 0x1, 0x2, 0x3, 0x4};
    test_t Obj_2 = {0x00, 0x00, 0x00, 0x00};

    Queue_Enqueue(&test_handler, (uint8_t *)&Obj_1);
    Queue_Dequeue(&test_handler, (uint8_t *)&Obj_2);
    
    TEST_ASSERT_EQUAL_HEX(Obj_1.data1, Obj_2.data1);
    TEST_ASSERT_EQUAL_HEX(Obj_1.data2, Obj_2.data2);
    TEST_ASSERT_EQUAL_HEX(Obj_1.data3, Obj_2.data3);
    TEST_ASSERT_EQUAL_HEX(Obj_1.data4, Obj_2.data4);
}

/**
 * @brief Test queue overflow behavior and state management.
 *
 * Verifies that the queue correctly handles enqueue operations at capacity limit
 * and maintains proper state when the buffer is full. Tests overflow detection
 * and queue state consistency.
 */
void test_queue_2(void)
{ 
    uint8_t test_buff[3];
    Queue_t test_handler;

    Queue_Init(&test_handler, (uint8_t *)test_buff, sizeof(test_buff[0]), sizeof(test_buff) / sizeof(test_buff[0]));

    uint8_t readval = 0;

    Queue_Enqueue(&test_handler, (uint8_t *)"a");
    Queue_Dequeue(&test_handler, (uint8_t *)&readval);  
    TEST_ASSERT_EQUAL_CHAR('a', readval);

    Queue_Enqueue(&test_handler, (uint8_t *)"b");
    Queue_Enqueue(&test_handler, (uint8_t *)"c");
    Queue_Enqueue(&test_handler, (uint8_t *)"d");

    TEST_ASSERT(Queue_isEmpty(&test_handler) == Queue_NotEmpty);
    TEST_ASSERT(Queue_isFull(&test_handler) == Queue_Full);
    TEST_ASSERT(Queue_isOverFlowed(&test_handler) == Queue_NoOverflow);
    Queue_Enqueue(&test_handler, (uint8_t *)"e");
    TEST_ASSERT(Queue_isOverFlowed(&test_handler) == Queue_Overflow);

    Queue_Dequeue(&test_handler, (uint8_t *)&readval);  
    TEST_ASSERT_EQUAL_CHAR('c', readval);
    Queue_Dequeue(&test_handler, (uint8_t *)&readval);  
    TEST_ASSERT_EQUAL_CHAR('d', readval);
    Queue_Dequeue(&test_handler, (uint8_t *)&readval);  
    TEST_ASSERT_EQUAL_CHAR('e', readval);

    TEST_ASSERT(Queue_isEmpty(&test_handler) == Queue_Empty);
    TEST_ASSERT(Queue_isFull(&test_handler) == Queue_NotFull);
    TEST_ASSERT(Queue_isOverFlowed(&test_handler) == Queue_NoOverflow);
}

void test_queue_3(void)
{
    typedef struct{
        uint8_t  data1:5;
        uint16_t data2:4;
        uint32_t data3:5;
        uint16_t data4:3;
    }test_t;

    test_t test_buff[10];
    Queue_t test_handler;

    Queue_Init(&test_handler, (uint8_t *)test_buff, sizeof(test_buff[0]), sizeof(test_buff) / sizeof(test_buff[0]));

    test_t Obj_1 = { 0x1, 0x2, 0x3, 0x4};
    test_t Obj_2 = {0x00, 0x00, 0x00, 0x00};

    Queue_Enqueue(&test_handler, (uint8_t *)&Obj_1);
    Queue_Dequeue(&test_handler, (uint8_t *)&Obj_2);
    
    TEST_ASSERT_EQUAL_HEX(Obj_1.data1, Obj_2.data1);
    TEST_ASSERT_EQUAL_HEX(Obj_1.data2, Obj_2.data2);
    TEST_ASSERT_EQUAL_HEX(Obj_1.data3, Obj_2.data3);
    TEST_ASSERT_EQUAL_HEX(Obj_1.data4, Obj_2.data4);
    
    Queue_fullFlush(&test_handler);

    for(uint32_t iter = 0; (iter < test_handler.QBuff_IdxSize * test_handler.QBuff_Max); iter++)
    {
        TEST_ASSERT_EQUAL_HEX(0, test_handler.QBuff[iter]);
    }
}
#endif // TEST
