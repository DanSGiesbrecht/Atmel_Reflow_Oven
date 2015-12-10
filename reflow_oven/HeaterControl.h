/*
 * reflow_HeaterControl.h
 *
 * Created: 12/6/2015 2:20:24 PM
 *  Author: dsgiesbrecht
 */ 


#ifndef REFLOW_HEATERCONTROL_H_
#define REFLOW_HEATERCONTROL_H_

/**************************************************************************/
/*      Function Prototypes                                               */
/**************************************************************************/

/*------------------------------------------------------------------------*/
/*      Public Functions                                                  */
/*------------------------------------------------------------------------*/
/* Set up the heater port/pin. */
void Heater_Init();

void Fan_Init();

typedef enum { FAN_OFF = 0, FAN_ON } FanSetting;
void FanSet(FanSetting status);

/*------------------------------------------------------------------------*/
/*      Protected Functions                                               */
/*------------------------------------------------------------------------*/
void HeaterControl_Initialize();

void HeaterControl_ActiveState();
/*------------------------------------------------------------------------*/
void HeaterPWM_Initialize();

void HeaterPWM_ActiveState();
/*------------------------------------------------------------------------*/
/*      Private Functions                                                 */
/*------------------------------------------------------------------------*/
/* 
 *      static void HeaterSet() is defined in the source file because
 *      it is static.
 */




#endif /* REFLOW_HEATERCONTROL_H_ */