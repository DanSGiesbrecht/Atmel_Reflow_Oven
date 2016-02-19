/**************************************************************************/
/*
 * reflow_app.h
 *
 *   Created: 18/02/2016 4:50:00 PM
 *    Author: dsgiesbrecht
 *   Version: v1.0
 *  
 * Description:
 *      Reflow Application
 *          -Consistently controls the temperature of the oven for
 *              repeatable solder reflow.
 *
 * Important Notes: Refer to the ATMEGA328P_reflow_config.h file for
                    hardware specifics.
 */
/**************************************************************************/
/**************************************************************************/

#include <avr/pgmspace.h>
//#include "ATMEGA328P_reflow_config.h"
#include <avr/io.h>


#define MAIN_ELEMENTS   2

/**************************************************************************/
/*      TYPEDEFS                                                          */
typedef void(*fnCode_type)(void);

/* Profile menu items */
typedef enum {BACK_MAIN = 0, LEADED, PB_FREE, CUSTOM} profileItems;
    
/* MAIN MENU STRINGS */
const char string_profiles[]  PROGMEM = "Profiles";
const char string_info[]      PROGMEM = "Info";

const char string_mainSide[]  PROGMEM = "MAIN";

/**************************************************************************/
/*      STRUCTS                                                           */

typedef struct Cursor
{
    const char icon;
    const uint8_t x_position;
    uint8_t row;
    } Cursor;
    
    
/**************************************************************************/
/**************************************************************************/
/*      Function Prototypes                                               */
/**************************************************************************/

/*------------------------------------------------------------------------*/
/*      Public Functions                                                  */
/*------------------------------------------------------------------------*/



/*------------------------------------------------------------------------*/
/*      Protected Functions                                               */
/*------------------------------------------------------------------------*/
void reflow_Initialize();

void reflow_ActiveState();

/**************************************************************************/
/*                         END OF FILE                                    */
/**************************************************************************/    