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
/*------------------------------------------------------------------------*/
static void reflowSM_Profiles();

static void reflowSM_Info();
/*------------------------------------------------------------------------*/
static void reflowSM_ProfBack();

static void reflowSM_Leaded();

static void reflowSM_PbFree();

static void reflowSM_Custom();
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

/* PROGRAM MEMORY STRINGS */
const char string_back[]    PROGMEM = {"BACK    "};
    
const char string_leaded[]  PROGMEM = {"Leaded  "};     // extra spaces allow for
const char string_pbFree[]  PROGMEM = {"Pb-Free "};     // not needing to clear any
const char string_custom[]  PROGMEM = {"Custom  "};     // characters for reprint.

/*------------------------------------------------------------------------*/
/* PROFILE MENU     */

/* Profile STRINGS  */
PGM_P const profile_table[] PROGMEM =
{
    string_back,
    string_leaded,
    string_pbFree,
    string_custom
};


fnCode_type reflow_Profile_Array[] =
{   
    reflowSM_ProfBack,

    reflowSM_Leaded,

    reflowSM_PbFree,

    reflowSM_Custom
};
/*------------------------------------------------------------------------*/
/*


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
    lcd_gotoxy(passedCursor->x_position,0);
    lcd_putc(' ');
    lcd_gotoxy(passedCursor->x_position,1);
    lcd_putc(' ');
    lcd_gotoxy(passedCursor->x_position,passedCursor->row);
    lcd_putc(passedCursor->icon);
}


static void printLines(uint8_t passedIndex, PGM_P const string_table[])
{
    char line0[10];
    char line1[10];
    strcpy_P(line0, (PGM_P)pgm_read_word(&(string_table[passedIndex])) );
    strcpy_P(line1, (PGM_P)pgm_read_word(&(string_table[passedIndex+1])) );
    lcd_gotoxy(0,0);
    lcd_puts(line0);
    lcd_gotoxy(0,1);
    lcd_puts(line1);
    
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
/*------------------------------------------------------------------------*/
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
            cursor.row = 0;
            EncoderTurnAck();
        }
        else if (WasEncoderTurnedLEFT() && cursor.row == 0)
        {
            cursor.row = 1;
            EncoderTurnAck();
        }
        else if (WasEncoderPressed())
        {
            fnCode_type nextState;
            if (cursor.row == PROFILES)
                nextState = reflowSM_Profiles;
            else if (cursor.row == INFO)
                nextState = reflowSM_Info;
            
            reflow_pfnStateMachine = nextState;
            updateWholeDisplay = 1;
            EncoderPushAck();
        }
        
        EncoderTurnAck();      /* The encoder was turned, but past the limits of the display */
        printCursor(&cursor);  /* The flag gets cleared, but no action is taken.             */
    }
}
/*------------------------------------------------------------------------*/
static void reflowSM_Profiles()
{
    static Cursor cursor = {.icon = '<', .x_position = 8, .row = 0};
    static uint8_t index = 0;
    
    #if 0
    if (!(IsEncoderReleased()))     // wait for the encoder to be released
    {
        break;
    }
    
    if (updateWholeDisplay)
    {
        updateWholeDisplay = 0;
        lcd_clrscr();
        printLines(index, profile_table);
        printCursor(&cursor);
        
        lcd_gotoxy(9,0);
        lcd_puts_P("PROFILE");
    }
    
    if (main_MASTER_CTRL_FLAG & UPDATE_ENCODER)
    {   /* Clear the UPDATE_ENCODER flag    */
        main_MASTER_CTRL_FLAG &= (~UPDATE_ENCODER);
    
        if (WasEncoderTurnedRIGHT())
        {
            if (cursor.row == 1)
                cursor.row = 0;
            else if (index > 0)
            {
                index--;
                printLines(index, profile_table);
            }
        
        }
        else if (WasEncoderTurnedLEFT())
        {
            if (cursor.row == 0)
                cursor.row = 1;
            else if (index < PROFILE_ELEMENTS - 2)
            {
               index++;
               printLines(index, profile_table);
            }
        }
        else if (WasEncoderPressed())
        {
            
        }
    EncoderTurnAck();
    printCursor(&cursor);
    }   // old if-else style 'state-machine'
    #endif // updated switch-case state machine listed below.
    
    static profileSwitch currentState = WAIT_FOR_RELEASE;
    profileSwitch nextState = currentState;
    
    switch (currentState)
    {
        case WAIT_FOR_RELEASE:
        if (IsEncoderReleased())
        {
            lcd_clrscr();
            printLines(index, profile_table);
            printCursor(&cursor);
            
            lcd_gotoxy(9,0);
            lcd_puts_P("PROFILE");
            
            nextState = WAIT_FOR_ENCODER;
        }
        break;
/*------------------------------------------------------------------------*/        
        case WAIT_FOR_ENCODER:
        if (main_MASTER_CTRL_FLAG & UPDATE_ENCODER)
        {   /* Clear the UPDATE_ENCODER flag    */
            main_MASTER_CTRL_FLAG &= (~UPDATE_ENCODER);
            
            if (WasEncoderTurnedRIGHT())
            {
                if (cursor.row == 1)
                cursor.row = 0;
                else if (index > 0)
                {
                    index--;
                    printLines(index, profile_table);
                }
                
            }
            else if (WasEncoderTurnedLEFT())
            {
                if (cursor.row == 0)
                cursor.row = 1;
                else if (index < PROFILE_ELEMENTS - 2)
                {
                    index++;
                    printLines(index, profile_table);
                }
            }
            else if (WasEncoderPressed())
            {
                EncoderPushAck();
                nextState = CHANGE_MENU;
            }
            EncoderTurnAck();
            printCursor(&cursor);
        }
        break;      
/*------------------------------------------------------------------------*/
        case CHANGE_MENU:   // when leaving this state to go to outer next statemachine,
                            // currentState needs to get put back to WAIT_FOR_RELEASE!
        reflow_pfnStateMachine = reflow_Profile_Array[index + cursor.row];
        nextState = WAIT_FOR_RELEASE;
        /* reset variables */
        index = 0;
        cursor.row = 0;
        break;
    }
    currentState = nextState;
    
            
}
/*------------------------------------------------------------------------*/
static void reflowSM_Info()
{
    //char buffer[16];
    
}

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/
static void reflowSM_ProfBack()
{
    reflow_pfnStateMachine = reflowSM_Main;
}

static void reflowSM_Leaded()
{
    
}

static void reflowSM_PbFree()
{
    
}

static void reflowSM_Custom()
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