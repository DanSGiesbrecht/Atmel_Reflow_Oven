/*
 * ProfileFunctions.h
 *
 * Created: 10/3/2015 9:25:12 PM
 *  Author: dsgiesbrecht
 */ 


#ifndef PROFILEFUNCTIONS_H_
#define PROFILEFUNCTIONS_H_

#include "reflow_EEPROM.h"

typedef struct profile_struct
{
    uint8_t name[NAMESIZE];
    uint8_t Time_array[NUM_TIME_PARAMS];
    uint8_t Temp_array[NUM_TEMP_PARAMS];
    
} Profile;


void update_Profile(Profile *_prof, uint8_t **eeprom_ptr, EEPROM_area select);
/*
*   Requires:
*       -Profile address
*       -the array of EEPROM pointers (eeprom_ptr)
*       -Which EEPROM area to update: (_ALL | _Name | _Time | _Temp )
*
*   Promises:
*       -To update the Profile with values from the selected EEPROM area.
*/

uint8_t get_Profile_param(Profile *_prof, uint8_t param, EEPROM_area select);

void store_EEPROM(uint8_t update_value, uint8_t *eeprom_ptr, uint8_t param);


#endif /* PROFILEFUNCTIONS_H_ */