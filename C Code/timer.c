//*****************************************************************************
//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//*****************************************************************************
//
//        NAME: timer Functions
//
//    FILENAME: timer.c
//
//    DESIGNER: Nolbert Valverde
//
//     CREATED: 2/27/2014
//
// DESCRIPTION: This file contains the functions that work the timer.
//
//*****************************************************************************
//*****************************************************************************

//*****************************************************************************
//                         Include Files
//*****************************************************************************
#include <stdio.h>                    // for NULL
#include <sys/alt_irq.h>              // for irq support function
#include "system.h"                   // for QSYS defines
#include "nios_std_types.h"           // for standard embedded types

//*****************************************************************************
//                        Define symbolic constants
//*****************************************************************************
#define TIMER_CTRL_OFFSET      1
#define TIMER_PERIOD_L_OFFSET  2
#define TIMER_PERIOD_H_OFFSET  3

#define TIMER_TO_BITMASK   0x1
#define TIMER_CONTINUOUS   0x2
#define TIMER_START_ENABLE 0x4
#define TIMER_STOP_ENABLE  0x8

#define LEDR_ON 0x3FFFF
#define LEDG_ON 0xFF

#define TIMER_QUART_FREQ_H 0x000000BE
#define TIMER_QUART_FREQ_L 0x0000BC1F

#define SECOND  1
#define HALFSEC 2
#define QUARTER 3

#define TIMER_TIMEOUT 0x1

//*****************************************************************************
//                            Define private data
//*****************************************************************************
volatile uint32* seven_seg_base   = (uint32*)SEVEN_SEG_BASE;
volatile uint32* timerStatRegPtr  = (uint32*)TIMER_0_BASE;
volatile uint32* timerCntrlRegPtr = ((uint32*)TIMER_0_BASE + TIMER_CTRL_OFFSET);
volatile uint32* timerPeriodLPtr  = ((uint32*)TIMER_0_BASE + TIMER_PERIOD_L_OFFSET);
volatile uint32* timerPeriodHPtr  = ((uint32*)TIMER_0_BASE + TIMER_PERIOD_H_OFFSET);

int timerTimeLimit = 0;
uint32 timerTimeExpired = FALSE;
uint32 normalEnable = TRUE;
uint32 ledREnable = FALSE;
uint32 ledGEnable = FALSE;

volatile uint32* ledg_ptr         = (uint32*)LED_G_BASE;
volatile uint32* ledr_ptr         = (uint32*)LED_R_BASE;
//*****************************************************************************
//                           Define external data
//*****************************************************************************


//*****************************************************************************
//                             private functions
//*****************************************************************************

//----------------------------------------------------------------------------
// NAME: TIMER Countdown Isr
//
// DESCRIPTION:
//    This function will trigger every time the Timeout Bit of the interval
//    timer is triggered.  Based on which enable is on, the ISR will trigger
//    one of three if statements.  If normalEnable is on, the seven-segment
//    timer will count down every second.  If ledREnable is on, the red LEDs
//    will toggle on and off every half second.  If ledGEnable is on, the
//    green LEDs will toggle on and off every quarter second.
//
// INPUT:
//    context - the Altera ISR requires this. The context is a pointer used to pass context-specific information into the ISR.
//
// OUTPUT:
//    none
//
// RETURN:
//   none
//----------------------------------------------------------------------------
void timer_countdownIsr(void* context)
{
  static int time_count = 0;
  uint32 to_reg = 0;
  to_reg = *(timerStatRegPtr + TIMER_TO_BITMASK);

  if (TIMER_TIMEOUT == (to_reg & TIMER_TIMEOUT) && time_count == 4 && normalEnable == TRUE)
  {
    if (timerTimeLimit > 0)
    {
      timerTimeLimit--;
    }
    else
    {
      timerTimeExpired = TRUE;
      ledREnable = TRUE;
      normalEnable = FALSE;
    }
    time_count = 0;
  }
  else if (TIMER_TIMEOUT == (to_reg & TIMER_TIMEOUT) && ledREnable == TRUE)
  {
    if (time_count == 2)
    {
      *ledr_ptr = ~*ledr_ptr;
      time_count = 0;
    }
  }
  else if (TIMER_TIMEOUT == (to_reg & TIMER_TIMEOUT) && ledGEnable == TRUE)
  {
    *ledg_ptr = ~*ledg_ptr;
    time_count = 0;
  }
  timer_DecimalToBCD(timerTimeLimit);
  time_count++;
  *timerStatRegPtr = 0;
}

//----------------------------------------------------------------------------
// NAME: TIMER Decimal to BCD
//
// DESCRIPTION:
//    This function converts the input decimal number to a BCD for the seven
//    segment display.
//
// INPUT:
//   dec_num - the input decimal number to be converted to BCD
//
// OUTPUT:
//   none
//
// RETURN:
//   none
//----------------------------------------------------------------------------
void timer_DecimalToBCD(int dec_num)
{
  uint32 bcd = 0;
  uint32 loop_count = 0;
  while (dec_num != 0)
  {
    int shifted_num = dec_num / 10;
    int digit = dec_num - (shifted_num * 10);
    bcd = bcd | (digit <<(loop_count * 4));
    dec_num = shifted_num;
    loop_count++;
  }
  *seven_seg_base = bcd;
}



//*****************************************************************************
//                             public functions
//*****************************************************************************

//----------------------------------------------------------------------------
// NAME: TIMER Set Time Limit
//
// DESCRIPTION:
//    This function sets the time limit based on the input decimal number.
//
// INPUT:
//   time - the time limit to be set for the clock
//
// OUTPUT:
//   none
//
// RETURN:
//   none
//----------------------------------------------------------------------------
void timer_SetTimeLimit(int time)
{
  timerTimeExpired = FALSE;
  timerTimeLimit = time;
  timer_DecimalToBCD(time);
}

//----------------------------------------------------------------------------
// NAME: TIMER Start Timer
//
// DESCRIPTION:
//    This function starts up the timer.  The function has an input to
//    signify the type of frequency the clock needs to count: a second, a
//    half second, or a quarter second
//
// INPUT:
//   freq - the frequency of the clock.
//
// OUTPUT:
//   none
//
// RETURN:
//   none
//----------------------------------------------------------------------------
void timer_StartTimer(int freq)
{
  *timerCntrlRegPtr |= TIMER_START_ENABLE;
  *timerCntrlRegPtr |= TIMER_CONTINUOUS;
  *timerCntrlRegPtr &= ~TIMER_STOP_ENABLE;

  if (freq == SECOND)
  {
    normalEnable = TRUE;
    ledREnable = FALSE;
    ledGEnable = FALSE;
  }
  else if (freq == HALFSEC)
  {
    normalEnable = FALSE;
    ledREnable = TRUE;
    ledGEnable = FALSE;
  }
  else if (freq == QUARTER)
  {
    normalEnable = FALSE;
    ledREnable = FALSE;
    ledGEnable = TRUE;
  }
  timerTimeExpired = FALSE;
}

//----------------------------------------------------------------------------
// NAME: TIMER Stop Timer
//
// DESCRIPTION:
//    This function stops the timer.
//
// INPUT:
//   none
//
// OUTPUT:
//   none
//
// RETURN:
//   none
//----------------------------------------------------------------------------
void timer_StopTimer(void)
{
  *timerCntrlRegPtr |= TIMER_STOP_ENABLE;
  *timerCntrlRegPtr &= ~TIMER_START_ENABLE;
  *ledr_ptr = 0x0000;
  *ledg_ptr = 0x0000;
}

//----------------------------------------------------------------------------
// NAME: TIMER Configure Timer Interrupt
//
// DESCRIPTION:
//    This function sets up the TIMER interrupt before enabling it.
//
// INPUT:
//   none
//
// OUTPUT:
//   none
//
// RETURN:
//   none
//----------------------------------------------------------------------------
void timer_ConfigureTimerInterrupt(void)
{
  *timerCntrlRegPtr &= 0x0000;
  alt_ic_isr_register (TIMER_0_IRQ_INTERRUPT_CONTROLLER_ID, TIMER_0_IRQ, timer_countdownIsr, 0, 0);
}

//----------------------------------------------------------------------------
// NAME: TIMER Enable Timer Interrupt
//
// DESCRIPTION:
//    This function enables the TIMER interrupt.
//
// INPUT:
//   none
//
// OUTPUT:
//   none
//
// RETURN:
//   none
//----------------------------------------------------------------------------
void timer_EnableTimerInterrupt(void)
{
  uint32 stat_reg = *timerStatRegPtr;
  stat_reg &= ~TIMER_TO_BITMASK;
  *timerCntrlRegPtr |= TIMER_TO_BITMASK;
}

//----------------------------------------------------------------------------
// NAME: TIMER Disable Timer Interrupt
//
// DESCRIPTION:
//    This function disables the TIMER interrupt.
//
// INPUT:
//   none
//
// OUTPUT:
//   none
//
// RETURN:
//   none
//----------------------------------------------------------------------------
void timer_DisableTimerInterrupt(void)
{
  *timerCntrlRegPtr |= TIMER_STOP_ENABLE;
  *timerCntrlRegPtr &= ~TIMER_START_ENABLE;
  *ledr_ptr = 0x0000;
  *ledg_ptr = 0x0000;
  *timerCntrlRegPtr &= 0x0000;
}

//----------------------------------------------------------------------------
// NAME: TIMER Is Timer Expired
//
// DESCRIPTION:
//    This function sends the value, timerTimeExpired, to see if the timer
//    has expired.
//
// INPUT:
//   none
//
// OUTPUT:
//   timerTimeExpired - the value that toggles on when the timer has reached
//   zero
//
// RETURN:
//   uint32
//----------------------------------------------------------------------------
uint32 timer_IsTimerExpired(void)
{
  return timerTimeExpired;
}
