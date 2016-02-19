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

static void reflowSM_Profiles();

static void reflowSM_Info();
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
static uint8_t updateWholeDisplay;

/* Profile STRINGS */
const char string_leaded[]  PROGMEM = {"Leaded  "};     // extra spaces allow for
const char string_pbFree[]  PROGMEM = {"Pb-Free "};     // not needing to clear any
const char string_custom[]  PROGMEM = {"Custom  "};     // characters for reprint.

PGM_P const profile_table[] PROGMEM =
{
    string_leaded,
    string_pbFree,
    string_custom
};

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
    lcd_gotoxy(passedCursor->x_position,0);
    lcd_putc(' ');
    lcd_gotoxy(passedCursor->x_position,1);
    lcd_putc(' ');
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
        lcd_gotoxy(2,0);
        lcd_puts("SOLDER REFLOW");
        lcd_gotoxy(1,1);
        lcd_puts("Firmware v 1.0");
    }
    else if (count == TICKS_PER_SEC << 2)    // 4 seconds
    {
        /* When 4 seconds has passed, change states. */
        reflow_pfnStateMachine = reflowSM_Main;
        updateWholeDisplay = 1;
        count = 0;
        return; // avoids count incrementing.
    }
    count++;
}

static void reflowSM_Main()
{
    static Cursor cursor = {.icon = '<', .x_position = 8, .row = 0};
    if (updateWholeDisplay)
    {
        lcd_clrscr();
        lcd_gotoxy(0,0);
        
        // maybe shorten this by making a function?
        //lcd_puts_p(string_profiles);
        lcd_puts_P("Profiles");
        lcd_gotoxy(0,1);
        //lcd_puts_p(string_info);
        lcd_puts_P("Info");
        lcd_gotoxy(12,0);
        //lcd_puts_p(string_mainSide);
        lcd_puts_P("MAIN");
       
        printCursor(&cursor);
        updateWholeDisplay = 0;
    }
    
    if (main_MASTER_CTRL_FLAG & UPDATE_ENCODER)
    {   /* Clear the UPDATE_ENCODER flag    */
        main_MASTER_CTRL_FLAG &= (~UPDATE_ENCODER);
        //updateWholeDisplay = 1;   // Not required: no scrolling. We just need to change the cursor.
        
        if (WasEncoderTurnedRIGHT() && cursor.row == 1)
        {
            cursor.row--;
            EncoderTurnAck();
        }
        else if (WasEncoderTurnedLEFT() && cursor.row == 0)
        {
            cursor.row++;
            EncoderTurnAck();
        }
        else if (WasEncoderPressed())
        {
            fnCode_type nextState;
            if (cursor.row == PROFILES)
                nextState = reflowSM_Profiles;
            else if (cursor.row == INFO)
                nextState = reflowSM_Info;
            
            reflow_pfnStateMachine =  nextState;
            updateWholeDisplay = 1;
            EncoderPushAck();
        }
        else
            EncoderTurnAck();   /* The encoder was turned, but past the limits of the display */
                                /* The flag gets cleared, but no action is taken.             */
        printCursor(&cursor);
    }
}

static void reflowSM_Profiles()
{
    static Cursor cursor = {.icon = '<', .x_position = 8, .row = 0};
    
    if (updateWholeDisplay)
    {
        updateWholeDisplay = 0;
        lcd_clrscr();
        lcd_gotoxy(9,0);
        lcd_puts_P("PROFILE");
        
    }
}

static void reflowSM_Info()
{
    
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