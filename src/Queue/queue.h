#ifndef __QUEUE_H
#define __QUEUE_H

#include "common.h"

typedef enum{
    Queue_Empty,
    Queue_NotEmpty
}Queue_Emptyness_e;

typedef enum{
    Queue_Full,
    Queue_NotFull
}Queue_Fullness_e;

typedef enum{
    Queue_Overflow,
    Queue_NoOverflow
}Queue_Overflow_state;

typedef struct{
  int32_t eIdx;
  int32_t dIdx;
  uint8_t *QBuff;
  int32_t QBuff_Max;
  int32_t eCount;
  int32_t dCount;
  Queue_Overflow_state QOverflow;
  uint32_t QBuff_IdxSize;
}Queue_t;

void Queue_Init(Queue_t *inst, uint8_t *pBuf, uint32_t IdxSize, uint32_t IdxMax);
void Queue_Enqueue(Queue_t *inst, uint8_t *InData);
void Queue_Dequeue(Queue_t *inst , uint8_t *OutData);
Queue_Emptyness_e Queue_isEmpty(Queue_t *inst);
Queue_Fullness_e Queue_isFull(Queue_t *inst);
void Queue_fullFlush(Queue_t *inst);
int32_t Queue_totalOccupied_Index(Queue_t *inst);
void Queue_setOverflow_State(Queue_t *inst, Queue_Overflow_state state);
Queue_Overflow_state Queue_getOverflow_State(Queue_t *inst);

#endif