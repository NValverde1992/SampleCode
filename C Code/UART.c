//*****************************************************************************
//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//*****************************************************************************
//
//        NAME: UART Functions
//
//    FILENAME: UART.c
//
//    DESIGNER: Nolbert Valverde
//
//     CREATED: 2/20/2014
//
// DESCRIPTION: This file contains the functions that work the UART
//
//*****************************************************************************
//*****************************************************************************

//*****************************************************************************
//                         Include Files
//*****************************************************************************
#include <stdio.h>                    // for NULL
#include <sys/alt_irq.h>              // for irq support function
#include "system.h"                   // for QSYS defines
#include "uart.h"                   // for QSYS defines
#include "nios_std_types.h"           // for standard embedded types
#include "pio.h"





//*****************************************************************************
//                        Define symbolic constants
//*****************************************************************************

#define JTAG_DATA_REG_OFFSET          0
#define JTAG_CNTRL_REG_OFFSET         1
#define JTAG_UART_INT_ENABLE_BITMASK  1

#define JTAG_UART_WSPACE_MASK         0xFFFF0000
#define JTAG_UART_RV_BIT_MASK         0x00008000
#define JTAG_UART_DATA_MASK           0x000000FF

#define BACKSPACE 0x7f
#define RETURN 0xa


//*****************************************************************************
//                            Define private data
//*****************************************************************************

volatile uint32* uartDataRegPtr   = (uint32*)JTAG_UART_0_BASE;
volatile uint32* uartCntrlRegPtr  = ((uint32*)JTAG_UART_0_BASE +
                                             JTAG_CNTRL_REG_OFFSET);
uint8   uartStoreValue[5];
uint8*  uartStorePtr;
uint32  userInputReady = FALSE;
static uint32 store_slot = 0;

//*****************************************************************************
//                           Define external data
//*****************************************************************************


//*****************************************************************************
//                             private functions
//*****************************************************************************

//----------------------------------------------------------------------------
// NAME: Check UART receive Buffer Isr
//
// DESCRIPTION:
//    This function will check to see if the receive buffer has any data
//    in it. If there is data in the UART, it is read at the same time as
//    the RV bit. If RV is set then the data is stripped out and echoed back
//    UART.
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
void uart_RecvBufferIsr (void* context)
{
  uint32 valid;
  uint32 data_reg;
  uint8 character;

  data_reg = *uartDataRegPtr;
  valid = JTAG_UART_RV_BIT_MASK & data_reg;

  if (valid != 0)
  {
    character = (uint8)data_reg & JTAG_UART_DATA_MASK;
    if ((character >= 'a') && (character <= 'z'))
    {
       character -= 32;
    } /* if */

    switch(character)
    {
      case BACKSPACE:
        character = '\b';
        if (store_slot != 0)
        {
          uart_SendByte(character);
          uart_SendByte(' ');
          uart_SendByte(character);
          uartStoreValue[store_slot] = (uint8)NULL;
          store_slot--;
        }
      break;

      case RETURN:
        uart_SendByte(character);
        userInputReady = TRUE;
        uartStoreValue[store_slot] = (uint8)NULL;
        store_slot = 0;
      break;

      default:
        if (store_slot == 0)
        {
          int i = 0;
          for (i = 0; i < 5; i++)
          {
            uartStoreValue[i] = 0;
          }
        }

        if (store_slot < 4)
        {
          uart_SendByte(character);
          uartStoreValue[store_slot] = character;
          store_slot++;
        }
      break;
    }
  }
  else
  {
    uart_SendString("The input character is invalid.");
  }
} /* uart_RecvBufferIsr */

//*****************************************************************************
//                             public functions
//*****************************************************************************

//----------------------------------------------------------------------------
// NAME: UART Send a String
//
// DESCRIPTION:
//    This function will output a string (NULL terminated) to the UART.
//
// INPUT:
//    msg  - contains the string to send to the UART
//
// OUTPUT:
//   none
//
// RETURN:
//   none
//----------------------------------------------------------------------------
void uart_SendString (char* msg)
{
  uint8 character = (uint8)NULL;

    // get first character of string
    character = *msg++;

    while (character != (uint8)NULL)
    {

      // as long as character is not NULL then keep looping
      // call uart_SendByte with character
      uart_SendByte (character);
      // Get next character
      character = *msg++;

    } /* while */
} /* uart_SendString */


//----------------------------------------------------------------------------
// NAME: UART Send a byte
//
// DESCRIPTION:
//    This function will send a single byte to the UART. Before the data is
//    sent to the UART, this function makes sure there is room in the buffer.
//
// INPUT:
//    byte  - contains the byte to send to the UART
//
// OUTPUT:
//   none
//
// RETURN:
//   none
//----------------------------------------------------------------------------
void uart_SendByte (uint8 byte)
{
  uint8 control_reg_value = 0;
  do
   {
     // read control register and mask out all bit but WPSPACE
    if ((JTAG_UART_WSPACE_MASK & *uartCntrlRegPtr) == 0)
    {
      control_reg_value = 0;
    }
    else
    {
      control_reg_value = 1;
    }

   } while (0 == control_reg_value);
   // now buffer has room so write the data to UART buffer
   *uartDataRegPtr = byte;
} /* uart_SendByte */

//----------------------------------------------------------------------------
// NAME: UART Get User Input
//
// DESCRIPTION:
//    This function will send the value stored inside the UART into the main
//    function.
//
// INPUT:
//    user_inval - pointer that points to the address of the input
//    length - the length of the input address
//
// OUTPUT:
//   none
//
// RETURN:
//   none
//----------------------------------------------------------------------------
void uart_GetUserInput(uint8* user_inval, uint8 length)
{
  int i;
  for (i = 0; i < length; i++)
  {
    user_inval[i] = uartStoreValue[i];
  }
  store_slot = 0;
}

//----------------------------------------------------------------------------
// NAME: UART Is User Input Ready
//
// DESCRIPTION:
//    This function sends out the value of userInputReady to signify if the
//    user is ready to send his input.
//
// INPUT:
//   none
//
// OUTPUT:
//   userInputReady - the value that toggles on when the user is ready
//
// RETURN:
//   uint32
//----------------------------------------------------------------------------
uint32 uart_IsUserInputReady(void)
{
  return userInputReady;
}

//----------------------------------------------------------------------------
// NAME: UART Clear User Input
//
// DESCRIPTION:
//    This function sets the value of userInputReady to "FALSE" to clear
//    the previous input.
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
void uart_ClearUserInput(void)
{
  userInputReady = FALSE;
}

//----------------------------------------------------------------------------
// NAME: UART Configure Interrupt
//
// DESCRIPTION:
//    This function sets up the UART interrupt before enabling it.
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
void uart_ConfigInterrupt(void)
{
  alt_ic_isr_register (JTAG_UART_0_IRQ_INTERRUPT_CONTROLLER_ID, JTAG_UART_0_IRQ, uart_RecvBufferIsr, 0, 0); // used for 2nd part when interrupts are enabled
}

//----------------------------------------------------------------------------
// NAME: UART Enable Interrupt
//
// DESCRIPTION:
//    This function enables the UART interrupt.
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
void uart_EnableInterrupt(void)
{
  *uartCntrlRegPtr = JTAG_UART_INT_ENABLE_BITMASK;
}
