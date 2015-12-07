/*
 * TemperatureMonitor.h
 *
 * Created: 12/6/2015 3:35:31 PM
 *  Author: dsgiesbrecht
 */ 


#ifndef TEMPERATUREMONITOR_H_
#define TEMPERATUREMONITOR_H_

#include "max31855.h"
#include "reflow_oven_main.h"

/**************************************************************************/
/*      Typedefs                                                          */
/**************************************************************************/
/*  Typedef for function pointer
 *  eg) static fnCode_type func_ptr;
 */
typedef void(*fnCode_type)(void);

/**************************************************************************/
/*      Prototypes                                                        */
/**************************************************************************/
/*------------------------------------------------------------------------*/
/*      Public Functions                                                  */
/*------------------------------------------------------------------------*/

/*      MeasureTemp_ReadAverage Function:
 *      Requires:
 *          -   The TEMP_IS_VALID Flag (located in main.h) must be set
 *              to indicate that the last calculated average is a
 *              correct value.
 *      Promises:
 *          -   To return a double value which is the most recently
 *              calculated temperature average.
 */
double MeasureTemp_ReadAverage();

/*------------------------------------------------------------------------*/
/*      Protected Functions                                               */
/*------------------------------------------------------------------------*/
void MeasureTemp_Initialize();

void MeasureTemperature_ActiveState();


/*------------------------------------------------------------------------*/
/*      Private Functions                                                 */
/*------------------------------------------------------------------------*/

/**************************************************************************/
/*      State Machines  (also private)                                    */
/**************************************************************************/

static void MeasureTempSM_Idle();

static void MeasureTempSM_Record();

static void MeasureTempSM_Average();

static void MeasureTempSM_Standby();


/**************************************************************************/
#endif /* TEMPERATUREMONITOR_H_ */

/**************************************************************************/
/*      END OF FILE                                                       */
/**************************************************************************/