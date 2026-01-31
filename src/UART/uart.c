#include "uart.h"
#include "common.h"

#define UART_MAX_QUEUE_COUNT 12

uint8_t UART_Buffer[UART_MAX_QUEUE_COUNT] = {0};


UART_Queue_t UART_Queue = {
                          .StartIdx = 0,
                          .Count = 0,
                          .Buffer = UART_Buffer,
                          .MaxCount = UART_MAX_QUEUE_COUNT
                          };

void UART0_handler(void)
{
  /* Check for Overflow */
  if(UART_Queue.Count >= UART_Queue.MaxCount)
    ASSERT(0);

  /* Read Received Data and Clears Interrupt */
  UART_Queue.Buffer[(UART_Queue.StartIdx + UART_Queue.Count) % UART_Queue.MaxCount] = RegRead_Bits(&UART0->DR, 0, 12);

  /* Increament Queue Counter */
  UART_Queue.Count++;
}

static UART0_Type* UART_getBase(UART_Module_e mod)
{
  /* Check Preconditions */
  ASSERT(mod < UART_Module_Max);

  UART0_Type * retval = 0;

  /* Get Base Address of the corresponding UART module */
  switch(mod)
  {
    case UART_0:
        retval = (UART0_Type *)UART0_BASE;
        break;
    case UART_1:
        retval = (UART0_Type *)UART1_BASE;
        break;
    case UART_2:
        retval = (UART0_Type *)UART2_BASE;
        break;
    case UART_3:
        retval = (UART0_Type *)UART3_BASE;
        break;
    case UART_4:
        retval = (UART0_Type *)UART4_BASE;
        break;
    case UART_5:
        retval = (UART0_Type *)UART5_BASE;
        break;
    case UART_6:
        retval = (UART0_Type *)UART6_BASE;
        break;
    case UART_7:
        retval = (UART0_Type *)UART7_BASE;
        break;
    default:
        ASSERT(0);
  }

  return retval;
}
/**
 * @brief Initializes UART0 with the specified baud rate.
 *
 * Configures UART0 for 8-bit, no parity, 1 stop bit, FIFO enabled, and sets the baud rate.
 * Uses system clock as source and prescaler of 8.
 *
 * @param baudrate Desired baud rate (e.g., 115200)
 */
void UART_Init(UART_Module_e mod, uint32_t baudrate)
{
  ASSERT((mod < UART_Module_Max) && (baudrate > 0));

  UART0_Type * uart_base = 0;

  /* Enable Clock for UART0 module */
  RegWrite_Bits(&SYSCTL->RCGCUART, 1, mod, 1);

  /* Wait till UART module is Enabled */
  while(!RegRead_Bits(&SYSCTL->PRUART, mod, 1))
  ;

  /* Get the Base Address based on Module */
  uart_base = UART_getBase(mod);

  /* Calculate Baudrate */
  float Baud_Val = (float)SYSTEM_CLOCK_FREQ / (8.0f * baudrate);
  uint16_t Baud_Integer = (uint16_t)Baud_Val; // Derive the Integer part of the Value
  uint8_t Baud_Fraction = (uint8_t)((((float)Baud_Val - (float)Baud_Integer) * 64.0f) + 0.5f); // Derive the Fraction part of the Value
  
  /* Disable UART */
  RegWrite_Bits(&uart_base->CTL, 0, 0, 1);

  /* Write the Baudrate */
  RegWrite_Bits(&uart_base->IBRD, Baud_Integer, 0, 16);
  RegWrite_Bits(&uart_base->FBRD, Baud_Fraction, 0, 6);

  /* Configure Stopbit, Parity, FIFOs, Word Length */
  RegWrite_Bits(&uart_base->LCRH, 3, 5, 2);
  
  /* Set prescaler to be 8 */
  RegWrite_Bits(&uart_base->CTL, 1, 5, 1); // Select UART prescaler as 8

  /* Select UART module's clock source - System Clock(16MHz) */
  RegWrite_Bits(&uart_base->CC, 0, 0, 4);

  /* Define Interrupt Masks - Receive Interrupt Mask */
  RegWrite_Bits(&uart_base->IM, 1, 4, 1);

  /* Enable UART */
  RegWrite_Bits(&uart_base->CTL, 1, 0, 1);
}

/**
 * @brief Sends a single character over UART0.
 *
 * Waits until the transmit FIFO is not full, then writes the character.
 *
 * @param ch Character to send
 */
void UART_sendChar(char ch)
{
    /* Wait till Transmission is completed */
    while(((UART0->FR >> 5) & 0x01))
    ;
    UART0->DR = ch;
}

/**
 * @brief Sends a null-terminated string over UART0.
 *
 * Sends each character in the string using UART_sendChar.
 *
 * @param str Pointer to the string to send
 */
void UART_sendString(char * str)
{
  while(*str)
  {
    UART_sendChar(*str);
    str++;
  }
}

/**
 * @brief Sends a signed 32-bit integer as ASCII characters over UART0.
 *
 * Converts the number to a string and sends it using UART_sendString.
 *
 * @param num Number to send
 */
void UART_sendNumber(int32_t num)
{
  char num_arr[20] = {0};
  uint8_t first_idx = 0, last_idx = 0;
  char temp_char = 0;

  /* Check whether the input is non-printable */
  ASSERT(num != -2147483648)  

  /* Check if the number is negative */
  if(num < 0)
  {
    UART_sendChar('-'); // Send minus character first
    num *= -1; // Make the number positive
  }

  do
  {
    num_arr[last_idx] = (num % 10)+'0'; // Store the digit as character
    num /= 10;  // Remove the digit
    last_idx++;
  }while(num);

  num_arr[last_idx] = '\0'; // Place NULL character at the end of the string array

  last_idx--; // Point the index to the last character of the num_arr

  /* Swap the Characters */
  while(last_idx > first_idx)
  {
    temp_char = num_arr[first_idx];
    num_arr[first_idx] = num_arr[last_idx];
    num_arr[last_idx] = temp_char;

    last_idx--;
    first_idx++;
  }

  /* Send the ASCII converted number String */
  UART_sendString(num_arr);
}

/**
 * @brief Receives a single character from UART0.
 *
 * Waits until the receive FIFO is not empty, then returns the character.
 *
 * @return Received character
 */
uint8_t UART_receiveChar(void)
{
    /* Wait till RX buffer is not empty */
    while(((UART0->FR >> 4) & 0x01))
    ;
    return (UART0->DR & 0xFF);
}

/**
 * @brief Receives a string from UART0 until newline.
 *
 * Reads characters into strBuf until '\n' is received, then null-terminates the buffer.
 *
 * @param strBuf Pointer to buffer for received string
 */
void UART_receiveString(uint8_t * strBuf)
{
    uint8_t rChar = 0;

    /* Receive and store the data in strBuf till \n character is received */
    do{
        rChar = UART_receiveChar();
        *strBuf = rChar;
        strBuf++;
    }while(rChar != '\n');

    *strBuf = '\0';
}