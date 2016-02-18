/*
 * reflow_HeaterControl.c
 *
 * Created: 12/6/2015 2:20:06 PM
 *  Author: dsgiesbrecht
 */ 


#include "HeaterControl.h"
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
extern volatile uint16_t   main_MASTER_CTRL_FLAG;


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

static uint16_t HeaterOnTime = 0;
static uint16_t HeaterOnTime_BUFFER = 0;
#define HEATER_PERIOD   1000    // 2sec (2ms period)

/**************************************************************************/
/*      Static Function Prototypes                                        */
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

/*      test function                                                     */
void HeaterPercent(uint16_t percent)
{
    uint16_t temp_percent = 0;
    if (percent > 90)
        temp_percent = 100;
    else if (percent < 10)
        temp_percent = 0;
    else
        temp_percent = percent;
        
    HeaterOnTime = (temp_percent * HEATER_PERIOD)/100;
}  
/*------------------------------------------------------------------------*/
/*      Protected Functions                                               */
/*------------------------------------------------------------------------*/
void HeaterControl_Initialize()
{
    //HeaterControl_pfnStateMachine = IDLESTATENAME;
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