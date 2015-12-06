/*
 * reflow_HeaterControl.c
 *
 * Created: 12/6/2015 2:20:06 PM
 *  Author: dsgiesbrecht
 */ 


#include "reflow_HeaterControl.h"
#include "ATMEGA328P_reflow_config.h"

void Heater_Init()
{
    /* Set The heater pin as an output. */
    DDR(HEATER_PORT) |= (1 << HEATER_PIN);
    
}

void HeaterSet(HeaterSetting status)
{
    uint8_t protectREG = HEATER_PORT & (~ (1 << HEATER_PIN) );
    HEATER_PORT = protectREG | (status << HEATER_PIN);
}

void Fan_Init()
{
    /* Set The heater pin as an output. */
    DDR(FAN_PORT) |= (1 << FAN_PIN);
}


void FanSet(FanSetting status)
{
    uint8_t protectREG = FAN_PORT & (~ (1 << FAN_PIN) );
    FAN_PORT = protectREG | (status << FAN_PIN);
}