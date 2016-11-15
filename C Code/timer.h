//*****************************************************************************
//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//*****************************************************************************
//
//        NAME: Timer Definitions
//
//    FILENAME: timer.h
//
//    DESIGNER: Nolbert Valverde
//
//     CREATED: 2/27/2014
//
// DESCRIPTION: This file contains the definitions of standard data types used
//              in timer.c.
//
//*****************************************************************************
//*****************************************************************************
#ifndef TIMER_MOD_H_
#define TIMER_MOD_H_

#include "nios_std_types.h"           // for standard embedded types

void timer_SetTimeLimit(int time);
void timer_StartTimer(int freq);
void timer_StopTimer(void);
void timer_ConfigureTimerInterrupt(void);
void timer_EnableTimerInterrupt(void);
void timer_DisableTimerInterrupt(void);
uint32 timer_IsTimerExpired(void);

#endif /*TIMER_MOD_H_*/
