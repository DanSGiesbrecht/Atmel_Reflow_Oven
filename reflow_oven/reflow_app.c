/**************************************************************************/
/*
 * reflow_app.c
 *
 *   Created: 18/02/2016 4:41:00 PM
 *    Author: dsgiesbrecht
 *   Version: v1.0
 *  
 * Description:
 *      Reflow Application
 *          -Consistently controls the temperature of the oven for
 *              repeatable solder reflow.
 *
 * Important Notes: none yet.
 */
/**************************************************************************/
/**************************************************************************/
#include "reflow_app.h"
#include "ATMEGA328P_reflow_config.h"
#include "lcd.h"
#include "quad_encoder.h"


/**************************************************************************/
/*      PROTOTYPES (excluded from header)                                 */
/*------------------------------------------------------------------------*/
/*      Private Functions                                                 */
/*------------------------------------------------------------------------*/
static void reflowSM_Boot();

static void reflowSM_Main();
/**************************************************************************/
/*      State Machines  (also private)                                    */
/*      This is here and not in the header because static function
 *      prototyptes in a header file included elsewhere causes a
 *      whole mess of problems.  This allows the functions to work
 *      as intended: locally in this file.
 */
/**************************************************************************/




/**************************************************************************/
/*      Global Variables that are !exteral!                               */

extern volatile uint16_t   main_MASTER_CTRL_FLAG; 


/**************************************************************************/
/*      Global Variable definitions with scope limited to this local
 *      application.  Variables shall start with the name 'ENC'
 *      and be declared as static.
 */
/**************************************************************************/
/*      State Machine Function Pointer                                    */
static fnCode_type reflow_pfnStateMachine;
/*------------------------------------------------------------------------*/


/**************************************************************************/
/**************************************************************************/
/*      Function Definitions                                              */
/**************************************************************************/

/*------------------------------------------------------------------------*/
/*      Public Functions                                                  */
/*------------------------------------------------------------------------*/




/*------------------------------------------------------------------------*/
/*      Protected Functions                                               */
/*------------------------------------------------------------------------*/
void reflow_Initialize()
{
    reflow_pfnStateMachine = reflowSM_Boot;
}

void reflow_ActiveState()
{
    reflow_pfnStateMachine();
}


/*------------------------------------------------------------------------*/
/*      Private Functions                                                 */
/*------------------------------------------------------------------------*/

/*  UNFINISHED
static void printString(uint8t location_x, uint8_t location_y, char* stringToPrint)
{
    lcd_gotoxy(location_x, location_y);
    lcd_puts(stringToPrint);
}
*/

static void printCursor(Cursor* passedCursor)
{
    lcd_gotoxy(passedCursor->x_position,passedCursor->row);
    lcd_putc(passedCursor->icon);
}

/**************************************************************************/
/*      State Machine Functions  (also private)                           */
/**************************************************************************/

static void reflowSM_Boot()
{
    static uint16_t count = 0;
    if (count == 0)
    {
        /* Print the boot screen */
        lcd_clrscr();
        lcd_gotoxy(0,2);
        lcd_puts("SOLDER REFLOW");
        lcd_gotoxy(1,1);
        lcd_puts("Firmware v 1.0");
    }
    else if (count == TICKS_PER_SEC << 2)    // 4 seconds
    {
        /* When 4 seconds has passed, change states. */
        reflow_pfnStateMachine = reflowSM_Main;
        count = 0;
        return; // avoids count incrementing.
    }
    count++;
}

static void reflowSM_Main()
{
    uint8_t updateDisplay = 1;
    static Cursor cursor = {.icon = '<', .x_position = 8, .row = 0};
    if (updateDisplay)
    {
        lcd_clrscr();
        lcd_gotoxy(0,0);
        
        /* maybe shorten this by making a function? */
        lcd_puts_p(string_profiles);
        lcd_gotoxy(0,1);
        lcd_puts_p(string_info);
        lcd_gotoxy(12,0);
        lcd_puts_p(string_mainSide);
        
        printCursor(&cursor);
        updateDisplay = 0;
    }
    
    if (main_MASTER_CTRL_FLAG & UPDATE_ENCODER)
    {
        updateDisplay = 1;
        
        if (WasEncoderTurnedLEFT() && cursor.row > 0)
        {
            cursor.row--;
            EncoderTurnAck();
        }
        else if (WasEncoderTurnedRIGHT() && cursor.row < MAIN_ELEMENTS)
        {
            cursor.row++;
            EncoderTurnAck();
        }
        else
        {
            //reflow_pfnStateMachine = /* next state*/;
            EncoderPushAck();
        }
    }
    
}




/*
profileItems selection = BACK_MAIN;
uint8_t updateDisplay = 0;
char* line1;
char* line2;
*/

/**************************************************************************/
/*      END OF FILE                                                       */
/**************************************************************************/