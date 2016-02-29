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
#include <stdlib.h>
#include "reflow_app.h"
#include "ATMEGA328P_reflow_config.h"
#include "lcd.h"
#include "quad_encoder.h"
#include "HeaterControl.h"
#include "TemperatureMonitor.h"


/**************************************************************************/
/*      PROTOTYPES (excluded from header)                                 */
/*------------------------------------------------------------------------*/
/*      Private Functions                                                 */
/*------------------------------------------------------------------------*/

static void reflowSM_Boot();

static void reflowSM_Main();
/*------------------------------------------------------------------------*/
static void reflowSM_ProfileSelect();

static void reflowSM_About();

static void reflowSM_Calibrate();
/*------------------------------------------------------------------------*/
static void reflowSM_ProfBack();

static void reflowSM_Leaded();

static void reflowSM_PbFree();

static void reflowSM_Custom();
/*------------------------------------------------------------------------*/
static void reflowSM_PbBack();

static void reflowSM_Start();

static void reflowSM_PbInfo();

static void reflowSM_PbSetTemp();

static void reflowSM_PbSetTime();

/*------------------------------------------------------------------------*/

static void reflowSM_Begin();

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

extern volatile uint16_t    main_MASTER_CTRL_FLAG;

/**************************************************************************/
/*      Global Variable definitions with scope limited to this local
 *      application.  Variables shall start with the name 'ENC'
 *      and be declared as static.
 */
/**************************************************************************/
/*      State Machine Function Pointer                                    */
static fnCode_type reflow_pfnStateMachine;

/*------------------------------------------------------------------------*/
/*      Other Variables.                                                  */

volatile uint32_t    reflow_systemSeconds;
volatile uint32_t reflow_startTime;         // potentially obsolete.  start time gets passed
                                            // to heaterControl.c via HeaterConfig().


static uint8_t updateWholeDisplay; // unused?

/*!*/
static profileChosen reflowProfileToUse;

static Cursor cursor = {.icon = '<', .x_position = 8, .row = 0};
static uint8_t index;
/*!!!!!!!!!!!!!!!!!!!!!!!!*/
/* PROGRAM MEMORY STRINGS */
/*!!!!!!!!!!!!!!!!!!!!!!!!*/

// -- multi-use
const char string_back[]    PROGMEM = {"BACK    "};
const char string_yes[]     PROGMEM = {"Yes!    "};
const char string_no[]      PROGMEM = {"Nope.   "};

// -- for main menu.
const char string_profiles[]    PROGMEM = {"Profiles"};
const char string_calibrate[]   PROGMEM = {"Calibrte"};
const char string_about[]       PROGMEM = {"About   "};

// -- for profile selection.    
const char string_leaded[]  PROGMEM = {"Leaded  "};     // extra spaces allow for
const char string_pbFree[]  PROGMEM = {"Pb-Free "};     // not needing to clear any
const char string_custom[]  PROGMEM = {"Custom  "};     // characters for reprint.

// -- for profile choice
const char string_start[]   PROGMEM = {"Start   "};
const char string_info[]    PROGMEM = {"Info    "};
const char string_setTemp[] PROGMEM = {"Set Temp"};
const char string_setTime[] PROGMEM = {"Set Time"};

const char string_ready[]   PROGMEM = {"Ready?  "};
/*------------------------------------------------------------------------*/
/*  MAIN MENU                   */
PGM_P const main_table[] PROGMEM =
{
    string_profiles,
    string_calibrate,
    string_about
};

fnCode_type reflowMain_Array[] =
{
    reflowSM_ProfileSelect,
    reflowSM_Calibrate,
    reflowSM_About
};
/*------------------------------------------------------------------------*/
/* PROFILES SELECTION MENU     */

/* ProfileSelect STRINGS  */
PGM_P const profileSelect_table[] PROGMEM =
{
    string_back,
    string_leaded,
    string_pbFree,
    string_custom
};

fnCode_type reflowProfiles_Array[] =
{   
    reflowSM_ProfBack,
    reflowSM_Leaded,
    reflowSM_PbFree,
    reflowSM_Custom
};
/*------------------------------------------------------------------------*/
/*  PROFILE CHOSEN MENU        */
PGM_P const profileChoice_table[] PROGMEM =
{
    string_back,
    string_start,
    string_info,
    string_setTemp,
    string_setTime
};

/* Pb-Free MENU                 */
fnCode_type reflowPbFree_Array[] =
{
    reflowSM_PbBack,
    reflowSM_Start,
    reflowSM_PbInfo,
    reflowSM_PbSetTemp,
    reflowSM_PbSetTime
};
/*------------------------------------------------------------------------*/
/*  START MENU              */
PGM_P const startMenu_table[] PROGMEM =
{
    string_yes,
    string_no
};

fnCode_type start_Array[] =
{
    reflowSM_Begin,
    reflowSM_PbFree
};

/*------------------------------------------------------------------------*/
/*  CALIBRATION MENU    */
//  string table is the same as the startMenu_table[]!

fnCode_type calibrate_Array[] =
{
    reflowSM_Calibrate, // stay in the same function pointer state machine.
    reflowSM_Main
};

/*------------------------------------------------------------------------*/
/**************************************************************************/
/**************************************************************************/
/*      Function Definitions                                              */
/**************************************************************************/

/*------------------------------------------------------------------------*/
/*      Public Functions                                                  */
/*------------------------------------------------------------------------*/

void countSeconds(uint32_t *seconds)
{
    static uint16_t twoMillisCounter = 0;
    if (twoMillisCounter == 499)
    {
        (*seconds)++;
        twoMillisCounter = 0;
    }
    twoMillisCounter++;
}



/*------------------------------------------------------------------------*/
/*      Protected Functions                                               */
/*------------------------------------------------------------------------*/
void reflow_Initialize()
{
    reflow_pfnStateMachine = reflowSM_Boot;
}

void reflow_ActiveState()
{
    countSeconds(&reflow_systemSeconds);
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
    index = 0;

    static menuSwitch currentState = WAIT_FOR_RELEASE;
    menuSwitch nextState = currentState;
    
    switch (currentState)
    {
        case WAIT_FOR_RELEASE:
        if (IsEncoderReleased())
        {
            lcd_clrscr();
            printLines(index, main_table);
            cursor.row = 0;
            printCursor(&cursor);
            
            lcd_gotoxy(12,0);
            lcd_puts_P("MAIN");
            
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
                    printLines(index, main_table);
                }
                
            }
            else if (WasEncoderTurnedLEFT())
            {
                if (cursor.row == 0)
                cursor.row = 1;
                else if (index < MAIN_ELEMENTS - 2)
                {
                    index++;
                    printLines(index, main_table);
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
        reflow_pfnStateMachine = reflowMain_Array[index + cursor.row];
        nextState = WAIT_FOR_RELEASE;
        /* reset variables */
        index = 0;
        cursor.row = 0;
        break;
    }
    currentState = nextState;
   
}
/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/
/*  Under Main  */
static void reflowSM_ProfileSelect()
{
    index = 0;

    static menuSwitch currentState = WAIT_FOR_RELEASE;
    menuSwitch nextState = currentState;
    
    switch (currentState)
    {
        case WAIT_FOR_RELEASE:
        if (IsEncoderReleased())
        {
            lcd_clrscr();
            printLines(index, profileSelect_table);
            cursor.row = 0;
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
                    printLines(index, profileSelect_table);
                }
                
            }
            else if (WasEncoderTurnedLEFT())
            {
                if (cursor.row == 0)
                cursor.row = 1;
                else if (index < LISTED_PROFILE_ELEMENTS - 2)
                {
                    index++;
                    printLines(index, profileSelect_table);
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
        reflow_pfnStateMachine = reflowProfiles_Array[index + cursor.row];
        nextState = WAIT_FOR_RELEASE;
        /* reset variables */
        index = 0;
        cursor.row = 0;
        break;
    }
    currentState = nextState;
}

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/
/*  Under Main  */
static void reflowSM_About()
{
    
}

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/
/*  Under Main  */
static void reflowSM_Calibrate()
{
    static double startTemp = 0;
    static double stopTemp = 0;
    char tempString[6];
    double degPerSecond;
    
    static double  totalTime  = 0;
    
    index = 0;

    static calibrateSwitch currentState = WAIT_FOR_RELEASE_CALI;
    calibrateSwitch nextState = currentState;
    
    switch (currentState)
    {
        case WAIT_FOR_RELEASE_CALI:
        if (IsEncoderReleased())
        {
            lcd_clrscr();
            printLines(index, startMenu_table);
            cursor.row = 0;
            printCursor(&cursor);
            
            lcd_gotoxy(10,0);
            lcd_puts_P("Ready?");
            
            nextState = WAIT_FOR_ENCODER_CALI;
        }
        break;
        /*------------------------------------------------------------------------*/
        case WAIT_FOR_ENCODER_CALI:
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
                    printLines(index, startMenu_table);
                }
                
            }
            else if (WasEncoderTurnedLEFT())
            {
                if (cursor.row == 0)
                cursor.row = 1;
                else if (index < START_ELEMENTS - 2)
                {
                    index++;
                    printLines(index, startMenu_table);
                }
            }
            else if (WasEncoderPressed())
            {
                EncoderPushAck();
                nextState = CHANGE_MENU_CALI;
            }
            EncoderTurnAck();
            printCursor(&cursor);
        }
        break;
        /*------------------------------------------------------------------------*/
        case CHANGE_MENU_CALI:
        reflow_pfnStateMachine = calibrate_Array[index + cursor.row];
        if (reflow_pfnStateMachine == reflowSM_Main)
        {
            nextState = WAIT_FOR_RELEASE_CALI;
            /* reset variables */
            index = 0;
            cursor.row = 0;
        }
        else    // will remain in this function pointer state machine.
        {
            nextState = SETUP;
        }
        lcd_clrscr();
        break;
        /*------------------------------------------------------------------------*/
        case SETUP:
        main_MASTER_CTRL_FLAG |= (TEMP_REQUEST | HEATER_POWERED);
        main_MASTER_CTRL_FLAG |= REFLOW_IN_PROGRESS;
        
        if (main_MASTER_CTRL_FLAG & TEMP_IS_VALID)
        {
            startTemp = MeasureTemp_ReadAverage();
            reflow_startTime = reflow_systemSeconds;
            nextState = START_PREHEAT;
        }
        break;
        /*------------------------------------------------------------------------*/
        case START_PREHEAT:
        HeaterPercent(100);
        nextState = RECORD_PREHEAT;
        break;
        /*------------------------------------------------------------------------*/
        case RECORD_PREHEAT:
        if ((main_MASTER_CTRL_FLAG & TEMP_IS_VALID) && MeasureTemp_ReadAverage() > 140.0)
        {
            stopTemp = MeasureTemp_ReadAverage();
            HeaterPercent(0);
            main_MASTER_CTRL_FLAG &= (~HEATER_POWERED);
            uint32_t stopTime = reflow_systemSeconds;
            totalTime = stopTime - reflow_startTime;
            
            nextState = DISPLAY_PREHEAT;
        }
        break;
        /*------------------------------------------------------------------------*/
        case DISPLAY_PREHEAT:
        degPerSecond = (stopTemp - startTemp)/totalTime;
        dtostrf(degPerSecond, -5, 1, tempString);
        lcd_gotoxy(0,1);
        lcd_puts_P("degC/s: ");
        lcd_puts(tempString);
        
        nextState = DONE;
        break;
        /*------------------------------------------------------------------------*/
        case DONE:
        if (WasEncoderPressed())
        {
            EncoderPushAck();
            reflow_pfnStateMachine = reflowSM_Main;
            nextState = WAIT_FOR_RELEASE_CALI;
            /* reset variables */
            index = 0;
            cursor.row = 0;
        }
        break;
    }
    currentState = nextState;
}
/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/
/* Under Profile Select     */
static void reflowSM_ProfBack()
{
    reflow_pfnStateMachine = reflowSM_Main;
}

/* Under Profile Select     */
static void reflowSM_Leaded()
{
    
}

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/
/* Under Profile Select     */
static void reflowSM_PbFree()
{
    index = 0;

    static menuSwitch currentState = WAIT_FOR_RELEASE;
    menuSwitch nextState = currentState;
    
    switch (currentState)
    {
        case WAIT_FOR_RELEASE:
        if (IsEncoderReleased())
        {
            lcd_clrscr();
            printLines(index, profileChoice_table);
            cursor.row = 0;
            printCursor(&cursor);
            
            lcd_gotoxy(9,0);
            lcd_puts_P("Pb-FREE");
            
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
                    printLines(index, profileChoice_table);
                }
                
            }
            else if (WasEncoderTurnedLEFT())
            {
                if (cursor.row == 0)
                cursor.row = 1;
                else if (index < PROFILE_SELECTED_ELEMENTS - 2)
                {
                    index++;
                    printLines(index, profileChoice_table);
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
        
        // select your desired profile when leaving this state machine.
        reflowProfileToUse = PBFREE;
        
        reflow_pfnStateMachine = reflowPbFree_Array[index + cursor.row];
        nextState = WAIT_FOR_RELEASE;
        /* reset variables */
        index = 0;
        cursor.row = 0;
        break;
    }
    currentState = nextState;
}

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/
/* Under Profile Select     */
static void reflowSM_Custom()
{
    
}

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/
/* Under Leaded     */

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/
/*  Under Pb-Free   */
static void reflowSM_PbBack()
{
    reflow_pfnStateMachine = reflowSM_ProfileSelect;
}

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/
/*  Under Pb-Free   */
static void reflowSM_PbInfo()
{
    
}

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/
/*  Under Pb-Free   */
static void reflowSM_PbSetTemp()
{
    
}

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/
/*  Under Pb-Free   */
static void reflowSM_PbSetTime()
{
    
}

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/

static void reflowSM_Start()
{
    index = 0;

    static menuSwitch currentState = WAIT_FOR_RELEASE;
    menuSwitch nextState = currentState;
    
    switch (currentState)
    {
        case WAIT_FOR_RELEASE:
        if (IsEncoderReleased())
        {
            lcd_clrscr();
            printLines(index, startMenu_table);
            cursor.row = 0;
            printCursor(&cursor);
            
            lcd_gotoxy(10,0);
            lcd_puts_P("Ready?");
            
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
                    printLines(index, startMenu_table);
                }
                
            }
            else if (WasEncoderTurnedLEFT())
            {
                if (cursor.row == 0)
                cursor.row = 1;
                else if (index < START_ELEMENTS - 2)
                {
                    index++;
                    printLines(index, startMenu_table);
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
        reflow_pfnStateMachine = start_Array[index + cursor.row];
        nextState = WAIT_FOR_RELEASE;
        /* reset variables */
        index = 0;
        cursor.row = 0;
        break;
    }
    currentState = nextState;
}


/*------------------------------------------------------------------------*/

void checkToKillProcess(reflowSwitch *nextState)
{
    if (! (main_MASTER_CTRL_FLAG & REFLOW_IN_PROGRESS))
    {   /* IF THE REFLOW HAS BEEN CANCELLED BY ANYTHING EXTERNAL */
        main_MASTER_CTRL_FLAG &= ~(HEATER_POWERED); // redundancy department of redundancy.
        *nextState = DONE_PROC;
    }
}
static void reflowSM_Begin()
{
    static double preheatGoalTemp = 0;
    static double preheatRate = 0;
    static double soakGoalTemp = 0;
    static double soakRate = 0;
    static double reflowGoalTemp = 0;
    static double reflowRate = 0;  
    static double cool1GoalTemp = 0;    // coming off reflow peak.
    static double cool1Rate = 0;
    static double cool2GoalTemp = 0;
    static double cool2Rate = 0;
    
    static reflowSwitch currentState = WAIT_FOR_RELEASE_PROC;
    reflowSwitch nextState = currentState;
    
    if (currentState != WAIT_FOR_RELEASE_PROC)
    {
        checkToKillProcess(&nextState); // checks if the reflow process has been cancelled, and if so,
                                        // set the nextState to DONE.
    }
    switch (currentState)
    {
        case WAIT_FOR_RELEASE_PROC:
        if (IsEncoderReleased())
        {
            lcd_clrscr();
            /* Print stuff related to the reflow. */
            /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
            
            //printLines(index, startMenu_table);
            //cursor.row = 0;
            //printCursor(&cursor);
            
            //lcd_gotoxy(10,0);
            //lcd_puts_P("Ready?");
            
            nextState = INITIALIZE;
        }
        break;
        
        case INITIALIZE:
        if (reflowProfileToUse == LEADED)
        {
            
        }
        else if (reflowProfileToUse == PBFREE)
        {
            preheatGoalTemp = 140.00;
            preheatRate = 1.17;
            
            soakGoalTemp = 200.00;
            soakRate = 1.00;
            
            reflowGoalTemp = 235.00;        // must be less than 249.00
            reflowRate = 1.17;
            
            cool1GoalTemp = 219.00;
            cool1Rate = -0.50;              // negative because the temp is decreasing.
            
            cool2GoalTemp = 150.00;
            cool2Rate = -2.00;              // if this rate isn't being achieved,
                                            //  ask for the oven door to be opened.
        }
        else if (reflowProfileToUse == CUSTOM)
        {
            
        }
        /* every time HeaterConfig() is called, the GOALTEMP_REACHED flag is cleared by default. =) */
        main_MASTER_CTRL_FLAG |= (HEATER_POWERED | REFLOW_IN_PROGRESS); // if REFLOW_IN_PROGRESS gets cleared,
        nextState = START_PREHEAT_PROC;                                 // checkToKillProcess() will reset the
        break;                                                          // switch-case state machine.
        
        
        case START_PREHEAT_PROC:
        HeaterConfig(preheatGoalTemp, preheatRate, reflow_systemSeconds);
        nextState = PREHEATING_PROC;
        break;
        
        case PREHEATING_PROC:
        if ( main_MASTER_CTRL_FLAG & GOALTEMP_REACHED)
        {
            nextState = START_SOAK_PROC;
        }            
        break;
        
        case START_SOAK_PROC:
        HeaterConfig(soakGoalTemp, soakRate, reflow_systemSeconds);
        nextState = SOAKING_PROC;
        break;
        
        case SOAKING_PROC:
        if ( main_MASTER_CTRL_FLAG & GOALTEMP_REACHED)
        {
            nextState = START_REFLOW_PROC;
        }
        break; 
        
        case START_REFLOW_PROC:
        HeaterConfig(reflowGoalTemp, reflowRate, reflow_systemSeconds);
        nextState = REFLOWING_PROC;
        break;
        
        case REFLOWING_PROC:
        if ( main_MASTER_CTRL_FLAG & GOALTEMP_REACHED)
        {
            nextState = START_COOL1_PROC;
        }
        break;
        
        case START_COOL1_PROC:
        HeaterConfig(cool1GoalTemp, cool1Rate, reflow_systemSeconds);
        nextState = COOLING1_PROC;
        break;
        
        case COOLING1_PROC:
        if ( main_MASTER_CTRL_FLAG & GOALTEMP_REACHED)
        {
            nextState = START_COOL2_PROC;
        }
        break;
        
        case START_COOL2_PROC:
        HeaterConfig(cool2GoalTemp, cool2Rate, reflow_systemSeconds);
        nextState = COOLING2_PROC;
        break;
        
        case COOLING2_PROC:
        if ( main_MASTER_CTRL_FLAG & GOALTEMP_REACHED)
        {
            nextState = DONE_PROC;
        }
        break;
        
        case DONE_PROC:
        /* if button pressed, go to main menu? */
        break;
    }               
    currentState = nextState;
}    



/**************************************************************************/
/*      END OF FILE                                                       */
/**************************************************************************/