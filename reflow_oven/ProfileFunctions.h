/*
 * ProfileFunctions.h
 *
 * Created: 10/3/2015 9:25:12 PM
 *  Author: dsgiesbrecht
 */ 


#ifndef PROFILEFUNCTIONS_H_
#define PROFILEFUNCTIONS_H_

#include "reflow_EEPROM.h"

typedef struct profile
{
    char name[NAMESIZE];
    uint8_t Time_array[NUM_TIME_PARAMS];
    uint8_t Temp_array[NUM_TEMP_PARAMS];
    
} Profile;


#endif /* PROFILEFUNCTIONS_H_ */