/*
 * ProfileFunctions.c
 *
 * Created: 10/3/2015 9:23:01 PM
 *  Author: dsgiesbrecht
 */ 

#include "ProfileFunctions.h"


Profile working_profile;
/* DEFINITIONS */

void update_Profile(Profile *_prof, uint8_t **eeprom_ptr, EEPROM_area select)
{
    /* _ALL: load all values from Name, Time and Temp EEPROM areas, */
    /*       and store them to a Profile struct on the stack.       */
    /* Otherwise: _Name, _Time or _Temp are used to specify which   */
    /*            area of EEPROM to load to the stack.              */
    if (select == _ALL || select == _Name )
    {
        uint8_t SRAM_name_array[NAMESIZE];
        eeprom_read_block((void*)SRAM_name_array, (const void*)(eeprom_ptr[0]), NAMESIZE);
        /* Copy values from temorary SRAM array into working copy of Profile */
        for (uint8_t i=0; i < NAMESIZE; i++)
        {
            if (SRAM_name_array[i] != '\0')
            {
                (_prof->name)[i] = SRAM_name_array[i];
            }
        }
    }
    
    
    if (select == _ALL || select == _Time)
    {
        uint8_t SRAM_time_array[NUM_TIME_PARAMS];
        eeprom_read_block((void*)SRAM_time_array, (const void*)(eeprom_ptr[1]), NUM_TIME_PARAMS);
        /* Copy values from temorary SRAM array into working copy of Profile */
        for (uint8_t i=0; i < NUM_TIME_PARAMS; i++)
        {
            (_prof->Time_array)[i] = SRAM_time_array[i];
        }
    }
    
    
    if (select == _ALL || select == _Temp)
    {
        char SRAM_temp_array[NUM_TEMP_PARAMS];
        eeprom_read_block((void*)SRAM_temp_array, (const void*)(eeprom_ptr[2]), NUM_TEMP_PARAMS);
        /* Copy values from temorary SRAM array into working copy of Profile */
        for (uint8_t i=0; i < NUM_TEMP_PARAMS; i++)
        {
            (_prof->Temp_array)[i] = SRAM_temp_array[i];
        }
    }
    
    return;
}


uint8_t get_Profile_param(Profile *_prof, uint8_t param, EEPROM_area select)
{
    switch (select)
    {
    case _Time:
        return (_prof->Time_array[param]);
    	break;
    case _Temp:
        return (_prof->Temp_array[param]);
        break;
        /* Can't use for name array. */
    case _Name:
        if (select == _Name)
        {
            return 0;
        }
    }
    return 0;
}

void store_EEPROM(uint8_t update_value, uint8_t *eeprom_ptr, uint8_t param)
{
    #warning "Incomplete code"
}    
