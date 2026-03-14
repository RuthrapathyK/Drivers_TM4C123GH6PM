
#ifdef CEEDLING_TEST

#include "unity.h"
#include <windows.h>

//#include "uart.h"

bool isAsserted = false;
//extern void UART0_handler(void);

void setUp(void)
{
}

void tearDown(void)
{
}

void test_uart_UART0_handler_0(void)
{ 
    //UART0_handler();
    TEST_IGNORE_MESSAGE("Ignore");
}
  
#endif // TEST
