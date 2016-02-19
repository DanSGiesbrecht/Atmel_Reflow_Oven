/*
 * TemperatureMonitor.c
 *
 * Created: 12/6/2015 3:35:17 PM
 *  Author: dsgiesbrecht
 */ 


#include "ATMEGA328P_reflow_config.h"
#include "TemperatureMonitor.h"

/**************************************************************************/
/*      MACROS                                                            */
#define MAX_TEMP_SAMPLE     10
/**************************************************************************/
/*      PROTOTYPES (excluded from header)                                 */
/*------------------------------------------------------------------------*/
/*      Private Functions                                                 */
/*------------------------------------------------------------------------*/

/**************************************************************************/
/*      State Machines  (also private)                                    */
/*      This is here and not in the header because static function
 *      prototyptes in a header file included elsewhere causes a
 *      whole mess of problems.  This allows the functions to work
 *      as intended: locally in this file.
 */
/**************************************************************************/

static void MeasureTempSM_Idle();

static void MeasureTempSM_Record();

static void MeasureTempSM_Average();

static void MeasureTempSM_Standby();
/**************************************************************************/
/*      Global Variables that are !exteral!                               */
extern volatile uint16_t   main_MASTER_CTRL_FLAG;


/**************************************************************************/
/*      Global Variable definitions with scope limited to this local
 *      application.  Variables shall start with the name 'MeasureTemp'
 *      and be declared as static.
 */
/**************************************************************************/
/*      State Machine Function Pointer                                    */
static fnCode_type MeasureTemp_pfnStateMachine;
/*------------------------------------------------------------------------*/
static double MeasureTemp_AveragedegC;
static double MeasureTemp_Array[MAX_TEMP_SAMPLE];
/**************************************************************************/
/**************************************************************************/
/*      Function Definitions                                              */
/**************************************************************************/

/*------------------------------------------------------------------------*/
/*      Public Functions                                                  */
/*------------------------------------------------------------------------*/

double MeasureTemp_ReadAverage()
{
    return MeasureTemp_AveragedegC;
}

/*------------------------------------------------------------------------*/
/*      Protected Functions                                               */
/*------------------------------------------------------------------------*/

/*      Initialize the State Machine.                                     */
void MeasureTemp_Initialize()
{
    /* Clear the 'Temperature is valid' flag, set the state to idle. */
    main_MASTER_CTRL_FLAG &= (~TEMP_IS_VALID);
    MeasureTemp_pfnStateMachine = MeasureTempSM_Idle;
}

/*      Run the current State.                                            */
void MeasureTemp_ActiveState()
{
    MeasureTemp_pfnStateMachine();
}



/*------------------------------------------------------------------------*/
/*      Private Functions                                                 */
/*------------------------------------------------------------------------*/


/**************************************************************************/
/*      State Machine Functions  (also private)                           */
/**************************************************************************/

static void MeasureTempSM_Idle()
{
    /* If some task is requesting a temperature; */
    if (main_MASTER_CTRL_FLAG & TEMP_REQUEST)
    {
        /* Change state to measure/record. */
        main_MASTER_CTRL_FLAG &= (~TEMP_IS_VALID);
        MeasureTemp_pfnStateMachine = MeasureTempSM_Record;
    }
}

static void MeasureTempSM_Record()
{
    /* Measure temperature, and make sure it's valid. */
    static uint8_t index = 0;
    double currentTemp = max31855_readCelsius();
    
    if (index < MAX_TEMP_SAMPLE)
    {
        if (currentTemp != NAN)
        {
            MeasureTemp_Array[index] = currentTemp;
            index++;
        }
    }
    else
    {
        /*  If the array is filled with valid temp's, change states
            and reset the index.
        */
        MeasureTemp_pfnStateMachine = MeasureTempSM_Average;
        index = 0;
    }
}

static void MeasureTempSM_Average()
{
    static double sum = 0;
    static uint8_t i = 0;
    
    if (i < MAX_TEMP_SAMPLE)
    {
        sum += MeasureTemp_Array[i];
        i++;
    }
    else
    {
        main_MASTER_CTRL_FLAG |= TEMP_IS_VALID;
        
        MeasureTemp_AveragedegC = sum / (double)MAX_TEMP_SAMPLE;
        sum = 0;
        i = 0;
        MeasureTemp_pfnStateMachine = MeasureTempSM_Standby;
    }
}

static void MeasureTempSM_Standby()
{
    /*  If the 'Request Temperature Flag is [still] set,
     *  set the next state to RecordTemp. Otherwise, stay here.
     *
     *  Note, this state is similar to 'Idle', but the Temp-Valid
     *  bit is [assumed] to still be set.
     *
     *  ->  Look into when the temperature may not be valid, maybe
     *      adjust code accordingly.
     */
    #warning "Potential bug - will the temperature ever *not* be valid after this?"
    if (main_MASTER_CTRL_FLAG & TEMP_REQUEST)
    {
        MeasureTemp_pfnStateMachine = MeasureTempSM_Record;
    }
}





/**************************************************************************/
/*      END OF FILE                                                       */
/**************************************************************************/