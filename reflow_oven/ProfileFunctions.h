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


void update_Profile(Profile *_prof, uint8_t **eeprom_ptr);
/*  Pass in 'working_profile', and the array of eeprom ptrs (eeprom_ptr) to update working profile values.          */

uint8_t get_Profile_time_param(Profile *_prof, uint8_t PARAMETER);

uint8_t get_Profile_temp_param(Profile *_prof, uint8_t PARAMETER);




#endif /* PROFILEFUNCTIONS_H_ */