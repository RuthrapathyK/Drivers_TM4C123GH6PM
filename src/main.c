#include "common.h"

typedef struct 
{
  uint32_t * SrcEnd_Ptr;
  uint32_t * DstEnd_Ptr;
  uint32_t ControlWord;
  uint32_t Reserved;
}DMA_chnlCtrl_s;

typedef enum{
  DMA_PrimControl = 0,
  DMA_SecControl,
  DMA_Max_Control
}DMA_Control_Channel_e;

/* Channel Control Table */
__attribute__((aligned(1024))) DMA_chnlCtrl_s DMA_Channel_Control_Table[DMA_Max_Control][32] = {0,};

uint32_t Source_Memory[8] = { 1, 2,3, 4, 5, 6, 7, 8};
uint32_t Destination_Memory[8] = {0,};

void uDMA_Software_handler(void)
{
  Source_Memory[0]++;
}
void main()
{
  /* Enable clock for uDMA */
  SYSCTL->RCGCDMA |= 1<<0;

  /* Enable the uDMA controller */
  UDMA->CFG |= 1<<0;

  /* Load the location of the Channel Control Table */
  UDMA->CTLBASE = (uint32_t)&DMA_Channel_Control_Table;

  /* Configure the Priority of Channel 30 to High */
  UDMA->PRIOSET |= 1<<30;

  /* Configure Primary for Channel 30 */
  UDMA->ALTCLR |= 1<<30;

  /* Configure Channel 30 to respond for both Single and Burst requests */
  UDMA->USEBURSTCLR |= 1<<30;

  /* Configure uDMA controller to recognize requests for this channel */
  UDMA->REQMASKCLR |= 1<<30;

  /* Configure Control Channel Table for Channel 30 */
  DMA_Channel_Control_Table[0][30].SrcEnd_Ptr = &Source_Memory[7];
  DMA_Channel_Control_Table[0][30].DstEnd_Ptr = &Destination_Memory[7];
  DMA_Channel_Control_Table[0][30].ControlWord |= 2<<30; //32bit Destination address increament
  DMA_Channel_Control_Table[0][30].ControlWord |= 2<<28; //32-bit Destination data size
  DMA_Channel_Control_Table[0][30].ControlWord |= 2<<26; //32bit Source address increament
  DMA_Channel_Control_Table[0][30].ControlWord |= 2<<24; //32-bit Source data size
  DMA_Channel_Control_Table[0][30].ControlWord |= 3<<14; //Re-arbitrate only after 8 transfers 

  /* Enable NVIC interrupt for GPIO Port F */
  NVIC->EN1 |= 1<<14;

  while(1)
  {
    DMA_Channel_Control_Table[0][30].ControlWord |= 7<<4; //Transfer 8 words
    DMA_Channel_Control_Table[0][30].ControlWord |= 2<<0; //Use Auto-request transfer mode

    /* Enable Channel 30 */
    UDMA->ENASET |= 1<<30;

    /* Trigger the Channel 30 transfer */
    UDMA->SWREQ |= 1<<30;

    /* Block till transfer is completed */
    while(UDMA->ENASET & (1<<30))
    ;
  }
}