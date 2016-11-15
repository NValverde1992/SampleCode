//*****************************************************************************
//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//*****************************************************************************
//
//        NAME: pio Definitions
//
//    FILENAME: pio.h
//
//    DESIGNER: Nolbert Valverde
//
//     CREATED: 2/27/2014
//
// DESCRIPTION: This file contains the definitions of standard data types used
//              in pio.c.
//
//*****************************************************************************
//*****************************************************************************

#ifndef PIO_MOD_H_
#define PIO_MOD_H_

#include "nios_std_types.h"           // for standard embedded types

void pio_ClearKeyPressedFlag(int KEY);
uint32 pio_IsKey1Pressed(void);
uint32 pio_IsKey2Pressed(void);
void pio_ConfigInterrupt(void);
void pio_EnableInterrupt(void);

#endif /*PIO_MOD_H_*/
