//*****************************************************************************
//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//*****************************************************************************
//
//        NAME: pio Functions
//
//    FILENAME: pio.c
//
//    DESIGNER: Nolbert Valverde
//
//     CREATED: 2/27/2014
//
// DESCRIPTION: This file contains the functions that work the input and output
//              buttons.
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
#include "UART.h"


//*****************************************************************************
//                        Define symbolic constants
//*****************************************************************************
#define PIO_INT_ENABLE_BITMASK 1

#define PIO_INT_OFFSET 2
#define PIO_EDG_CAP_OFFSET 3

#define KEY1 0x1
#define KEY2 0x2



//*****************************************************************************
//                            Define private data
//*****************************************************************************
uint32 pioKey1Pressed = FALSE;
uint32 pioKey2Pressed = FALSE;

volatile uint32* pioPtr  = ((uint32*)KEY1_KEY2_BASE);


//*****************************************************************************
//                           Define external data
//*****************************************************************************


//*****************************************************************************
//                             private functions
//*****************************************************************************

//----------------------------------------------------------------------------
// NAME: PIO Pushbutton Isr
//
// DESCRIPTION:
//    This function will trigger every time either Key 1 or Key 2 on the
//    DE2 board is pressed.  Based on which one is pressed, one of the two
//    flags will be raised.
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
void pio_PushBIsr (void* context)
{
  uint32 pio_reg = 0;

  pioKey1Pressed = FALSE;
  pioKey2Pressed = FALSE;

  pio_reg = *(pioPtr + PIO_EDG_CAP_OFFSET);

  if (KEY1 == (pio_reg & KEY1))
  {
    pioKey1Pressed = TRUE;
  }
  if (KEY2 == (pio_reg & KEY2))
  {
    pioKey2Pressed = TRUE;
  }
  *(pioPtr + PIO_EDG_CAP_OFFSET) = pio_reg;
}



//*****************************************************************************
//                             public functions
//*****************************************************************************

//----------------------------------------------------------------------------
// NAME: PIO Clear Key Pressed Flag
//
// DESCRIPTION:
//    This function clears the flag of the key value that's been inputted.
//
// INPUT:
//    KEY - the key number to be reset
//
// OUTPUT:
//    none
//
// RETURN:
//   none
//----------------------------------------------------------------------------
void pio_ClearKeyPressedFlag(int KEY)
{
  if (KEY == 1)
  {
    pioKey1Pressed = FALSE;
  }
  else if (KEY == 2)
  {
    pioKey2Pressed = FALSE;
  }
}

//----------------------------------------------------------------------------
// NAME: PIO Is KEY1 Pressed
//
// DESCRIPTION:
//    This function sends the value, pioKey1Pressed, to see if KEY1 has been
//    pressed.
//
// INPUT:
//   none
//
// OUTPUT:
//   pioKey1Pressed - the value that toggles on when KEY1 is pressed
//
// RETURN:
//   uint32
//----------------------------------------------------------------------------
uint32 pio_IsKey1Pressed(void)
{
  return pioKey1Pressed;
}

//----------------------------------------------------------------------------
// NAME: PIO Is KEY2 Pressed
//
// DESCRIPTION:
//    This function sends the value, pioKey2Pressed, to see if KEY2 has been
//    pressed.
//
// INPUT:
//   none
//
// OUTPUT:
//   pioKey2Pressed - the value that toggles on when KEY2 is pressed
//
// RETURN:
//   uint32
//----------------------------------------------------------------------------
uint32 pio_IsKey2Pressed(void)
{
  return pioKey2Pressed;
}

//----------------------------------------------------------------------------
// NAME: PIO Configure Interrupt
//
// DESCRIPTION:
//    This function sets up the PIO interrupt before enabling it.
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
void pio_ConfigInterrupt(void)
{
  alt_ic_isr_register (KEY1_KEY2_IRQ_INTERRUPT_CONTROLLER_ID, KEY1_KEY2_IRQ, pio_PushBIsr, 0, 0);
}

//----------------------------------------------------------------------------
// NAME: PIO Enable Interrupt
//
// DESCRIPTION:
//    This function enables the PIO interrupt.
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
void pio_EnableInterrupt(void)
{
  *(pioPtr + PIO_EDG_CAP_OFFSET) = (KEY1|KEY2);
  *(pioPtr + PIO_INT_OFFSET)     = (KEY1|KEY2);
}
