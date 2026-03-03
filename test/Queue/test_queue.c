
#ifdef CEEDLING_TEST

#include "unity.h"
#include <windows.h>

#include "queue.h"

bool isAsserted = false;

void setUp(void)
{
}

void tearDown(void)
{
}

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
    TEST_ASSERT(test_handler.QOverflow == Queue_NoOverflow);
    Queue_Enqueue(&test_handler, (uint8_t *)"e");
    TEST_ASSERT(test_handler.QOverflow == Queue_Overflow);

    Queue_Dequeue(&test_handler, (uint8_t *)&readval);  
    TEST_ASSERT_EQUAL_CHAR('e', readval);
    Queue_Dequeue(&test_handler, (uint8_t *)&readval);  
    TEST_ASSERT_EQUAL_CHAR('c', readval);
    Queue_Dequeue(&test_handler, (uint8_t *)&readval);  
    TEST_ASSERT_EQUAL_CHAR('d', readval);

    TEST_ASSERT(Queue_isEmpty(&test_handler) == Queue_Empty);
    TEST_ASSERT(Queue_isFull(&test_handler) == Queue_NotFull);
    TEST_ASSERT(test_handler.QOverflow == Queue_Overflow);
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
