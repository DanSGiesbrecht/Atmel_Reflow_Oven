/*
 * reflow_EEPROM.c
 *
 * Created: 10/3/2015 8:44:03 PM
 *  Author: dsgiesbrecht
 */ 

#include "reflow_EEPROM.h"
#if 1
//moved this to .h file
uint8_t EEMEM profile1name[14] = "EEPROM TEST\0";
uint8_t EEMEM profile1_time_array[NUM_TIME_PARAMS] = {120, 90, 60};
uint8_t EEMEM profile1_temp_array[NUM_TEMP_PARAMS] = {140, 200, 240, 220};

uint8_t *profile1_ptr[] = {profile1name, profile1_time_array, profile1_temp_array};
    
//uint8_t EEMEM profile2_time_array[NUM_TIME_PARAMS] = {XXX, XX, XX};
//uint8_t EEMEM profile2_temp_array[NUM_TEMP_PARAMS] = {XXX, XXX, XXX, XXX};

#endif