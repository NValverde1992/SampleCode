//*****************************************************************************
//****************************  C Source Code  ********************************
//*********  Copyright 2014, Rochester Institute of Technology  ***************
//*****************************************************************************
//
//  DESIGNER NAME:  Nolbert Valverde
//
//       LAB NAME:  Using Program Controlled Input and Output
//
//      FILE NAME:  lab_4.c
//
//        CREATED:  9/30/2014
//
//    DESCRIPTION:  This is the main program file.  This contains the CodeBreaker
//                  main function which utilizes states to get to the different
//                  parts of the game.  This also contains the two special helper
//                  functions that generate the secret code and compares a user
//                  input with the secret code.
//
//*****************************************************************************
//*****************************************************************************


//*****************************************************************************
//                    Include Files
//*****************************************************************************
#include <stdio.h>                    // for NULL
#include <sys/alt_irq.h>              // for irq support function
#include "system.h"                   // for QSYS defines
#include "nios_std_types.h"           // for standard embedded types
#include "UART.h"                     // for UART Functions
#include "display.h"                  // for display functions
#include "pio.h"                      // for pio funcitons
#include "timer.h"


//*****************************************************************************
//                    Define Symbolic Constants
//*****************************************************************************
#define NUM_OF_COLORS_INCODE  4

#define eGAME_IDLE      0
#define eINIT_GAME      1
#define eREQUEST_GUESS  2
#define eWAITING_4_USER 3
#define eWIN_GAME       4
#define eLOSE_GAME      5
#define eWAIT_4_KEY1    6
#define eEND_GAME       7

#define KEY1 1
#define KEY2 2

#define SECOND  1
#define HALFSEC 2
#define QUARTER 3

#define TIME_OUT_PERIOD 60
//*****************************************************************************
//                    Define Global Variables
//*****************************************************************************
uint8  compared_answer[NUM_OF_COLORS_INCODE + 1] = "----";

//----------------------------------------------------------------------------
// NAME: Generate Secret Code
//
// DESCRIPTION:
//    This function creates a random secret code using the random function
//    to output a value between 0 and 5 and setting the color based on the
//    output number.  There is a check routine to see if the random function
//    outputs a number already stored in the code
//
// INPUT:
//   code - the value that the code is to be sent in
//
// OUTPUT:
//   none
//
// RETURN:
//   none
//----------------------------------------------------------------------------
void GenerateSecretCode(uint8* code)
{
  int i = 0;
  int j = 0;
  int same_value = FALSE;
  int random_value = 0;
  uint8 rand_code[NUM_OF_COLORS_INCODE + 1] = "----";
  for (i = 0; i < NUM_OF_COLORS_INCODE; i++)
  {
    random_value = rand()%6;
    for (j = 0; j < NUM_OF_COLORS_INCODE; j++)
    {
      if (rand_code[j] == random_value)
      {
        same_value = TRUE;
      }
    }
    if (!same_value)
    {
      rand_code[i] = random_value;
    }
    else
    {
      i--;
    }
    same_value = FALSE;
  }
  for (i = 0; i < NUM_OF_COLORS_INCODE; i++)
  {
    switch(rand_code[i])
    {
    case 0:
      code[i] = 'G';
      break;
    case 1:
      code[i] = 'B';
      break;
    case 2:
      code[i] = 'R';
      break;
    case 3:
      code[i] = 'O';
      break;
    case 4:
      code[i] = 'Y';
      break;
    case 5:
      code[i] = 'W';
      break;
    }
  }
}


//----------------------------------------------------------------------------
// NAME: Compare Code
//
// DESCRIPTION:
//    This function compares the secret code with the user input.
//
// INPUT:
//   guess - the user's guess
//   answer - the secret code
//
// OUTPUT:
//   none
//
// RETURN:
//   none
//----------------------------------------------------------------------------
int compareCode(uint8* guess, uint8* answer)
{
  int i;
  int j;
  int match_counter = 0;
  for (i = 0; i < NUM_OF_COLORS_INCODE; i++)
  {
    compared_answer[i] = '-';
  }
  for (i = 0; i < NUM_OF_COLORS_INCODE; i++)
  {
    for (j = 0; j < NUM_OF_COLORS_INCODE; j++)
    {
      if ((guess[i] == answer[j]) && (i == j))
      {
        compared_answer[i] = 'P';
        match_counter++;
      }
      else if ((guess[i] == answer[j]) && (i != j))
      {
        if (compared_answer[i] != 'P')
        {
          compared_answer[i] = 'C';
        }
      }
      else
      {
        if ((compared_answer[i] != 'P') && (compared_answer[i] != 'C'))
        {
          compared_answer[i] = '-';
        }
      }
    }
  }
  return match_counter;
}

int main(void)

{
  pio_ConfigInterrupt();
  pio_EnableInterrupt();

  uart_ConfigInterrupt();
  uart_EnableInterrupt();

  timer_ConfigureTimerInterrupt();
  timer_EnableTimerInterrupt();

  uint32 game_done = FALSE;
  uint8  secret_code[NUM_OF_COLORS_INCODE + 1] = "----";
  uint8  user_input[NUM_OF_COLORS_INCODE + 1] = "----";

  uint8 sPresentState = eGAME_IDLE;

  srand(time(NULL));
  do
  {
    switch (sPresentState)
    {
    case eGAME_IDLE:
      display_DisplayWelcomeMsg();
      timer_StopTimer();
      while (!uart_IsUserInputReady());
      {
        uart_GetUserInput(&user_input[0], NUM_OF_COLORS_INCODE);

        if (0 == strcmp((char*)user_input, "HELP"))
        {
          display_DisplayHelpMsg();
          sPresentState = eWAIT_4_KEY1;
        }/* if */

        else if (0 == strcmp(user_input, "PLAY"))
        {
          sPresentState = eINIT_GAME;
        }

        else if (0 == strcmp(user_input, "EXIT"))
        {
          sPresentState = eEND_GAME;
        }

        else
        {
          display_DisplayMsg("\nIncorrect Response\n\n");
        }
        uart_ClearUserInput();
      }
     break;



    case eINIT_GAME:
        pio_ClearKeyPressedFlag(KEY1);
        pio_ClearKeyPressedFlag(KEY2);

        timer_SetTimeLimit(TIME_OUT_PERIOD);

        GenerateSecretCode(&secret_code[0]);

        #if(DEBUG_ENABLE)
          display_DisplayMsg("Secret Code = ");
          display_DisplayMsg(secret_code);
          display_DisplayMsg("\n");
        #endif

        sPresentState = eREQUEST_GUESS;
        break;



      case eREQUEST_GUESS:
        pio_ClearKeyPressedFlag(KEY1);
        pio_ClearKeyPressedFlag(KEY2);
        timer_StartTimer(SECOND);
        display_DisplayMsg("\n\nEnter Your guess:");
        sPresentState = eWAITING_4_USER;
        break;



      case eWAITING_4_USER:
        // if KEY1 pressed then restart game
        if (pio_IsKey1Pressed())
        {
          timer_StopTimer();
          sPresentState = eGAME_IDLE;
        } /* if */

        // if KEY2 pressed then check user_input
        if (pio_IsKey2Pressed())
        {
          timer_StopTimer();
          uart_GetUserInput(&user_input[0], NUM_OF_COLORS_INCODE);

          if (NUM_OF_COLORS_INCODE == compareCode(user_input, secret_code))
          {
            sPresentState = eWIN_GAME;
          }  /*if correct guess*/

          else
          {
            display_DisplayMsg("\n\nThat guess is incorrect.  Your guess was:  ");
            display_DisplayMsg(user_input);
            display_DisplayMsg("\nThis is the hint from your guess:  ");
            display_DisplayMsg(compared_answer);
            timer_SetTimeLimit(TIME_OUT_PERIOD);
            sPresentState = eREQUEST_GUESS;
          }  /*else bad guess*/

        } /* if */

        // if timer expired then user loses game
        if (timer_IsTimerExpired() )
        {
          timer_StopTimer();
          pio_ClearKeyPressedFlag(KEY1);
          sPresentState = eLOSE_GAME;
        } /* if timer expired */
        break;



      case eWIN_GAME:
        timer_StartTimer(QUARTER);
        display_DisplayWinnerMsg();

        sPresentState = eWAIT_4_KEY1;
        break;



      case eLOSE_GAME:
        display_DisplayLoserMsg();
        timer_StartTimer(HALFSEC);
        display_DisplayMsg("The secret code was ");
        display_DisplayMsg((char*)secret_code);
        sPresentState = eWAIT_4_KEY1;
        break;



      case eWAIT_4_KEY1:
        pio_ClearKeyPressedFlag(KEY1);
        display_DisplayMsg("\n\nPress KEY1 to return to the main menu.\n");

        while (!pio_IsKey1Pressed());
        sPresentState = eGAME_IDLE;
        break;



      case eEND_GAME:
        timer_StopTimer();
        timer_DisableTimerInterrupt();

        display_DisplayEndMsg();
        game_done = TRUE;
        break;
    } /* switch */

  } while (!game_done);

  return 0;

} /* main */
