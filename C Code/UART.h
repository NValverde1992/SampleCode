//*****************************************************************************
//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//*****************************************************************************
//
//        NAME: UART Definitions
//
//    FILENAME: UART.h
//
//    DESIGNER: Nolbert Valverde
//
//     CREATED: 2/25/2014
//
// DESCRIPTION: This file contains the definitions of standard data types used
//              in UART.c.
//
//*****************************************************************************
//*****************************************************************************

#ifndef UART_MOD_H_
#define UART_MOD_H_

#include "nios_std_types.h"           // for standard embedded types

void uart_SendString (char* msg);
void uart_SendByte (uint8 byte);
void uart_GetUserInput(uint8* user_inval, uint8 length);
void uart_EnableInterrupt(void);
void uart_ConfigInterrupt(void);
uint32 uart_IsUserInputReady(void);
void uart_ClearUserInput(void);

#endif /*UART_MOD_H_*/
