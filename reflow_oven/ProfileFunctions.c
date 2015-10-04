/*
 * ProfileFunctions.c
 *
 * Created: 10/3/2015 9:23:01 PM
 *  Author: dsgiesbrecht
 */ 

#include "ProfileFunctions.h"

/* GLOBAL profile struct, used as the working copy */
Profile working_profile;

/* DEFINITIONS */

void update_Profile(Profile *_prof, uint8_t **eeprom_ptr)
{
    uint8_t SRAM_name_array[NAMESIZE];
    uint8_t SRAM_time_array[NUM_TIME_PARAMS];
    uint8_t SRAM_temp_array[NUM_TEMP_PARAMS];
    
    eeprom_read_block((void*)SRAM_name_array, (const void*)(eeprom_ptr[0]), NAMESIZE);
    eeprom_read_block((void*)SRAM_time_array, (const void*)(eeprom_ptr[1]), NUM_TIME_PARAMS);
    eeprom_read_block((void*)SRAM_temp_array, (const void*)(eeprom_ptr[2]), NUM_TEMP_PARAMS);
    
    for (uint8_t i=0; i < NAMESIZE; i++)
    {
        if (SRAM_name_array[i] != '\0')
        {   
        (_prof->name)[i] = SRAM_name_array[i];
        }
    }
    
    for (uint8_t i=0; i < NUM_TIME_PARAMS; i++)
    {
        (_prof->Time_array)[i] = SRAM_time_array[i];
    }        
    
    for (uint8_t i=0; i < NUM_TEMP_PARAMS; i++)
    {
        (_prof->Temp_array)[i] = SRAM_temp_array[i];
    }
    
    return;
}


uint8_t get_Profile_time_param(Profile *_prof, uint8_t PARAMETER)
{
    return _prof->Time_array[PARAMETER];
}

uint8_t get_Profile_temp_param(Profile *_prof, uint8_t PARAMETER)
{
    return _prof->Temp_array[PARAMETER];
}