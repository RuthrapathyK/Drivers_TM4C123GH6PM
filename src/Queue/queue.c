#include "queue.h"

static void Queue_getDefaultConfig(Queue_t *inst)
{
    inst->eIdx = -1;
    inst->dIdx = -1;
    inst->QBuff = NULL;
    inst->QBuff_Max = 0;
    inst->QOverflow = Queue_NoOverflow;
    inst->QBuff_IdxSize = 0;
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
  /* Increament Enqueuing Index */
  inst->eIdx = (inst->eIdx + 1) % inst->QBuff_Max;

  /* Check for Overflow */
  if(inst->dIdx == inst->eIdx)
    Queue_setOverflow_State(inst, Queue_Overflow);

  /* Load the Queue with all Enqueued bytes */
  for(uint32_t byteIdx = 0; byteIdx < inst->QBuff_IdxSize; byteIdx++)
  {
    inst->QBuff[(inst->eIdx * inst->QBuff_IdxSize)+ byteIdx] = InData[byteIdx];
  }
}

void Queue_Dequeue(Queue_t *inst , uint8_t *OutData)
{
    /* Increament the Dequeuing Index */
    inst->dIdx = (inst->dIdx + 1) % inst->QBuff_Max;

    /* Load the Dequeued data to the Out Variable */
    for(uint32_t byteIdx = 0; byteIdx < inst->QBuff_IdxSize; byteIdx++)
    {
        OutData[byteIdx] = inst->QBuff[(inst->dIdx * inst->QBuff_IdxSize)+ byteIdx];
    }
}

Queue_State Queue_isEmpty(Queue_t *inst)
{
    /* Check if Queue is Empty or Not */
    return (inst->dIdx == inst->eIdx) ? Queue_Empty : Queue_NotEmpty;
}

void Queue_setOverflow_State(Queue_t *inst, Queue_Overflow_state state)
{
    inst->QOverflow = state;
}

Queue_Overflow_state Queue_getOverflow_State(Queue_t *inst)
{
    return inst->QOverflow;
}