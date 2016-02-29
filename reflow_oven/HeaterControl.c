/*
 * reflow_HeaterControl.c
 *
 * Created: 12/6/2015 2:20:06 PM
 *  Author: dsgiesbrecht
 */ 


#include "HeaterControl.h"
#include "TemperatureMonitor.h"
#include "ATMEGA328P_reflow_config.h"

/**************************************************************************/
/*      Typedefs                                                          */
/**************************************************************************/
/*  Typedef for function pointer
 *  eg) static fnCode_type func_ptr;
 */
typedef void(*fnCode_type)(void);


/**************************************************************************/
/*      Global Variables that are !exteral!                               */
extern volatile uint16_t   main_MASTER_CTRL_FLAG; // reflow_oven_main.c
extern volatile uint32_t    reflow_systemSeconds; // reflow_app.c

/**************************************************************************/
/**************************************************************************/
/*      Global Variable definitions with scope limited to this local
 *      application.  Variables shall start with the name 'MeasureTemp'
 *      and be declared as static.
 */
/**************************************************************************/
/*      State Machine Function Pointer                                    */
static fnCode_type HeaterControl_pfnStateMachine;
static fnCode_type HeaterPWM_pfnStateMachine;

static double HeaterOnTime = 0;
static double HeaterOnTime_BUFFER = 0;
//#define HEATER_PERIOD   1000    // 2sec (2ms period)
#define HEATER_PERIOD     500     // 1sec (2ms period)

/* For heaterControl State Machine */
volatile static double   _heater_goalTemp = 0;
volatile static double   _heater_degPerSecond = 0;
volatile static uint32_t _heater_startTime = 0;
static uint32_t initCounter = 0;
static double   _heater_startTemp = 0;
static uint8_t  percentToHeat = 0;

/**************************************************************************/
/*      Static Function Prototypes                                        */
/**************************************************************************/
static void HeaterControlSM_Idle();

static void HeaterControlSM_Setup();

static void HeaterControlSM_AdjustHeat();

static void HeaterControlSM_Wait2s();

static void HeaterControlSM_GetTempCalculate();

/**************************************************************************/
/* Turn the heater on or off. */
typedef enum { HEATER_OFF = 0, HEATER_ON } HeaterSetting;
static void HeaterSet(HeaterSetting status);

static void HeaterPWM_SM_Idle();
 
static void HeaterPWM_SM_Standby();
 
static void HeaterPWM_SM_HeaterON();

static void HeaterPWM_SM_CountON();

static void HeaterPWM_SM_HeaterOFF();

static void HeaterPWM_SM_CountOFF();

/**************************************************************************/
/*      Function Definitions                                              */
/**************************************************************************/

/*------------------------------------------------------------------------*/
/*      Public Functions                                                  */
/*------------------------------------------------------------------------*/

void Heater_Init()
{
    /* Set The heater pin as an output. */
    DDR(HEATER_PORT) |= (1 << HEATER_PIN);
    
}

/* FAN FUNCTIONS ARE LEFT PUBLIC BECAUSE THEY ARE BEING USED TO CONTROL
 * THE HEARTBEAT LED.
 */
void Fan_Init()
{
    /* Set The heater pin as an output. */
    DDR(FAN_PORT) |= (1 << FAN_PIN);
}


void FanSet(FanSetting status)
{
    uint8_t protectREG = FAN_PORT & (~ (1 << FAN_PIN) );
    FAN_PORT = protectREG | (status << FAN_PIN);
}
/*------------------------------------------------------------------------*/
/* Settings used for Heater Control */
void HeaterConfig(double goalTemp, double degPerSecond, uint32_t startTime)
{
    main_MASTER_CTRL_FLAG &= ~(GOALTEMP_REACHED);   // make sure that this flag is clear.
    _heater_goalTemp = goalTemp;
    _heater_degPerSecond = degPerSecond;
    _heater_startTime = startTime;
}
/*------------------------------------------------------------------------*/
/*                                                                        */
void HeaterPercent(double percent)
{
    double temp_percent = 0;
    if (percent > 90)
        HeaterOnTime = HEATER_PERIOD;
    else if (percent < 10)
        HeaterOnTime = 0;
    else
    {
        temp_percent = percent;
        HeaterOnTime = (temp_percent * HEATER_PERIOD)/100;
    }        
}  
/*------------------------------------------------------------------------*/
/*      Protected Functions                                               */
/*------------------------------------------------------------------------*/
void HeaterControl_Initialize()
{
    HeaterControl_pfnStateMachine = HeaterControlSM_Idle;
}

void HeaterControl_ActiveState()
{
    HeaterControl_pfnStateMachine();
}
/*------------------------------------------------------------------------*/
void HeaterPWM_Initialize()
{
    HeaterPWM_pfnStateMachine = HeaterPWM_SM_Idle;
}

void HeaterPWM_ActiveState()
{
    HeaterPWM_pfnStateMachine();
}
/*------------------------------------------------------------------------*/
/*      Private Functions                                                 */
/*------------------------------------------------------------------------*/
/*      HeaterSet Function:
 *
 *      Apart from direct bit-manipulation, the heater cannot be turned on
 *      outside of this file
 */
static void HeaterSet(HeaterSetting status)
{
    uint8_t protectREG = HEATER_PORT & (~ (1 << HEATER_PIN) );
    HEATER_PORT = protectREG | (status << HEATER_PIN);
}

/**************************************************************************/
/*      State Machine Functions  (also private)                           */
/**************************************************************************/

/*      HEATER CONTROL STATE MACHINE
 *      
 *      Sets the required temperature based on the state of the system.
 *      ie: an active reflow process is happening, and this state machine
 *      needs to calculate the temperature required.
 */

static void HeaterControlSM_Idle()
{
    HeaterPercent(0);
    if (_heater_goalTemp != 0 && _heater_degPerSecond != 0)
    {
        HeaterControl_pfnStateMachine = HeaterControlSM_Setup;
    }
}

static void HeaterControlSM_Setup()
{
    main_MASTER_CTRL_FLAG |= (TEMP_REQUEST);
    if (main_MASTER_CTRL_FLAG & TEMP_IS_VALID)
    {
        main_MASTER_CTRL_FLAG &= ~(TEMP_REQUEST | TEMP_IS_VALID);
        _heater_startTemp = MeasureTemp_ReadAverage();
        percentToHeat = 0;
        if (_heater_degPerSecond > 0)
        {
            if (_heater_degPerSecond > 1)
                percentToHeat = 100;
            else
                percentToHeat = _heater_degPerSecond * 100;
        }
        HeaterControl_pfnStateMachine = HeaterControlSM_AdjustHeat;
    }
}

static void HeaterControlSM_AdjustHeat()
{
    HeaterPercent(percentToHeat);
    initCounter = reflow_systemSeconds;
    HeaterControl_pfnStateMachine = HeaterControlSM_Wait2s;
}

static void HeaterControlSM_Wait2s()
{
    if ((reflow_systemSeconds - initCounter) >= 2)
    {
        HeaterControl_pfnStateMachine = HeaterControlSM_GetTempCalculate;
    }
}

static void HeaterControlSM_GetTempCalculate()
{
    main_MASTER_CTRL_FLAG |= (TEMP_REQUEST);
    if (main_MASTER_CTRL_FLAG & TEMP_IS_VALID)
    {
        main_MASTER_CTRL_FLAG &= ~(TEMP_REQUEST | TEMP_IS_VALID);
        //double currentTemp = MeasureTemp_ReadAverage();
        
        double degPerSec = (MeasureTemp_ReadAverage() - _heater_startTemp)/(reflow_systemSeconds - _heater_startTime);
        
        if (_heater_degPerSecond >= 0)
        {
            if ( MeasureTemp_ReadAverage() >= _heater_goalTemp - 5) // goal almost reached!
            {
                percentToHeat = 0;  // turn off the heating elements early, to allow for delayed heating.
                
                if (MeasureTemp_ReadAverage() >= _heater_goalTemp)  // goal reached! set main flag!
                {
                    main_MASTER_CTRL_FLAG |= GOALTEMP_REACHED;
                    HeaterControl_pfnStateMachine = HeaterControlSM_Idle;
                    return;
                }
            }
        }
        else    // _heater_degPerSecond < 0
        {
            if (MeasureTemp_ReadAverage() <= _heater_goalTemp)  // goal reached! set main flag!
            {
                percentToHeat = 0;  // precautionary.
                main_MASTER_CTRL_FLAG |= GOALTEMP_REACHED;
                HeaterControl_pfnStateMachine = HeaterControlSM_Idle;
                return;
            }
        }
        HeaterControl_pfnStateMachine = HeaterControlSM_AdjustHeat;
        /*------------------------------------------------------------------------*/
        // Control temperature. (should work for + and - rate changes.
        if ( degPerSec > _heater_degPerSecond + 2) // (greater than +2)
        {
            percentToHeat = 0;
        }
        else if (degPerSec > _heater_degPerSecond + 1) // (between +1 and +2)
        {
            if (percentToHeat >= 30)
            percentToHeat -= 30;
            else
            percentToHeat = 0;
        }
        else if (degPerSec > _heater_degPerSecond + 0.5) // (between +0.5 and +1)
        {
            if (percentToHeat >= 10)
            percentToHeat -= 10;
            else
            percentToHeat = 0;
        }
        else if (degPerSec > _heater_degPerSecond - 0.5) // (between +0.5 and -0.5) no change
        {
            // no change.
        }
        else if (degPerSec > _heater_degPerSecond - 1) //(between -1 and -0.5)
        {
            if (percentToHeat <= 90)
            percentToHeat += 10;
            else
            percentToHeat = 100;
        }
        else if (degPerSec > _heater_degPerSecond -2) // (between -1 and -2)
        {
            if (percentToHeat <= 70)
            percentToHeat += 30;
            else
            percentToHeat = 100;
        }
        else if (degPerSec <= _heater_degPerSecond - 2) // (less than -2)
        {
            percentToHeat = 100;
        }
        /*------------------------------------------------------------------------*/
        
        
        /*
        if (_heater_degPerSecond >= 0)
        {
            if ( MeasureTemp_ReadAverage() >= _heater_goalTemp - 5) // goal almost reached!
            {
                percentToHeat = 0;  // turn off the heating elements early, to allow for delayed heating.
                
                if (MeasureTemp_ReadAverage() >= _heater_goalTemp)  // goal reached! set main flag!
                {
                    main_MASTER_CTRL_FLAG |= GOALTEMP_REACHED;
                    HeaterControl_pfnStateMachine = HeaterControlSM_Idle;
                    return;
                }
            }
            else
                HeaterControl_pfnStateMachine = HeaterControlSM_AdjustHeat;
            
            if ( degPerSec > _heater_degPerSecond + 2) // (greater than +2)
            {
                percentToHeat = 0;
            }
            else if (degPerSec > _heater_degPerSecond + 1) // (between +1 and +2)
            {
                if (percentToHeat >= 30)
                    percentToHeat -= 30;
                else
                    percentToHeat = 0;
            }
            else if (degPerSec > _heater_degPerSecond + 0.5) // (between +0.5 and +1)
            {
                if (percentToHeat >= 10)
                    percentToHeat -= 10;
                else
                    percentToHeat = 0;
            }
            else if (degPerSec > _heater_degPerSecond - 0.5) // (between +0.5 and -0.5) no change
            {
                // no change.
            }
            else if (degPerSec > _heater_degPerSecond - 1) //(between -1 and -0.5)
            {
                if (percentToHeat <= 90)
                    percentToHeat += 10;
                else
                    percentToHeat = 100;
            }
            else if (degPerSec > _heater_degPerSecond -2) // (between -1 and -2)
            {
                if (percentToHeat <= 70)
                    percentToHeat += 30;
                else
                    percentToHeat = 100;
            }
            else if (degPerSec <= _heater_degPerSecond - 2) // (less than -2)
            {
                percentToHeat = 100;
            }
        }
        else    // _heater_degPerSecond < 0
        {
            if (MeasureTemp_ReadAverage() <= _heater_goalTemp)  // goal reached! set main flag!
            {
                percentToHeat = 0;  // precautionary.
                main_MASTER_CTRL_FLAG |= GOALTEMP_REACHED;
                HeaterControl_pfnStateMachine = HeaterControlSM_Idle;
                return;
            }
            else
                HeaterControl_pfnStateMachine = HeaterControlSM_AdjustHeat;
            
            if (degPerSec > _heater_degPerSecond - 1)
            {
                if (percentToHeat <= 90)
                    percentToHeat += 10;
                else
                    percentToHeat = 100;
            }
            else if (degPerSec > _heater_degPerSecond - 2)
            {
                if (percentToHeat <= 70)
                    percentToHeat += 30;
                else
                    percentToHeat = 100;
            }
            else if (degPerSec <= _heater_degPerSecond - 2)
            {
                percentToHeat = 100;
            }
        }
        
    }
    */
    }        
}

/*--------------------------------------------------------------------------------------------------*/
/*      HEATER PWM STATE MACHINE
 *
 *      This state machine directly manipulates the length of time that the
 *      heater relay remains turned on, using heater manip. functions created
 *      above.
 */
 static void HeaterPWM_SM_Idle()
 {
     /* To leave this state, a reflow process must be running (flag set), and
      * the Heater-powered flag must also be set.
      */
     HeaterSet(HEATER_OFF);
     if ((main_MASTER_CTRL_FLAG & REFLOW_IN_PROGRESS)&&(main_MASTER_CTRL_FLAG & HEATER_POWERED))
     {
         HeaterPWM_pfnStateMachine = HeaterPWM_SM_Standby;
     }
 }
 
 static void HeaterPWM_SM_Standby()
 {
     if ( ( main_MASTER_CTRL_FLAG & REFLOW_IN_PROGRESS & HEATER_POWERED)==(REFLOW_IN_PROGRESS & HEATER_POWERED)
            && HeaterOnTime != 0)
     {
         HeaterPWM_pfnStateMachine = HeaterPWM_SM_HeaterON;
         /*     A buffer is used because it is possible that HeaterOnTime might get changed midway through
          *     a heating cycle.
          */
         HeaterOnTime_BUFFER = HeaterOnTime;
     }         
     else
     {
         HeaterPWM_pfnStateMachine = HeaterPWM_SM_Idle;
     }
 }
 
 static void HeaterPWM_SM_HeaterON()
 {
     HeaterSet(HEATER_ON);
     HeaterPWM_pfnStateMachine = HeaterPWM_SM_CountON;
 }
 
 static void HeaterPWM_SM_CountON()
 {
     static uint16_t Count = 0;
     if (Count == HeaterOnTime_BUFFER)
     {
         HeaterPWM_pfnStateMachine = HeaterPWM_SM_HeaterOFF;
         Count = 0;
     }
     else
     {
         Count++;
     }
 }
 
 static void HeaterPWM_SM_HeaterOFF()
 {
     HeaterSet(HEATER_OFF);
     HeaterPWM_pfnStateMachine = HeaterPWM_SM_CountOFF;
 }
 
 static void HeaterPWM_SM_CountOFF()
 {
     uint16_t OffTime = (uint16_t)HEATER_PERIOD - HeaterOnTime_BUFFER;
     static uint16_t Count = 0;
     if (Count == OffTime)
     {
         HeaterPWM_pfnStateMachine = HeaterPWM_SM_Standby;
         Count = 0;
     }
     else
     {
         Count++;
     }
 }
 
 /**************************************************************************/
 /*      END OF FILE                                                       */
 /**************************************************************************/