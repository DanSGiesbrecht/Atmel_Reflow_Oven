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
     /* To leave this state, a reflow process must be running (flag set), and the Heater-
      * powered flag must also be set.
      */
     HeaterSet(HEATER_OFF);
     if ((main_MASTER_CTRL_FLAG & REFLOW_IN_PROGRESS)&&(main_MASTER_CTRL_FLAG & HEATER_POWERED))
     {
         HeaterPWM_pfnStateMachine = HeaterPWM_SM_Standby;
     }
 }
 
 static void HeaterPWM_SM_Standby()
 {
     if (!(main_MASTER_CTRL_FLAG & REFLOW_IN_PROGRESS))
 }
 
 static void HeaterPWM_SM_HeaterON()
 {
     
 }
 
 static void HeaterPWM_SM_CountON()
 {
     
 }
 
 static void HeaterPWM_SM_HeaterOFF()
 {
     
 }
 
 static void HeaterPWM_SM_CountOFF()
 {
     
 }