/*
 * reflow_EEPROM.h
 *
 * Created: 10/3/2015 8:45:16 PM
 *  Author: dsgiesbrecht
 */ 


#ifndef REFLOW_EEPROM_H_
#define REFLOW_EEPROM_H_

#include <avr/eeprom.h>

#define NAMESIZE            14
#define NUM_TIME_PARAMS     3
#define NUM_TEMP_PARAMS     4

enum time_params
{
    Preheat_time = 0,
    Soak_time,
    Reflow_time
};

enum temp_params
{
    Preheat_tempMAX = 0,
    Soak_tempMAX,
    Reflow_tempMAX,
    Cooldown_tempMAX
};

#endif /* REFLOW_EEPROM_H_ */