#ifndef __TIMER_H
#define __TIMER_H

#include "common.h"


typedef enum{
    TIM_ConcatenatedMode = 0x0,
    TIM_RTCMode = 0x1,
    TIM_NonConcatenatedMode = 0x4
}TIM_BlockMode;

typedef enum{
    TIM_OneShot = 1,
    TIM_Periodic = 2,
    TIM_Capture = 3
}TIM_Mode;

typedef enum{
    TIM_CountDown = 0,
    TIM_CountUp = 1
}TIM_Direction_e;

typedef enum{
    TIM_ReloadSameCycle = 0,
    TIM_ReloadNextCycle = 1
}TIM_Reload_e;

void TIM_Init(void);

#endif