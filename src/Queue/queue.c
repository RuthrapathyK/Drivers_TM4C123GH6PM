#include "queue.h"

static void Queue_getDefaultConfig(Queue_t *inst)
{
    inst->eIdx = 0;
    inst->dIdx = 0;
    inst->eCount = 0;
    inst->dCount = 0;
    inst->QOverflow = Queue_NoOverflow;
}

void Queue_Init(Queue_t *inst, uint8_t *pBuf, uint32_t IdxSize, uint32_t IdxMax)
{
    /* Load Default Configuration of a Queue */
    Queue_getDefaultConfig(inst);

    /* Load Handler with Other configurations */
    inst->QBuff = pBuf;
    inst->QBuff_IdxSize = IdxSize;
    inst->QBuff_Max = IdxMax;
}

void Queue_Enqueue(Queue_t *inst, uint8_t *InData)
{
  /* Check for Overflow */
  if(Queue_isFull(inst) == Queue_Full)
    Queue_setOverflow_State(inst, Queue_Overflow);

  /* Load the Queue with all Enqueued bytes */
  for(uint32_t byteIdx = 0; byteIdx < inst->QBuff_IdxSize; byteIdx++)
  {
    inst->QBuff[(inst->eIdx * inst->QBuff_IdxSize)+ byteIdx] = InData[byteIdx];
  }

  /* Increment Enqueuing Index */
  inst->eIdx = (inst->eIdx + 1) % inst->QBuff_Max;
  inst->eCount++; // Increment the Enqueuing Count also - Count will be used to detect whether the Buffer is full or empty
}

void Queue_Dequeue(Queue_t *inst , uint8_t *OutData)
{
    /* Load the Dequeued data to the Out Variable */
    for(uint32_t byteIdx = 0; byteIdx < inst->QBuff_IdxSize; byteIdx++)
    {
        OutData[byteIdx] = inst->QBuff[(inst->dIdx * inst->QBuff_IdxSize)+ byteIdx];
    }

    inst->dIdx = (inst->dIdx + 1) % inst->QBuff_Max; // Increament the Dequeuing Index
    inst->dCount++; // Increment the Dequeuing Count also - Count will be used to detect whether the Buffer is full or empty
}

int32_t Queue_TotalFilledIndex(Queue_t *inst)
{
    return (inst->eCount - inst->dCount);
}

Queue_Emptyness_e Queue_isEmpty(Queue_t *inst)
{
    /* Check if Queue is Empty or Not */
    return Queue_TotalFilledIndex(inst) == 0 ? Queue_Empty : Queue_NotEmpty;
}

Queue_Fullness_e Queue_isFull(Queue_t *inst)
{
    /* Check if Queue is Full or Not */
    return Queue_TotalFilledIndex(inst) >= inst->QBuff_Max ? Queue_Full : Queue_NotFull;
}

void Queue_fullFlush(Queue_t *inst)
{
    /* Load Default Configuration of a Queue */
    Queue_getDefaultConfig(inst);

    /* Clear all Bytes in the Buffer to 0 */
    for(uint32_t iter = 0; iter < (inst->QBuff_IdxSize * inst->QBuff_Max); iter++)
    {
        inst->QBuff[iter] = 0;
    } 
}
void Queue_setOverflow_State(Queue_t *inst, Queue_Overflow_state state)
{
    inst->QOverflow = state;
}

Queue_Overflow_state Queue_getOverflow_State(Queue_t *inst)
{
    return inst->QOverflow;
}