#ifndef __PWM_H
#define __PWM_H

#include "common.h"

typedef enum{
    PWM_EvtAct_DoNothing = 0,
    PWM_EvtAct_Invert = 1,
    PWM_EvtAct_DriveLow = 2,
    PWM_EvtAct_DriveHigh = 3
}PWM_EventAction_e;

void PWM_Init(void);

#endif