//*****************************************************************************
//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//*****************************************************************************
//
//         NAME: display Definitions
//
//     FILENAME: display.h
//
//     DESIGNER: Nolbert Valverde
//
//      CREATED: 2/27/2014
//
//  DESCRIPTION: This file contains the definitions of standard data types used
//               in display.c.
//
//*****************************************************************************
//*****************************************************************************
#ifndef DISPLAY_MOD_H_
#define DISPLAY_MOD_H_

#include "nios_std_types.h"           // for standard embedded types

void display_DisplayWelcomeMsg(void);
void display_DisplayHelpMsg(void);
void display_DisplayWinnerMsg(void);
void display_DisplayLoserMsg(void);
void display_DisplayMsg(char* message);
void display_DisplayEndMsg(void);

#endif /*DISPLAY_MOD_H_*/

